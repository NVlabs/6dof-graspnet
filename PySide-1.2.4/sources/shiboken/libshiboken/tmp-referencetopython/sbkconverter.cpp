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

#include "sbkconverter.h"
#include "sbkconverter_p.h"
#include "basewrapper_p.h"

#include "sbkdbg.h"

namespace Shiboken {
namespace Conversions {

static SbkConverter* createConverterObject(PyTypeObject* type,
                                           PythonToCppFunc toCppPointerConvFunc,
                                           IsConvertibleToCppFunc toCppPointerCheckFunc,
                                           CppToPythonFunc pointerToPythonFunc,
                                           CppToPythonFunc copyToPythonFunc)
{
    SbkConverter* converter = new SbkConverter;
    converter->pythonType = type;

    converter->pointerToPython = pointerToPythonFunc;
    converter->copyToPython = copyToPythonFunc;

    converter->toCppPointerConversion = std::make_pair(toCppPointerCheckFunc, toCppPointerConvFunc);
    converter->toCppConversions.clear();

    return converter;
}

SbkConverter* createConverter(SbkObjectType* type,
                              PythonToCppFunc toCppPointerConvFunc,
                              IsConvertibleToCppFunc toCppPointerCheckFunc,
                              CppToPythonFunc pointerToPythonFunc,
                              CppToPythonFunc copyToPythonFunc)
{
    SbkConverter* converter = createConverterObject((PyTypeObject*)type,
                                                    toCppPointerConvFunc, toCppPointerCheckFunc,
                                                    pointerToPythonFunc, copyToPythonFunc);
    type->d->converter = converter;
    return converter;
}

void deleteConverter(SbkConverter* converter)
{
    if (converter) {
        converter->toCppConversions.clear();
        delete converter;
    }
}

void addPythonToCppValueConversion(SbkConverter* converter,
                                   PythonToCppFunc pythonToCppFunc,
                                   IsConvertibleToCppFunc isConvertibleToCppFunc)
{
    converter->toCppConversions.push_back(std::make_pair(isConvertibleToCppFunc, pythonToCppFunc));
}
void addPythonToCppValueConversion(SbkObjectType* type,
                                   PythonToCppFunc pythonToCppFunc,
                                   IsConvertibleToCppFunc isConvertibleToCppFunc)
{
    addPythonToCppValueConversion(type->d->converter, pythonToCppFunc, isConvertibleToCppFunc);
}

PyObject* pointerToPython(SbkObjectType* type, const void* cppIn)
{
    if (!cppIn)
        Py_RETURN_NONE;
    return type->d->converter->pointerToPython(cppIn);
}

static inline PyObject* CopyCppToPython(SbkConverter* converter, const void* cppIn)
{
    assert(cppIn);
    return converter->copyToPython(cppIn);
}
PyObject* copyToPython(SbkObjectType* type, const void* cppIn)
{
    return CopyCppToPython(type->d->converter, cppIn);
}
PyObject* toPython(SbkConverter* converter, const void* cppIn)
{
    return CopyCppToPython(converter, cppIn);
}

PyObject* referenceToPython(SbkObjectType* type, const void* cppIn)
{
    assert(cppIn);
    PyObject* pyOut = (PyObject*)BindingManager::instance().retrieveWrapper(cppIn);
    if (pyOut) {
        Py_INCREF(pyOut);
        return pyOut;
    }
    // If it is Value Type, return a copy of the C++ object.
    if (type->d->converter->copyToPython)
        return type->d->converter->copyToPython(cppIn);
    // If it is an Object Type, return a copy of the C++ object.
    return type->d->converter->pointerToPython(cppIn);
}

PythonToCppFunc isPythonToCppPointerConvertible(SbkObjectType* type, PyObject* pyIn)
{
    assert(pyIn);
    return type->d->converter->toCppPointerConversion.first(pyIn);
}

static inline PythonToCppFunc IsPythonToCppConvertible(SbkConverter* converter, PyObject* pyIn)
{
    assert(pyIn);
    ToCppConversionList& convs = converter->toCppConversions;
    for (ToCppConversionList::iterator conv = convs.begin(); conv != convs.end(); ++conv) {
        PythonToCppFunc toCppFunc = 0;
        if ((toCppFunc = (*conv).first(pyIn)))
            return toCppFunc;
    }
    return 0;
}
PythonToCppFunc isPythonToCppValueConvertible(SbkObjectType* type, PyObject* pyIn)
{
    return IsPythonToCppConvertible(type->d->converter, pyIn);
}
PythonToCppFunc isPythonToCppConvertible(SbkConverter* converter, PyObject* pyIn)
{
    return IsPythonToCppConvertible(converter, pyIn);
}

PythonToCppFunc isPythonToCppReferenceConvertible(SbkObjectType* type, PyObject* pyIn)
{
    if (pyIn != Py_None) {
        PythonToCppFunc toCpp = isPythonToCppPointerConvertible(type, pyIn);
        if (toCpp)
            return toCpp;
    }
    return isPythonToCppValueConvertible(type, pyIn);
}

void nonePythonToCppNullPtr(PyObject*, void* cppOut)
{
    assert(cppOut);
    *((void**)cppOut) = 0;
}

void pythonToCppPointer(SbkObjectType* type, PyObject* pyIn, void* cppOut)
{
    assert(pyIn);
    assert(cppOut);
    SbkObjectType* inType = (SbkObjectType*)pyIn->ob_type;
    if (ObjectType::hasCast(inType))
        *((void**)cppOut) = ObjectType::cast(inType, (SbkObject*)pyIn, (PyTypeObject*)type);
    else
        *((void**)cppOut) = Object::cppPointer((SbkObject*)pyIn, (PyTypeObject*)type);
}

bool isImplicitConversion(SbkObjectType* type, PythonToCppFunc toCppFunc)
{
    // This is the Object Type or Value Type conversion that only
    // retrieves the C++ pointer held in the Python wrapper.
    if (toCppFunc == type->d->converter->toCppPointerConversion.second)
        return false;

    // Object Types doesn't have any kind of value conversion,
    // only C++ pointer retrieval.
    if (type->d->converter->toCppConversions.empty())
        return false;

    // The first conversion of the non-pointer conversion list is
    // a Value Type's copy to C++ function, which is not an implicit
    // conversion.
    // Otherwise it must be one of the implicit conversions.
    // Note that we don't check if the Python to C++ conversion is in
    // the list of the type's conversions, for it is expected that the
    // caller knows what he's doing.
    ToCppConversionList::iterator conv = type->d->converter->toCppConversions.begin();
    return toCppFunc != (*conv).second;
}

} } // namespace Shiboken::Conversions
