/*
 * This file is part of the Shiboken Python Bindings Generator project.
 *
 * Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
 *
 * Contact: PySide team <contact@pyside.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "basewrapper.h"
#include "basewrapper_p.h"
#include "sbkconverter.h"
#include "sbkenum.h"
#include "autodecref.h"
#include "typeresolver.h"
#include "gilstate.h"
#include <string>
#include <cstring>
#include <cstddef>
#include <set>
#include <sstream>
#include <algorithm>
#include "threadstatesaver.h"

namespace {
    void _destroyParentInfo(SbkObject* obj, bool keepReference);
}

extern "C"
{

static void SbkObjectTypeDealloc(PyObject* pyObj);
static PyObject* SbkObjectTypeTpNew(PyTypeObject* metatype, PyObject* args, PyObject* kwds);

PyTypeObject SbkObjectType_Type = {
    PyVarObject_HEAD_INIT(0, 0)
    /*tp_name*/             "Shiboken.ObjectType",
    /*tp_basicsize*/        sizeof(SbkObjectType),
    /*tp_itemsize*/         0,
    /*tp_dealloc*/          SbkObjectTypeDealloc,
    /*tp_print*/            0,
    /*tp_getattr*/          0,
    /*tp_setattr*/          0,
    /*tp_compare*/          0,
    /*tp_repr*/             0,
    /*tp_as_number*/        0,
    /*tp_as_sequence*/      0,
    /*tp_as_mapping*/       0,
    /*tp_hash*/             0,
    /*tp_call*/             0,
    /*tp_str*/              0,
    /*tp_getattro*/         0,
    /*tp_setattro*/         PyObject_GenericSetAttr,
    /*tp_as_buffer*/        0,
    /*tp_flags*/            Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE,
    /*tp_doc*/              0,
    /*tp_traverse*/         0,
    /*tp_clear*/            0,
    /*tp_richcompare*/      0,
    /*tp_weaklistoffset*/   0,
    /*tp_iter*/             0,
    /*tp_iternext*/         0,
    /*tp_methods*/          0,
    /*tp_members*/          0,
    /*tp_getset*/           0,
    /*tp_base*/             &PyType_Type,
    /*tp_dict*/             0,
    /*tp_descr_get*/        0,
    /*tp_descr_set*/        0,
    /*tp_dictoffset*/       0,
    /*tp_init*/             0,
    /*tp_alloc*/            PyType_GenericAlloc,
    /*tp_new*/              SbkObjectTypeTpNew,
    /*tp_free*/             PyObject_GC_Del,
    /*tp_is_gc*/            0,
    /*tp_bases*/            0,
    /*tp_mro*/              0,
    /*tp_cache*/            0,
    /*tp_subclasses*/       0,
    /*tp_weaklist*/         0
};

static PyObject* SbkObjectGetDict(SbkObject* obj)
{
    if (!obj->ob_dict)
        obj->ob_dict = PyDict_New();
    if (!obj->ob_dict)
        return 0;
    Py_INCREF(obj->ob_dict);
    return obj->ob_dict;
}

static PyGetSetDef SbkObjectGetSetList[] = {
    {const_cast<char*>("__dict__"), (getter)SbkObjectGetDict, 0},
    {0} // Sentinel
};

static int SbkObject_traverse(PyObject* self, visitproc visit, void* arg)
{
    SbkObject* sbkSelf = reinterpret_cast<SbkObject*>(self);

    //Visit children
    Shiboken::ParentInfo* pInfo = sbkSelf->d->parentInfo;
    if (pInfo) {
        std::set<SbkObject*>::const_iterator it = pInfo->children.begin();
        for(; it != pInfo->children.end(); ++it)
            Py_VISIT(*it);
    }

    //Visit refs
    Shiboken::RefCountMap* rInfo = sbkSelf->d->referredObjects;
    if (rInfo) {
        Shiboken::RefCountMap::const_iterator it = rInfo->begin();
        for (; it != rInfo->end(); ++it) {
            std::list<PyObject*>::const_iterator ref = it->second.begin();
            for(; ref != it->second.end(); ++ref)
                Py_VISIT(*ref);
        }
    }

    if (sbkSelf->ob_dict)
        Py_VISIT(sbkSelf->ob_dict);
    return 0;
}

static int SbkObject_clear(PyObject* self)
{
    SbkObject* sbkSelf = reinterpret_cast<SbkObject*>(self);

    Shiboken::Object::removeParent(sbkSelf);

    if (sbkSelf->d->parentInfo)
        _destroyParentInfo(sbkSelf, true);

    Shiboken::Object::clearReferences(sbkSelf);

    if (sbkSelf->ob_dict)
        Py_CLEAR(sbkSelf->ob_dict);
    return 0;
}

SbkObjectType SbkObject_Type = { { {
    PyVarObject_HEAD_INIT(&SbkObjectType_Type, 0)
    /*tp_name*/             "Shiboken.Object",
    /*tp_basicsize*/        sizeof(SbkObject),
    /*tp_itemsize*/         0,
    /*tp_dealloc*/          SbkDeallocWrapperWithPrivateDtor,
    /*tp_print*/            0,
    /*tp_getattr*/          0,
    /*tp_setattr*/          0,
    /*tp_compare*/          0,
    /*tp_repr*/             0,
    /*tp_as_number*/        0,
    /*tp_as_sequence*/      0,
    /*tp_as_mapping*/       0,
    /*tp_hash*/             0,
    /*tp_call*/             0,
    /*tp_str*/              0,
    /*tp_getattro*/         0,
    /*tp_setattro*/         0,
    /*tp_as_buffer*/        0,
    /*tp_flags*/            Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE|Py_TPFLAGS_HAVE_GC,
    /*tp_doc*/              0,
    /*tp_traverse*/         SbkObject_traverse,
    /*tp_clear*/            SbkObject_clear,
    /*tp_richcompare*/      0,
    /*tp_weaklistoffset*/   offsetof(SbkObject, weakreflist),
    /*tp_iter*/             0,
    /*tp_iternext*/         0,
    /*tp_methods*/          0,
    /*tp_members*/          0,
    /*tp_getset*/           SbkObjectGetSetList,
    /*tp_base*/             0,
    /*tp_dict*/             0,
    /*tp_descr_get*/        0,
    /*tp_descr_set*/        0,
    /*tp_dictoffset*/       offsetof(SbkObject, ob_dict),
    /*tp_init*/             0,
    /*tp_alloc*/            0,
    /*tp_new*/              0,
    /*tp_free*/             0,
    /*tp_is_gc*/            0,
    /*tp_bases*/            0,
    /*tp_mro*/              0,
    /*tp_cache*/            0,
    /*tp_subclasses*/       0,
    /*tp_weaklist*/         0
}, },
    /*priv_data*/           0
};


