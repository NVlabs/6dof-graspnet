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

#ifndef SBK_CONVERTER_H
#define SBK_CONVERTER_H

#include <limits>
#include <Python.h>
#include "shibokenmacros.h"
#include "basewrapper.h"

extern "C"
{

/**
 *  SbkConverter is used to perform type conversions from C++
 *  to Python and vice-versa;.and it is also used for type checking.
 *  SbkConverter is a private structure that must be accessed
 *  using the functions provided by the converter API.
 */
struct SbkConverter;

/**
 *  Given a void pointer to a C++ object, this function must return
 *  the proper Python object. It may be either an existing wrapper
 *  for the C++ object, or a newly create one. Or even the Python
 *  equivalent of the C++ value passed in the argument.
 *
 *  C++ -> Python
 */
typedef PyObject* (*CppToPythonFunc)(const void*);

/**
 *  This function converts a Python object to a C++ value, it may be
 *  a pointer, value, class, container or primitive type, passed via
 *  a void pointer, that will be cast properly inside the function.
 *  This function is usually returned by an IsConvertibleToCppFunc
 *  function, or obtained knowing the type of the Python object input,
 *  thus it will not check the Python object type, and will expect
 *  the void pointer to be pointing to a proper variable.
 *
 *  Python -> C++
 */
typedef void (*PythonToCppFunc)(PyObject*,void*);

/**
 *  Checks if the Python object passed in the argument is convertible to a
 *  C++ type defined inside the function, it returns the converter function
 *  that will transform a Python argument into a C++ value.
 *  It returns NULL if the Python object is not convertible to the C++ type
 *  that the function represents.
 *
 *  Python -> C++ ?
 */
typedef PythonToCppFunc (*IsConvertibleToCppFunc)(PyObject*);

} // extern "C"


namespace Shiboken {
namespace Conversions {

/**
 *  Creates a converter for a wrapper type.
 *  \param type                  A Shiboken.ObjectType that will receive the new converter.
 *  \param toCppPointerConvFunc  Function to retrieve the C++ pointer held by a Python wrapper.
 *  \param toCppPointerCheckFunc Check and return the retriever function of the C++ pointer held by a Python wrapper.
 *  \param pointerToPythonFunc   Function to convert a C++ object to a Python \p type wrapper, keeping their identity.
 *  \param copyToPythonFunc      Function to convert a C++ object to a Python \p type, copying the object.
 *  \returns                     The new converter referred by the wrapper \p type.
 */
LIBSHIBOKEN_API SbkConverter* createConverter(SbkObjectType* type,
                                              PythonToCppFunc toCppPointerConvFunc,
                                              IsConvertibleToCppFunc toCppPointerCheckFunc,
                                              CppToPythonFunc pointerToPythonFunc,
                                              CppToPythonFunc copyToPythonFunc = 0);

LIBSHIBOKEN_API void deleteConverter(SbkConverter* converter);

/**
 *  Adds a new conversion of a Python object to a C++ value.
 *  This is used in copy and implicit conversions.
 */
LIBSHIBOKEN_API void addPythonToCppValueConversion(SbkConverter* converter,
                                                   PythonToCppFunc pythonToCppFunc,
                                                   IsConvertibleToCppFunc isConvertibleToCppFunc);
LIBSHIBOKEN_API void addPythonToCppValueConversion(SbkObjectType* type,
                                                   PythonToCppFunc pythonToCppFunc,
                                                   IsConvertibleToCppFunc isConvertibleToCppFunc);

// C++ -> Python ---------------------------------------------------------------------------

/**
 *  Retrieves the Python wrapper object for the given \p cppIn C++ pointer object.
 *  This function is used only for Value and Object Types.
 *  Example usage:
 *      TYPE* var;
 *      PyObject* pyVar = pointerToPython(SBKTYPE, &var);
 */
LIBSHIBOKEN_API PyObject* pointerToPython(SbkObjectType* type, const void* cppIn);

/**
 *  Retrieves the Python wrapper object for the given C++ value pointed by \p cppIn.
 *  This function is used only for Value Types.
 *  Example usage:
 *      TYPE var;
 *      PyObject* pyVar = copyToPython(SBKTYPE, &var);
 */
LIBSHIBOKEN_API PyObject* copyToPython(SbkObjectType* type, const void* cppIn);

// TODO:WRITEDOCSTRING - used only for Value Types - cppIn must point to a value
/**
 *  Retrieves the Python wrapper object for the given C++ reference pointed by \p cppIn.
 *  This function is used only for Value and Object Types.
 *  It differs from pointerToPython() for not checking for a NULL pointer.
 *  Example usage:
 *      TYPE& var = SOMETHING;
 *      PyObject* pyVar = copyToPython(SBKTYPE, &var);
 */
LIBSHIBOKEN_API PyObject* referenceToPython(SbkObjectType* type, const void* cppIn);

// TODO:WRITEDOCSTRING - used only for Primitives and Containers (and Value Types) - cppIn must point to a primitive, container or value type
/// This is the same as copyToPython function.
LIBSHIBOKEN_API PyObject* toPython(SbkConverter* converter, const void* cppIn);

// Python -> C++ convertibility checks -----------------------------------------------------

// TODO:WRITEDOCSTRING
LIBSHIBOKEN_API PythonToCppFunc isPythonToCppPointerConvertible(SbkObjectType* type, PyObject* pyIn);

// TODO:WRITEDOCSTRING- Returns a Python to C++ conversion function if true, or NULL if false.
LIBSHIBOKEN_API PythonToCppFunc isPythonToCppValueConvertible(SbkObjectType* type, PyObject* pyIn);

// TODO:WRITEDOCSTRING- Returns a Python to C++ conversion function if true, or NULL if false.
LIBSHIBOKEN_API PythonToCppFunc isPythonToCppReferenceConvertible(SbkObjectType* type, PyObject* pyIn);

/// This is the same as isPythonToCppValueConvertible function.
LIBSHIBOKEN_API PythonToCppFunc isPythonToCppConvertible(SbkConverter* converter, PyObject* pyIn);

// Python -> C++ ---------------------------------------------------------------------------

// TODO:WRITEDOCSTRING - function used by the generated [TYPE]_PythonToCpp_[TYPE]_PTR
LIBSHIBOKEN_API void pythonToCppPointer(SbkObjectType* type, PyObject* pyIn, void* cppOut);

// TODO:WRITEDOCSTRING - function used by the generated isConvertible when the PyObject is None,
// making a C++ NULL pointer the result of the toCpp function call.
// DRAFT: When the Python object is a Py_None, it's C++ conversion is always a NULL pointer.
LIBSHIBOKEN_API void nonePythonToCppNullPtr(PyObject*, void* cppOut);

// TODO:WRITEDOCSTRING - tells if \p toCpp is an implicit conversion.
LIBSHIBOKEN_API bool isImplicitConversion(SbkObjectType* type, PythonToCppFunc toCpp);

} } // namespace Shiboken::Conversions

#endif // SBK_CONVERTER_H
