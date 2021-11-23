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

#include "typeresolver.h"
#include "google/dense_hash_map"
#include "sbkdbg.h"
#include <cstdlib>
#include <string>
#include "basewrapper_p.h"

using namespace Shiboken;

typedef google::dense_hash_map<std::string, TypeResolver*> TypeResolverMap;
static TypeResolverMap typeResolverMap;

struct TypeResolver::TypeResolverPrivate
{
    CppToPythonFunc cppToPython;
    PythonToCppFunc pythonToCpp;
    PyTypeObject* pyType;
};

static void deinitTypeResolver()
{
    for (TypeResolverMap::const_iterator it = typeResolverMap.begin(); it != typeResolverMap.end(); ++it)
        delete it->second;
    typeResolverMap.clear();
}

void Shiboken::initTypeResolver()
{
    assert(typeResolverMap.empty());
    typeResolverMap.set_empty_key("");
    typeResolverMap.set_deleted_key("?");
    std::atexit(deinitTypeResolver);
}

TypeResolver::TypeResolver() : m_d(new TypeResolverPrivate)
{
}

TypeResolver* TypeResolver::createTypeResolver(const char* typeName,
                                               CppToPythonFunc cppToPy,
                                               PythonToCppFunc pyToCpp,
                                               PyTypeObject* pyType)
{
    TypeResolver*& tr = typeResolverMap[typeName];
    if (!tr) {
        tr = new TypeResolver;
        tr->m_d->cppToPython = cppToPy;
        tr->m_d->pythonToCpp = pyToCpp;
        tr->m_d->pyType = pyType;

        /*
         * Note:
         *
         *     Value types are also registered as object types, but the generator *always* first register the value
         *     type version in the TypeResolver and it *must* always do it! otherwise this code wont work.
         */
        if (pyType && PyType_IsSubtype(pyType, reinterpret_cast<PyTypeObject*>(&SbkObject_Type))) {
            SbkObjectType* sbkType = reinterpret_cast<SbkObjectType*>(pyType);
            // TODO-CONVERTERS: to be deprecated
            if (!sbkType->d->type_behaviour) {
                int len = strlen(typeName);
                sbkType->d->type_behaviour = typeName[len -1] == '*' ? BEHAVIOUR_OBJECTTYPE : BEHAVIOUR_VALUETYPE;
            }
        }
    }
    return tr;
}

TypeResolver::~TypeResolver()
{
    delete m_d;
}

TypeResolver* TypeResolver::get(const char* typeName)
{
    TypeResolverMap::const_iterator it = typeResolverMap.find(typeName);
    if (it != typeResolverMap.end()) {
        return it->second;
    } else {
        if (Py_VerboseFlag > 0)
            SbkDbg() << "Can't find type resolver for " << typeName;
        return 0;
    }
}

void TypeResolver::toCpp(PyObject* pyObj, void** place)
{
    m_d->pythonToCpp(pyObj, place);
}

PyObject* TypeResolver::toPython(void* cppObj)
{
    return m_d->cppToPython(cppObj);
}

PyTypeObject* TypeResolver::pythonType()
{
    return m_d->pyType;
}

TypeResolver::Type TypeResolver::getType(const char* name)
{
    int len = strlen(name);
    bool isObjTypeName = name[len - 1] == '*';
    if (TypeResolver::get(name)) {
        // great, we found the type in our first attempt!
        return isObjTypeName ? ObjectType : ValueType;
    } else {
        // Type not found... let's copy the string.
        std::string typeName(name);
        if (isObjTypeName)
            typeName.erase(len - 1, 1);
        else
            typeName += '*';
        isObjTypeName = !isObjTypeName;

        if (TypeResolver::get(typeName.c_str()))
            return isObjTypeName ? ObjectType : ValueType;
        else
            return UnknownType;
    }
}