static void SbkDeallocWrapperCommon(PyObject* pyObj, bool canDelete)
{
    SbkObject* sbkObj = reinterpret_cast<SbkObject*>(pyObj);
    PyTypeObject* pyType = Py_TYPE(pyObj);

    // Need to decref the type if this is the dealloc func; if type
    // is subclassed, that dealloc func will decref (see subtype_dealloc
    // in typeobject.c in the python sources)
    bool needTypeDecref = (pyType->tp_dealloc == SbkDeallocWrapper
                           || pyType->tp_dealloc == SbkDeallocWrapperWithPrivateDtor);

    // Ensure that the GC is no longer tracking this object to avoid a
    // possible reentrancy problem.  Since there are multiple steps involved
    // in deallocating a SbkObject it is possible for the garbage collector to
    // be invoked and it trying to delete this object while it is still in
    // progress from the first time around, resulting in a double delete and a
    // crash.
    PyObject_GC_UnTrack(pyObj);

    // Check that Python is still initialized as sometimes this is called by a static destructor
    // after Python interpeter is shutdown.
    if (sbkObj->weakreflist && Py_IsInitialized())
        PyObject_ClearWeakRefs(pyObj);

    // If I have ownership and is valid delete C++ pointer
    if (canDelete && sbkObj->d->hasOwnership && sbkObj->d->validCppObject) {
        SbkObjectType* sbkType = reinterpret_cast<SbkObjectType*>(pyType);
        if (sbkType->d->is_multicpp) {
            Shiboken::DeallocVisitor visitor(sbkObj);
            Shiboken::walkThroughClassHierarchy(pyObj->ob_type, &visitor);
        } else {
            void* cptr = sbkObj->d->cptr[0];
            Shiboken::Object::deallocData(sbkObj, true);

            Shiboken::ThreadStateSaver threadSaver;
            if (Py_IsInitialized())
                threadSaver.save();
            sbkType->d->cpp_dtor(cptr);
        }
    } else {
        Shiboken::Object::deallocData(sbkObj, true);
    }

    if (needTypeDecref)
        Py_DECREF(pyType);
}

void SbkDeallocWrapper(PyObject* pyObj)
{
    SbkDeallocWrapperCommon(pyObj, true);
}

void SbkDeallocWrapperWithPrivateDtor(PyObject* self)
{
    SbkDeallocWrapperCommon(self, false);
}

void SbkObjectTypeDealloc(PyObject* pyObj)
{
    SbkObjectType* sbkType = reinterpret_cast<SbkObjectType*>(pyObj);

    PyObject_GC_UnTrack(pyObj);
    Py_TRASHCAN_SAFE_BEGIN(pyObj);
    if (sbkType->d) {
        if(sbkType->d->user_data && sbkType->d->d_func) {
            sbkType->d->d_func(sbkType->d->user_data);
            sbkType->d->user_data = 0;
        }
        free(sbkType->d->original_name);
        sbkType->d->original_name = 0;
        if (!Shiboken::ObjectType::isUserType(reinterpret_cast<PyTypeObject*>(sbkType)))
            Shiboken::Conversions::deleteConverter(sbkType->d->converter);
        delete sbkType->d;
        sbkType->d = 0;
    }
    Py_TRASHCAN_SAFE_END(pyObj);
}

PyObject* SbkObjectTypeTpNew(PyTypeObject* metatype, PyObject* args, PyObject* kwds)
{
#ifndef IS_PY3K
    // Check if all bases are new style before calling type.tp_new
    // Was causing gc assert errors in test_bug704.py when
    // this check happened after creating the type object.
    // Argument parsing take from type.tp_new code.
    PyObject* name;
    PyObject* pyBases;
    PyObject* dict;
    static const char* kwlist[] = { "name", "bases", "dict", 0};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "SO!O!:sbktype", (char**)kwlist,
                                     &name,
                                     &PyTuple_Type, &pyBases,
                                     &PyDict_Type, &dict))
        return NULL;

    for(int i=0, i_max=PyTuple_GET_SIZE(pyBases); i < i_max; i++) {
        PyObject* baseType = PyTuple_GET_ITEM(pyBases, i);
        if (PyClass_Check(baseType)) {
            PyErr_Format(PyExc_TypeError, "Invalid base class used in type %s. PySide only support multiple inheritance from python new style class.", metatype->tp_name);
            return 0;
        }
    }
#endif

    // The meta type creates a new type when the Python programmer extends a wrapped C++ class.
    SbkObjectType* newType = reinterpret_cast<SbkObjectType*>(PyType_Type.tp_new(metatype, args, kwds));
    if (!newType)
        return 0;

    Shiboken::ObjectType::initPrivateData(newType);
    SbkObjectTypePrivate* d = newType->d;

    std::list<SbkObjectType*> bases = Shiboken::getCppBaseClasses(reinterpret_cast<PyTypeObject*>(newType));
    if (bases.size() == 1) {
        SbkObjectTypePrivate* parentType = bases.front()->d;
        d->mi_offsets = parentType->mi_offsets;
        d->mi_init = parentType->mi_init;
        d->mi_specialcast = parentType->mi_specialcast;
        d->type_discovery = parentType->type_discovery;
        d->cpp_dtor = parentType->cpp_dtor;
        d->is_multicpp = 0;
        d->converter = parentType->converter;
    } else {
        d->mi_offsets = 0;
        d->mi_init = 0;
        d->mi_specialcast = 0;
        d->type_discovery = 0;
        d->cpp_dtor = 0;
        d->is_multicpp = 1;
        d->converter = 0;
    }
    if (bases.size() == 1)
        d->original_name = strdup(bases.front()->d->original_name);
    else
        d->original_name = strdup("object");
    d->user_data = 0;
    d->d_func = 0;
    d->is_user_type = 1;

    std::list<SbkObjectType*>::const_iterator it = bases.begin();
    for (; it != bases.end(); ++it) {
        if ((*it)->d->subtype_init)
            (*it)->d->subtype_init(newType, args, kwds);
    }

    return reinterpret_cast<PyObject*>(newType);
}

PyObject* SbkObjectTpNew(PyTypeObject* subtype, PyObject*, PyObject*)
{
    SbkObject* self = PyObject_GC_New(SbkObject, subtype);
    Py_INCREF(reinterpret_cast<PyObject*>(subtype));
    SbkObjectPrivate* d = new SbkObjectPrivate;

    SbkObjectType* sbkType = reinterpret_cast<SbkObjectType*>(subtype);
    int numBases = ((sbkType->d && sbkType->d->is_multicpp) ? Shiboken::getNumberOfCppBaseClasses(subtype) : 1);
    d->cptr = new void*[numBases];
    std::memset(d->cptr, 0, sizeof(void*)*numBases);
    d->hasOwnership = 1;
    d->containsCppWrapper = 0;
    d->validCppObject = 0;
    d->parentInfo = 0;
    d->referredObjects = 0;
    d->cppObjectCreated = 0;
    self->ob_dict = 0;
    self->weakreflist = 0;
    self->d = d;
    PyObject_GC_Track(reinterpret_cast<PyObject*>(self));
    return reinterpret_cast<PyObject*>(self);
}


} //extern "C"


namespace
{

void _destroyParentInfo(SbkObject* obj, bool keepReference)
{
    Shiboken::ParentInfo* pInfo = obj->d->parentInfo;
    if (pInfo) {
        while(!pInfo->children.empty()) {
            SbkObject* first = *pInfo->children.begin();
            // Mark child as invalid
            Shiboken::Object::invalidate(first);
            Shiboken::Object::removeParent(first, false, keepReference);
        }
        Shiboken::Object::removeParent(obj, false);
    }
}

}

namespace Shiboken
{

static void decRefPyObjectList(const std::list<PyObject*> &pyObj, PyObject* skip = 0);

static void _walkThroughClassHierarchy(PyTypeObject* currentType, HierarchyVisitor* visitor)
{
    PyObject* bases = currentType->tp_bases;
    Py_ssize_t numBases = PyTuple_GET_SIZE(bases);
    for (int i = 0; i < numBases; ++i) {
        PyTypeObject* type = reinterpret_cast<PyTypeObject*>(PyTuple_GET_ITEM(bases, i));

        if (!PyType_IsSubtype(type, reinterpret_cast<PyTypeObject*>(&SbkObject_Type))) {
            continue;
        } else {
            SbkObjectType* sbkType = reinterpret_cast<SbkObjectType*>(type);
            if (sbkType->d->is_user_type)
                _walkThroughClassHierarchy(type, visitor);
            else
                visitor->visit(sbkType);
        }
        if (visitor->wasFinished())
            break;
    }
}

void walkThroughClassHierarchy(PyTypeObject* currentType, HierarchyVisitor* visitor)
{
    _walkThroughClassHierarchy(currentType, visitor);
    visitor->done();
}


bool importModule(const char* moduleName, PyTypeObject*** cppApiPtr)
{
    PyObject* sysModules = PyImport_GetModuleDict();
    PyObject* module = PyDict_GetItemString(sysModules, moduleName);
    if (!module) {
        module = PyImport_ImportModule(moduleName);
        if (!module)
            return false;
    } else {
        Py_INCREF(module);
    }

    Shiboken::AutoDecRef cppApi(PyObject_GetAttrString(module, "_Cpp_Api"));
    Py_DECREF(module);

    if (cppApi.isNull())
        return false;

#ifdef IS_PY3K
    if (PyCapsule_CheckExact(cppApi))
        *cppApiPtr = reinterpret_cast<PyTypeObject**>(PyCapsule_GetPointer(cppApi, 0));
#else
    // Python 2.6 doesn't have PyCapsule API, so let's keep usign PyCObject on all Python 2.x
    if (PyCObject_Check(cppApi))
        *cppApiPtr = reinterpret_cast<PyTypeObject**>(PyCObject_AsVoidPtr(cppApi));
#endif
    return true;
}

// Wrapper metatype and base type ----------------------------------------------------------

void DtorCallerVisitor::visit(SbkObjectType* node)
{
    m_ptrs.push_back(std::make_pair(m_pyObj->d->cptr[m_ptrs.size()], node));
}

void DtorCallerVisitor::done()
{
    std::list<std::pair<void*, SbkObjectType*> >::const_iterator it = m_ptrs.begin();
    for (; it != m_ptrs.end(); ++it) {
        Shiboken::ThreadStateSaver threadSaver;
        threadSaver.save();
        it->second->d->cpp_dtor(it->first);
    }
}

void DeallocVisitor::done()
{
    Shiboken::Object::deallocData(m_pyObj, true);
    DtorCallerVisitor::done();
}

namespace Module { void init(); }
namespace Conversions { void init(); }

void init()
{
    static bool shibokenAlreadInitialised = false;
    if (shibokenAlreadInitialised)
        return;

    Module::init();
    Conversions::init();

    initTypeResolver();
    PyEval_InitThreads();

    //Init private data
    Shiboken::ObjectType::initPrivateData(&SbkObject_Type);

    if (PyType_Ready(&SbkEnumType_Type) < 0)
        Py_FatalError("[libshiboken] Failed to initialise Shiboken.SbkEnumType metatype.");

    if (PyType_Ready(&SbkObjectType_Type) < 0)
        Py_FatalError("[libshiboken] Failed to initialise Shiboken.BaseWrapperType metatype.");

    if (PyType_Ready((PyTypeObject *)&SbkObject_Type) < 0)
        Py_FatalError("[libshiboken] Failed to initialise Shiboken.BaseWrapper type.");

    shibokenAlreadInitialised = true;
}

void setErrorAboutWrongArguments(PyObject* args, const char* funcName, const char** cppOverloads)
{
    std::string msg;
    std::string params;
    if (args) {
        if (PyTuple_Check(args)) {
            for (int i = 0, max = PyTuple_GET_SIZE(args); i < max; ++i) {
                if (i)
                    params += ", ";
                PyObject* arg = PyTuple_GET_ITEM(args, i);
                params += arg->ob_type->tp_name;
            }
        } else {
            params = args->ob_type->tp_name;
        }
    }

    if (!cppOverloads) {
        msg = "'" + std::string(funcName) + "' called with wrong argument types: " + params;
    } else {
        msg = "'" + std::string(funcName) + "' called with wrong argument types:\n  ";
        msg += funcName;
        msg += '(';
        msg += params;
        msg += ")\n";
        msg += "Supported signatures:";
        for (int i = 0; cppOverloads[i]; ++i) {
            msg += "\n  ";
            msg += funcName;
            msg += '(';
            msg += cppOverloads[i];
            msg += ')';
        }
    }
    PyErr_SetString(PyExc_TypeError, msg.c_str());

}

class FindBaseTypeVisitor : public HierarchyVisitor
{
    public:
        FindBaseTypeVisitor(PyTypeObject* typeToFind) : m_found(false), m_typeToFind(typeToFind) {}
        virtual void visit(SbkObjectType* node)
        {
            if (reinterpret_cast<PyTypeObject*>(node) == m_typeToFind) {
                m_found = true;
                finish();
            }
        }
        bool found() const { return m_found; }

    private:
        bool m_found;
        PyTypeObject* m_typeToFind;
};

std::list<SbkObject*> splitPyObject(PyObject* pyObj)
{
    std::list<SbkObject*> result;
    if (PySequence_Check(pyObj)) {
        AutoDecRef lst(PySequence_Fast(pyObj, "Invalid keep reference object."));
        if (!lst.isNull()) {
            for(int i = 0, i_max = PySequence_Fast_GET_SIZE(lst.object()); i < i_max; i++) {
                PyObject* item = PySequence_Fast_GET_ITEM(lst.object(), i);
                if (Object::checkType(item))
                    result.push_back(reinterpret_cast<SbkObject*>(item));
            }
        }
    } else {
        result.push_back(reinterpret_cast<SbkObject*>(pyObj));
    }
    return result;
}

static void decRefPyObjectList(const std::list<PyObject*>& lst, PyObject *skip)
{
    std::list<PyObject*>::const_iterator iter = lst.begin();
    while(iter != lst.end()) {
        if (*iter != skip)
            Py_DECREF(*iter);
        ++iter;
    }
}

namespace ObjectType
{

bool checkType(PyTypeObject* type)
{
    return PyType_IsSubtype(type, reinterpret_cast<PyTypeObject*>(&SbkObject_Type));
}

bool isUserType(PyTypeObject* type)
{
    return checkType(type) && reinterpret_cast<SbkObjectType*>(type)->d->is_user_type;
}

bool canCallConstructor(PyTypeObject* myType, PyTypeObject* ctorType)
{
    FindBaseTypeVisitor visitor(ctorType);
    walkThroughClassHierarchy(myType, &visitor);
    if (!visitor.found()) {
        PyErr_Format(PyExc_TypeError, "%s isn't a direct base class of %s", ctorType->tp_name, myType->tp_name);
        return false;
    }
    return true;
}


bool hasExternalCppConversions(SbkObjectType*) { return false; }                    // DEPRECATED.
bool isExternalConvertible(SbkObjectType* self, PyObject* obj) { return false; }    // DEPRECATED.
void setExternalCppConversionFunction(SbkObjectType*, ExtendedToCppFunc) {}         // DEPRECATED.
void setExternalIsConvertibleFunction(SbkObjectType*, ExtendedIsConvertibleFunc) {} // DEPRECATED.
void* callExternalCppConversion(SbkObjectType*, PyObject*) { return 0; }            // DEPRECATED.


bool hasCast(SbkObjectType* type)
{
    return type->d->mi_specialcast;
}

void* cast(SbkObjectType* sourceType, SbkObject* obj, PyTypeObject* targetType)
{
    return sourceType->d->mi_specialcast(Object::cppPointer(obj, targetType), reinterpret_cast<SbkObjectType*>(targetType));
}

void setCastFunction(SbkObjectType* type, SpecialCastFunction func)
{
    type->d->mi_specialcast = func;
}

void setOriginalName(SbkObjectType* self, const char* name)
{
    if (self->d->original_name)
        free(self->d->original_name);
    self->d->original_name = strdup(name);
}

const char* getOriginalName(SbkObjectType* self)
{
    return self->d->original_name;
}

void setTypeDiscoveryFunctionV2(SbkObjectType* self, TypeDiscoveryFuncV2 func)
{
    self->d->type_discovery = func;
}

void setTypeDiscoveryFunction(SbkObjectType* self, TypeDiscoveryFunc func)
{
    self->d->type_discovery = (TypeDiscoveryFuncV2)func;
}

TypeDiscoveryFunc getTypeDiscoveryFunction(SbkObjectType* self)
{
    // This is an illegal cast because the return value is different,
    // but nobody ever used this function, so... =]
    return (TypeDiscoveryFunc)self->d->type_discovery;
}

void copyMultimpleheritance(SbkObjectType* self, SbkObjectType* other)
{
    self->d->mi_init = other->d->mi_init;
    self->d->mi_offsets = other->d->mi_offsets;
    self->d->mi_specialcast = other->d->mi_specialcast;
}

void setMultipleIheritanceFunction(SbkObjectType* self, MultipleInheritanceInitFunction function)
{
    self->d->mi_init = function;
}

MultipleInheritanceInitFunction getMultipleIheritanceFunction(SbkObjectType* self)
{
    return self->d->mi_init;
}

void setDestructorFunction(SbkObjectType* self, ObjectDestructor func)
{
    self->d->cpp_dtor = func;
}

void initPrivateData(SbkObjectType* self)
{
    self->d = new SbkObjectTypePrivate;
    memset(self->d, 0, sizeof(SbkObjectTypePrivate));
}

bool introduceWrapperType(PyObject* enclosingObject,
                          const char* typeName, const char* originalName,
                          SbkObjectType* type, ObjectDestructor cppObjDtor,
                          SbkObjectType* baseType, PyObject* baseTypes,
                          bool isInnerClass)
{
    initPrivateData(type);
    setOriginalName(type, originalName);
    setDestructorFunction(type, cppObjDtor);

    if (baseType) {
        type->super.ht_type.tp_base = (PyTypeObject*)baseType;
        if (baseTypes) {
            for (int i = 0; i < PySequence_Fast_GET_SIZE(baseTypes); ++i)
                BindingManager::instance().addClassInheritance((SbkObjectType*)PySequence_Fast_GET_ITEM(baseTypes, i), type);
            type->super.ht_type.tp_bases = baseTypes;
        } else {
            BindingManager::instance().addClassInheritance(baseType, type);
        }
    }

    if (PyType_Ready((PyTypeObject*)type) < 0)
        return false;

    if (isInnerClass)
        return PyDict_SetItemString(enclosingObject, typeName, (PyObject*)type) == 0;

    //PyModule_AddObject steals type's reference.
    Py_INCREF((PyObject*)type);
    return PyModule_AddObject(enclosingObject, typeName, (PyObject*)type) == 0;
}

void setSubTypeInitHook(SbkObjectType* self, SubTypeInitHook func)
{
    self->d->subtype_init = func;
}

void* getTypeUserData(SbkObjectType* self)
{
    return self->d->user_data;
}

void setTypeUserData(SbkObjectType* self, void* userData, DeleteUserDataFunc d_func)
{
    self->d->user_data = userData;
    self->d->d_func = d_func;
}

} // namespace ObjectType


namespace Object
{

static void recursive_invalidate(SbkObject* self, std::set<SbkObject*>& seen);

bool checkType(PyObject* pyObj)
{
    return ObjectType::checkType(pyObj->ob_type);
}

bool isUserType(PyObject* pyObj)
{
    return ObjectType::isUserType(pyObj->ob_type);
}

Py_hash_t hash(PyObject* pyObj)
{
    assert(Shiboken::Object::checkType(pyObj));
    return reinterpret_cast<Py_hash_t>(pyObj);
}

static void setSequenceOwnership(PyObject* pyObj, bool owner)
{
    if (PySequence_Check(pyObj)) {
        std::list<SbkObject*> objs = splitPyObject(pyObj);
        std::list<SbkObject*>::const_iterator it = objs.begin();
        for(; it != objs.end(); ++it) {
            if (owner)
                getOwnership(*it);
            else
                releaseOwnership(*it);
        }
    } else if (Object::checkType(pyObj)) {
        if (owner)
            getOwnership(reinterpret_cast<SbkObject*>(pyObj));
        else
            releaseOwnership(reinterpret_cast<SbkObject*>(pyObj));
    }
}

void setValidCpp(SbkObject* pyObj, bool value)
{
    pyObj->d->validCppObject = value;
}

void setHasCppWrapper(SbkObject* pyObj, bool value)
{
    pyObj->d->containsCppWrapper = value;
}

bool hasCppWrapper(SbkObject* pyObj)
{
    return pyObj->d->containsCppWrapper;
}

bool wasCreatedByPython(SbkObject* pyObj)
{
    return pyObj->d->cppObjectCreated;
}

void callCppDestructors(SbkObject* pyObj)
{
    SbkObjectType* sbkType = reinterpret_cast<SbkObjectType*>(Py_TYPE(pyObj));
    if (sbkType->d->is_multicpp) {
        Shiboken::DtorCallerVisitor visitor(pyObj);
        Shiboken::walkThroughClassHierarchy(Py_TYPE(pyObj), &visitor);
    } else {
        Shiboken::ThreadStateSaver threadSaver;
        threadSaver.save();
        sbkType->d->cpp_dtor(pyObj->d->cptr[0]);
    }

    /* invalidate needs to be called before deleting pointer array because
       it needs to delete entries for them from the BindingManager hash table;
       also release wrapper explicitly if object contains C++ wrapper because
       invalidate doesn't */
    invalidate(pyObj);
    if (pyObj->d->validCppObject && pyObj->d->containsCppWrapper) {
      BindingManager::instance().releaseWrapper(pyObj);
    }

    delete[] pyObj->d->cptr;
    pyObj->d->cptr = 0;
    pyObj->d->validCppObject = false;
}

bool hasOwnership(SbkObject* pyObj)
{
    return pyObj->d->hasOwnership;
}

void getOwnership(SbkObject* self)
{
    // skip if already have the ownership
    if (self->d->hasOwnership)
        return;

    // skip if this object has parent
    if (self->d->parentInfo && self->d->parentInfo->parent)
        return;

    // Get back the ownership
    self->d->hasOwnership = true;

    if (self->d->containsCppWrapper)
        Py_DECREF((PyObject*) self); // Remove extra ref
    else
        makeValid(self); // Make the object valid again
}

void getOwnership(PyObject* pyObj)
{
    if (pyObj)
        setSequenceOwnership(pyObj, true);
}

void releaseOwnership(SbkObject* self)
{
    // skip if the ownership have already moved to c++
    SbkObjectType* selfType = reinterpret_cast<SbkObjectType*>(Py_TYPE(self));
    if (!self->d->hasOwnership || Shiboken::Conversions::pythonTypeIsValueType(selfType->d->converter))
        return;

    // remove object ownership
    self->d->hasOwnership = false;

    // If We have control over object life
    if (self->d->containsCppWrapper)
        Py_INCREF((PyObject*) self); // keep the python object alive until the wrapper destructor call
    else
        invalidate(self); // If I do not know when this object will die We need to invalidate this to avoid use after
}

void releaseOwnership(PyObject* self)
{
    setSequenceOwnership(self, false);
}

/* Needed forward declarations */
static void recursive_invalidate(PyObject* pyobj, std::set<SbkObject*>& seen);
static void recursive_invalidate(SbkObject* self, std::set<SbkObject*>& seen);

void invalidate(PyObject* pyobj)
{
    std::set<SbkObject*> seen;
    recursive_invalidate(pyobj, seen);
}

void invalidate(SbkObject* self)
{
    std::set<SbkObject*> seen;
    recursive_invalidate(self, seen);
}

static void recursive_invalidate(PyObject* pyobj, std::set<SbkObject*>& seen)
{
    std::list<SbkObject*> objs = splitPyObject(pyobj);
    std::list<SbkObject*>::const_iterator it = objs.begin();
    for (; it != objs.end(); it++)
        recursive_invalidate(*it, seen);
}

static void recursive_invalidate(SbkObject* self, std::set<SbkObject*>& seen)
{
    // Skip if this object not is a valid object or if it's already been seen
    if (!self || ((PyObject*)self == Py_None) || seen.find(self) != seen.end())
        return;
    seen.insert(self);

    if (!self->d->containsCppWrapper) {
        self->d->validCppObject = false; // Mark object as invalid only if this is not a wrapper class
        BindingManager::instance().releaseWrapper(self);
    }

    // If it is a parent invalidate all children.
    if (self->d->parentInfo) {
        // Create a copy because this list can be changed during the process
        ChildrenList copy = self->d->parentInfo->children;
        ChildrenList::iterator it = copy.begin();

        for (; it != copy.end(); ++it) {
            // invalidate the child
            recursive_invalidate(*it, seen);

            // if the parent not is a wrapper class, then remove children from him, because We do not know when this object will be destroyed
            if (!self->d->validCppObject)
                removeParent(*it, true, true);
        }
    }

    // If has ref to other objects invalidate all
    if (self->d->referredObjects) {
        RefCountMap& refCountMap = *(self->d->referredObjects);
        RefCountMap::iterator iter;
        for (iter = refCountMap.begin(); iter != refCountMap.end(); ++iter) {
            const std::list<PyObject*> lst = iter->second;
            std::list<PyObject*>::const_iterator it = lst.begin();
            while(it != lst.end()) {
                recursive_invalidate(*it, seen);
                ++it;
            }
        }
    }
}

void makeValid(SbkObject* self)
{
    // Skip if this object not is a valid object
    if (!self || ((PyObject*)self == Py_None) || self->d->validCppObject)
        return;

    // Mark object as invalid only if this is not a wrapper class
    self->d->validCppObject = true;

    // If it is a parent make  all children valid
    if (self->d->parentInfo) {
        ChildrenList::iterator it = self->d->parentInfo->children.begin();
        for (; it != self->d->parentInfo->children.end(); ++it)
            makeValid(*it);
    }

    // If has ref to other objects make all valid again
    if (self->d->referredObjects) {
        RefCountMap& refCountMap = *(self->d->referredObjects);
        RefCountMap::iterator iter;
        for (iter = refCountMap.begin(); iter != refCountMap.end(); ++iter) {
            const std::list<PyObject*> lst = iter->second;
            std::list<PyObject*>::const_iterator it = lst.begin();
            while(it != lst.end()) {
                if (Shiboken::Object::checkType(*it))
                    makeValid(reinterpret_cast<SbkObject*>(*it));
                ++it;
            }
        }
    }
}

bool hasParentInfo(SbkObject* pyObj)
{
    return pyObj->d->parentInfo;
}

void* cppPointer(SbkObject* pyObj, PyTypeObject* desiredType)
{
    PyTypeObject* type = Py_TYPE(pyObj);
    int idx = 0;
    if (reinterpret_cast<SbkObjectType*>(type)->d->is_multicpp)
        idx = getTypeIndexOnHierarchy(type, desiredType);
    if (pyObj->d->cptr)
        return pyObj->d->cptr[idx];
    return 0;
}

std::vector<void*> cppPointers(SbkObject* pyObj)
{
    int n = getNumberOfCppBaseClasses(Py_TYPE(pyObj));
    std::vector<void*> ptrs(n);
    for (int i = 0; i < n; ++i)
        ptrs[i] = pyObj->d->cptr[i];
    return ptrs;
}


bool setCppPointer(SbkObject* sbkObj, PyTypeObject* desiredType, void* cptr)
{
    int idx = 0;
    if (reinterpret_cast<SbkObjectType*>(Py_TYPE(sbkObj))->d->is_multicpp)
        idx = getTypeIndexOnHierarchy(Py_TYPE(sbkObj), desiredType);

    bool alreadyInitialized = sbkObj->d->cptr[idx];
    if (alreadyInitialized)
        PyErr_SetString(PyExc_RuntimeError, "You can't initialize an object twice!");
    else
        sbkObj->d->cptr[idx] = cptr;

    sbkObj->d->cppObjectCreated = true;
    return !alreadyInitialized;
}

bool isValid(PyObject* pyObj)
{
    if (!pyObj || pyObj == Py_None
        || Py_TYPE(pyObj->ob_type) != &SbkObjectType_Type) {
        return true;
    }

    SbkObjectPrivate* priv = reinterpret_cast<SbkObject*>(pyObj)->d;

    if (!priv->cppObjectCreated && isUserType(pyObj)) {
        PyErr_Format(PyExc_RuntimeError, "'__init__' method of object's base class (%s) not called.", pyObj->ob_type->tp_name);
        return false;
    }

    if (!priv->validCppObject) {
        PyErr_Format(PyExc_RuntimeError, "Internal C++ object (%s) already deleted.", pyObj->ob_type->tp_name);
        return false;
    }

    return true;
}

bool isValid(SbkObject* pyObj, bool throwPyError)
{
    if (!pyObj)
        return false;

    SbkObjectPrivate* priv = pyObj->d;
    if (!priv->cppObjectCreated && isUserType(reinterpret_cast<PyObject*>(pyObj))) {
        if (throwPyError)
            PyErr_Format(PyExc_RuntimeError, "Base constructor of the object (%s) not called.", Py_TYPE(pyObj)->tp_name);
        return false;
    }

    if (!priv->validCppObject) {
        if (throwPyError)
            PyErr_Format(PyExc_RuntimeError, "Internal C++ object (%s) already deleted.", Py_TYPE(pyObj)->tp_name);
        return false;
    }

    return true;
}

bool isValid(PyObject* pyObj, bool throwPyError)
{
    if (!pyObj || pyObj == Py_None ||
        !PyType_IsSubtype(pyObj->ob_type, reinterpret_cast<PyTypeObject*>(&SbkObject_Type))) {
        return true;
    }
    return isValid(reinterpret_cast<SbkObject*>(pyObj), throwPyError);
}

PyObject* newObject(SbkObjectType* instanceType,
                    void* cptr,
                    bool hasOwnership,
                    bool isExactType,
                    const char* typeName)
{
    // Try to find the exact type of cptr.
    if (!isExactType) {
        PyTypeObject* exactType = 0;
        if (typeName) {
            exactType = Shiboken::Conversions::getPythonTypeObject(typeName);
            if (exactType)
                instanceType = reinterpret_cast<SbkObjectType*>(exactType);
        }
        if (!exactType)
            instanceType = BindingManager::instance().resolveType(&cptr, instanceType);
    }

    SbkObject* self = reinterpret_cast<SbkObject*>(SbkObjectTpNew(reinterpret_cast<PyTypeObject*>(instanceType), 0, 0));
    self->d->cptr[0] = cptr;
    self->d->hasOwnership = hasOwnership;
    self->d->validCppObject = 1;
    BindingManager::instance().registerWrapper(self, cptr);
    return reinterpret_cast<PyObject*>(self);
}

void destroy(SbkObject* self)
{
    destroy(self, 0);
}

void destroy(SbkObject* self, void* cppData)
{
    // Skip if this is called with NULL pointer this can happen in derived classes
    if (!self)
        return;

    // This can be called in c++ side
    Shiboken::GilState gil;

    // Remove all references attached to this object
    clearReferences(self);

    // Remove the object from parent control

    // Verify if this object has parent
    bool hasParent = (self->d->parentInfo && self->d->parentInfo->parent);

    if (self->d->parentInfo) {
        // Check for children information and make all invalid if they exists
        _destroyParentInfo(self, true);
        // If this object has parent then the pyobject can be invalid now, because we remove the last ref after remove from parent
    }

    //if !hasParent this object could still alive
    if (!hasParent && self->d->containsCppWrapper && !self->d->hasOwnership) {
        // Remove extra ref used by c++ object this will case the pyobject destruction
        // This can cause the object death
        Py_DECREF((PyObject*)self);
    }

    //Python Object is not destroyed yet
    if (cppData && Shiboken::BindingManager::instance().hasWrapper(cppData)) {
        // Remove from BindingManager
        Shiboken::BindingManager::instance().releaseWrapper(self);
        self->d->hasOwnership = false;

        // the cpp object instance was deleted
        delete[] self->d->cptr;
        self->d->cptr = 0;
    }

    // After this point the object can be death do not use the self pointer bellow
}

void removeParent(SbkObject* child, bool giveOwnershipBack, bool keepReference)
{
    ParentInfo* pInfo = child->d->parentInfo;
    if (!pInfo || !pInfo->parent) {
        if (pInfo && pInfo->hasWrapperRef) {
            pInfo->hasWrapperRef = false;
        }
        return;
    }

    ChildrenList& oldBrothers = pInfo->parent->d->parentInfo->children;
    // Verify if this child is part of parent list
    ChildrenList::iterator iChild = std::find(oldBrothers.begin(), oldBrothers.end(), child);
    if (iChild == oldBrothers.end())
        return;

    oldBrothers.erase(iChild);

    pInfo->parent = 0;

    // This will keep the wrapper reference, will wait for wrapper destruction to remove that
    if (keepReference &&
        child->d->containsCppWrapper) {
        //If have already a extra ref remove this one
        if (pInfo->hasWrapperRef)
            Py_DECREF(child);
        else
            pInfo->hasWrapperRef = true;
        return;
    }

    // Transfer ownership back to Python
    child->d->hasOwnership = giveOwnershipBack;

    // Remove parent ref
    Py_DECREF(child);
}

void setParent(PyObject* parent, PyObject* child)
{
    if (!child || child == Py_None || child == parent)
        return;

    /*
     *  setParent is recursive when the child is a native Python sequence, i.e. objects not binded by Shiboken
     *  like tuple and list.
     *
     *  This "limitation" exists to fix the following problem: A class multiple inherits QObject and QString,
     *  so if you pass this class to someone that takes the ownership, we CAN'T enter in this if, but hey! QString
     *  follows the sequence protocol.
     */
    if (PySequence_Check(child) && !Object::checkType(child)) {
        Shiboken::AutoDecRef seq(PySequence_Fast(child, 0));
        for (int i = 0, max = PySequence_Size(seq); i < max; ++i)
            setParent(parent, PySequence_Fast_GET_ITEM(seq.object(), i));
        return;
    }

    bool parentIsNull = !parent || parent == Py_None;
    SbkObject* parent_ = reinterpret_cast<SbkObject*>(parent);
    SbkObject* child_ = reinterpret_cast<SbkObject*>(child);

    if (!parentIsNull) {
        if (!parent_->d->parentInfo)
            parent_->d->parentInfo = new ParentInfo;

        // do not re-add a child
        if (child_->d->parentInfo && (child_->d->parentInfo->parent == parent_))
            return;
    }

    ParentInfo* pInfo = child_->d->parentInfo;
    bool hasAnotherParent = pInfo && pInfo->parent && pInfo->parent != parent_;

    //Avoid destroy child during reparent operation
    Py_INCREF(child);

    // check if we need to remove this child from the old parent
    if (parentIsNull || hasAnotherParent)
        removeParent(child_);

    // Add the child to the new parent
    pInfo = child_->d->parentInfo;
    if (!parentIsNull) {
        if (!pInfo)
            pInfo = child_->d->parentInfo = new ParentInfo;

        pInfo->parent = parent_;
        parent_->d->parentInfo->children.insert(child_);

        // Add Parent ref
        Py_INCREF(child_);

        // Remove ownership
        child_->d->hasOwnership = false;
    }

    // Remove previous safe ref
    Py_DECREF(child);
}

void deallocData(SbkObject* self, bool cleanup)
{
    // Make cleanup if this is not a wrapper otherwise this will be done on wrapper destructor
    if(cleanup) {
        removeParent(self);

        if (self->d->parentInfo)
            _destroyParentInfo(self, true);

        clearReferences(self);
    }

    if (self->d->cptr) {
        // Remove from BindingManager
        Shiboken::BindingManager::instance().releaseWrapper(self);
        delete[] self->d->cptr;
        self->d->cptr = 0;
        delete self->d;
    }
    Py_XDECREF(self->ob_dict);
    Py_TYPE(self)->tp_free(self);
}

void setTypeUserData(SbkObject* wrapper, void* userData, DeleteUserDataFunc d_func)
{
    SbkObjectType* ob_type = reinterpret_cast<SbkObjectType*>(Py_TYPE(wrapper));
    if (ob_type->d->user_data)
        ob_type->d->d_func(ob_type->d->user_data);

    ob_type->d->d_func = d_func;
    ob_type->d->user_data = userData;
}

void* getTypeUserData(SbkObject* wrapper)
{
    return reinterpret_cast<SbkObjectType*>(Py_TYPE(wrapper))->d->user_data;
}

void keepReference(SbkObject* self, const char* key, PyObject* referredObject, bool append)
{
    bool isNone = (!referredObject || (referredObject == Py_None));

    if (!self->d->referredObjects)
        self->d->referredObjects = new Shiboken::RefCountMap;

    RefCountMap& refCountMap = *(self->d->referredObjects);
    RefCountMap::iterator iter = refCountMap.find(key);
    std::list<PyObject*> objects;
    if (iter != refCountMap.end()) {
        objects = (*iter).second;
        std::list<PyObject*>::const_iterator found = std::find(objects.begin(), objects.end(), referredObject);

        // skip if objects already exists
        if (found != objects.end())
            return;
    }

    if (append && !isNone) {
        refCountMap[key].push_back(referredObject);
        Py_INCREF(referredObject);
    } else if (!append) {
        if (objects.size() > 0)
            decRefPyObjectList(objects, isNone ? 0 : referredObject);
        if (isNone) {
            if (iter != refCountMap.end())
                refCountMap.erase(iter);
        } else {
            objects.clear();
            objects.push_back(referredObject);
            refCountMap[key] = objects;
            Py_INCREF(referredObject);
        }
    }
}

void removeReference(SbkObject* self, const char* key, PyObject* referredObject)
{
    if (!referredObject || (referredObject == Py_None))
        return;

    if (!self->d->referredObjects)
        return;

    RefCountMap& refCountMap = *(self->d->referredObjects);
    RefCountMap::iterator iter = refCountMap.find(key);
    if (iter != refCountMap.end()) {
        decRefPyObjectList(iter->second);
        refCountMap.erase(iter);
    }
}

void clearReferences(SbkObject* self)
{
    if (!self->d->referredObjects)
        return;

    RefCountMap& refCountMap = *(self->d->referredObjects);
    RefCountMap::iterator iter;
    for (iter = refCountMap.begin(); iter != refCountMap.end(); ++iter)
        decRefPyObjectList(iter->second);
    self->d->referredObjects->clear();
}

std::string info(SbkObject* self)
{
    std::ostringstream s;
    std::list<SbkObjectType*> bases;

    if (self->d && self->d->cptr) {
        if (ObjectType::isUserType(Py_TYPE(self)))
            bases = getCppBaseClasses(Py_TYPE(self));
        else
            bases.push_back(reinterpret_cast<SbkObjectType*>(Py_TYPE(self)));

        s << "C++ address....... ";
        std::list<SbkObjectType*>::const_iterator it = bases.begin();
        for (int i = 0; it != bases.end(); ++it, ++i)
            s << ((PyTypeObject*)*it)->tp_name << "/" << self->d->cptr[i] << ' ';
        s << "\n";
    }
    else {
        s << "C++ address....... <<Deleted>>\n";
    }

    s << "hasOwnership...... " << bool(self->d->hasOwnership) << "\n"
         "containsCppWrapper " << self->d->containsCppWrapper << "\n"
         "validCppObject.... " << self->d->validCppObject << "\n"
         "wasCreatedByPython " << self->d->cppObjectCreated << "\n";


    if (self->d->parentInfo && self->d->parentInfo->parent) {
        s << "parent............ ";
        Shiboken::AutoDecRef parent(PyObject_Str((PyObject*)self->d->parentInfo->parent));
        s << String::toCString(parent) << "\n";
    }

    if (self->d->parentInfo && self->d->parentInfo->children.size()) {
        s << "children.......... ";
        ChildrenList& children = self->d->parentInfo->children;
        for (ChildrenList::const_iterator it = children.begin(); it != children.end(); ++it) {
            Shiboken::AutoDecRef child(PyObject_Str((PyObject*)*it));
            s << String::toCString(child) << ' ';
        }
        s << '\n';
    }

    if (self->d->referredObjects && self->d->referredObjects->size()) {
        Shiboken::RefCountMap& map = *self->d->referredObjects;
        s << "referred objects.. ";
        Shiboken::RefCountMap::const_iterator it = map.begin();
        for (; it != map.end(); ++it) {
            if (it != map.begin())
                s << "                   ";
            s << '"' << it->first << "\" => ";
            std::list<PyObject*>::const_iterator j = it->second.begin();
            for (; j != it->second.end(); ++j) {
                Shiboken::AutoDecRef obj(PyObject_Str(*j));
                s << String::toCString(obj) << ' ';
            }
            s << ' ';
        }
        s << '\n';
    }
    return s.str();
}

} // namespace Object

} // namespace Shiboken
