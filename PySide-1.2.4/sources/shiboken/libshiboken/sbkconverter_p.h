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

#ifndef SBK_CONVERTER_P_H
#define SBK_CONVERTER_P_H

#include "sbkpython.h"
#include "sbkconverter.h"
#include "sbkstring.h"
#include <list>
#include <limits>

#include "sbkdbg.h"

extern "C"
{

typedef std::pair<IsConvertibleToCppFunc, PythonToCppFunc> ToCppConversion;
typedef std::list<ToCppConversion> ToCppConversionList;

/**
 *  \internal
 *  Private structure of SbkConverter.
 */
struct SbkConverter
{
    /**
     *  Python type associated with this converter. If the type is a Shiboken
     *  wrapper, then it must be a SbkObjectType; otherwise it will be the
     *  Python type to which the C++ value will be converted (note that the
     *  C++ type could be produced from various Python types).
     */
    PyTypeObject*           pythonType;
    /**
     *  This function converts a C++ object to a Python object of the type
     *  indicated in pythonType. The identity of the C++ object is kept,
     *  because it looks for an already existing Python wrapper associated
     *  with the C++ instance.
     *  It is used to convert C++ pointers and references to Python objects.
     */
    CppToPythonFunc         pointerToPython;
    /**
     *  This function converts a C++ object to a Python object of the type
     *  indicated in pythonType. The identity of the object is not kept,
     *  because a new instance of the C++ object is created.
     *  It is used to convert objects passed by value, or reference, if said
     *  reference can't be traced to an object that already has a Python
     *  wrapper assigned for it.
     */
    CppToPythonFunc         copyToPython;
    /**
     *  This is a special case of a Python to C++ conversion. It returns
     *  the underlying C++ pointer of a Python wrapper passed as parameter
     *  or NULL if the Python object is a None value.
     *  It comes separated from the other toCppConversions because if you
     *  have a Python object representing a Value Type the type checking
     *  for both ValueType and ValueType* would be the same, thus the first
     *  check would be true and the second would never be reached.
     */
    ToCppConversion         toCppPointerConversion;
    /**
     *  This is a list of type checking functions that return the
     *  proper Python to C++ conversion function, for the given Python
     *  object.
     *  For Object Types, that never have implicit conversions, this
     *  list is always empty.
     */
    ToCppConversionList     toCppConversions;
};

} // extern "C"

// Helper template for checking if a value overflows when cast to type T.
template<typename T, bool isSigned = std::numeric_limits<T>::is_signed >
struct OverFlowChecker;

template<typename T>
struct OverFlowChecker<T, true> {
    static bool check(const PY_LONG_LONG& value) {
        return value < std::numeric_limits<T>::min() || value > std::numeric_limits<T>::max();
    }
};
template<typename T>
struct OverFlowChecker<T, false> {
    static bool check(const PY_LONG_LONG& value) {
        return value < 0 || static_cast<unsigned long long>(value) > std::numeric_limits<T>::max();
    }
};
template<>
struct OverFlowChecker<PY_LONG_LONG, true> {
    static bool check(const PY_LONG_LONG& value) { return false; }
};
template<>
struct OverFlowChecker<double, true> {
    static bool check(const double& value) { return false; }
};
template<>
struct OverFlowChecker<float, true> {
    static bool check(const double& value) {
        return value < std::numeric_limits<float>::min() || value > std::numeric_limits<float>::max();
    }
};

// Basic primitive type converters ---------------------------------------------------------
template<typename T> PyTypeObject* SbkType() { return 0; }
template<> inline PyTypeObject* SbkType<PY_LONG_LONG>() { return &PyLong_Type; }
template<> inline PyTypeObject* SbkType<bool>() { return &PyBool_Type; }
template<> inline PyTypeObject* SbkType<char>() { return &PyInt_Type; }
template<> inline PyTypeObject* SbkType<const char*>() { return &PyString_Type; }
template<> inline PyTypeObject* SbkType<double>() { return &PyFloat_Type; }
template<> inline PyTypeObject* SbkType<float>() { return &PyFloat_Type; }
template<> inline PyTypeObject* SbkType<int>() { return &PyInt_Type; }
template<> inline PyTypeObject* SbkType<long>() { return &PyLong_Type; }
template<> inline PyTypeObject* SbkType<short>() { return &PyInt_Type; }
template<> inline PyTypeObject* SbkType<signed char>() { return &PyInt_Type; }
template<> inline PyTypeObject* SbkType<unsigned PY_LONG_LONG>() { return &PyLong_Type; }
template<> inline PyTypeObject* SbkType<unsigned char>() { return &PyInt_Type; }
template<> inline PyTypeObject* SbkType<unsigned int>() { return &PyLong_Type; }
template<> inline PyTypeObject* SbkType<unsigned long>() { return &PyLong_Type; }
template<> inline PyTypeObject* SbkType<unsigned short>() { return &PyInt_Type; }

template <typename T> struct Primitive {};

template <typename T>
struct OnePrimitive
{
    static PyObject* toPython(const void*) { return 0; }
    static PythonToCppFunc isConvertible(PyObject*) { return 0; }
    static void toCpp(PyObject*, void*) {}
    static SbkConverter* createConverter()
    {
        SbkConverter* converter = Shiboken::Conversions::createConverter(SbkType<T>(), Primitive<T>::toPython);
        Shiboken::Conversions::addPythonToCppValueConversion(converter, Primitive<T>::toCpp, Primitive<T>::isConvertible);
        return converter;
    }
};
template <typename T>
struct TwoPrimitive : OnePrimitive<T>
{
    static PythonToCppFunc isOtherConvertible(PyObject*) { return 0; }
    static void otherToCpp(PyObject*, void*) {}
    static SbkConverter* createConverter()
    {
        SbkConverter* converter = OnePrimitive<T>::createConverter();
        Shiboken::Conversions::addPythonToCppValueConversion(converter, Primitive<T>::otherToCpp, Primitive<T>::isOtherConvertible);
        return converter;
    }
};

// Integers --------------------------------------------------------------------------------

template <typename INT>
struct IntPrimitive : TwoPrimitive<INT>
{
    static PyObject* toPython(const void* cppIn)
    {
        return PyInt_FromLong((long)*((INT*)cppIn));
    }
    static void toCpp(PyObject* pyIn, void* cppOut)
    {
        double result = PyFloat_AS_DOUBLE(pyIn);
        // If cast to long directly it could overflow silently.
        if (OverFlowChecker<INT>::check(result))
            PyErr_SetObject(PyExc_OverflowError, 0);
        *((INT*)cppOut) = static_cast<INT>(result);
    }
    static PythonToCppFunc isConvertible(PyObject* pyIn)
    {
        if (PyFloat_Check(pyIn))
            return toCpp;
        return 0;
    }
    static void otherToCpp(PyObject* pyIn, void* cppOut)
    {
        PY_LONG_LONG result = PyLong_AsLongLong(pyIn);
        if (OverFlowChecker<INT>::check(result))
            PyErr_SetObject(PyExc_OverflowError, 0);
        *((INT*)cppOut) = static_cast<INT>(result);
    }
    static PythonToCppFunc isOtherConvertible(PyObject* pyIn)
    {
        if (SbkNumber_Check(pyIn))
            return otherToCpp;
        return 0;
    }
};
template <> struct Primitive<int> : IntPrimitive<int> {};
template <> struct Primitive<long> : IntPrimitive<long> {};
template <> struct Primitive<short> : IntPrimitive<short> {};
template <> struct Primitive<unsigned short> : IntPrimitive<unsigned short> {};

// Unsigned Long Integers ------------------------------------------------------------------

template <typename LONG>
struct UnsignedLongPrimitive : IntPrimitive<LONG>
{
    static PyObject* toPython(const void* cppIn)
    {
        return PyLong_FromUnsignedLong(*((LONG*)cppIn));
    }
};
template <> struct Primitive<unsigned int> : UnsignedLongPrimitive<unsigned int> {};
template <> struct Primitive<unsigned long> : UnsignedLongPrimitive<unsigned long> {};

// Big integers ----------------------------------------------------------------------------

template <>
struct Primitive<PY_LONG_LONG> : OnePrimitive<PY_LONG_LONG>
{
    static PyObject* toPython(const void* cppIn)
    {
        return PyLong_FromLongLong(*((PY_LONG_LONG*)cppIn));
    }
    static void toCpp(PyObject* pyIn, void* cppOut)
    {
        *((PY_LONG_LONG*)cppOut) = (PY_LONG_LONG) PyLong_AsLongLong(pyIn);
    }
    static PythonToCppFunc isConvertible(PyObject* pyIn)
    {
        if (SbkNumber_Check(pyIn))
            return toCpp;
        return 0;
    }
};

template <>
struct Primitive<unsigned PY_LONG_LONG> : OnePrimitive<unsigned PY_LONG_LONG>
{
    static PyObject* toPython(const void* cppIn)
    {
        return PyLong_FromUnsignedLongLong(*((unsigned PY_LONG_LONG*)cppIn));
    }
    static void toCpp(PyObject* pyIn, void* cppOut)
    {
        if (PyInt_Check(pyIn)) {
            long result = (unsigned PY_LONG_LONG) PyInt_AsLong(pyIn);
            if (result < 0)
                PyErr_SetObject(PyExc_OverflowError, 0);
            else
                *((unsigned PY_LONG_LONG*)cppOut) = (unsigned PY_LONG_LONG) result;
        } else if (PyLong_Check(pyIn)) {
            *((unsigned PY_LONG_LONG*)cppOut) = (unsigned PY_LONG_LONG) PyLong_AsUnsignedLongLong(pyIn);
        } else {
            PyErr_SetString(PyExc_TypeError, "Invalid type for unsigned long long conversion");
        }
    }
    static PythonToCppFunc isConvertible(PyObject* pyIn)
    {
        if (SbkNumber_Check(pyIn))
            return toCpp;
        return 0;
    }
};

// Floating point --------------------------------------------------------------------------

template <typename FLOAT>
struct FloatPrimitive : TwoPrimitive<FLOAT>
{
    static PyObject* toPython(const void* cppIn)
    {
        return PyFloat_FromDouble((double)*((FLOAT*)cppIn));
    }
    static void toCpp(PyObject* pyIn, void* cppOut)
    {
        *((FLOAT*)cppOut) = (FLOAT) PyLong_AsLong(pyIn);
    }
    static PythonToCppFunc isConvertible(PyObject* pyIn)
    {
        if (PyInt_Check(pyIn) || PyLong_Check(pyIn))
            return toCpp;
        return 0;
    }
    static void otherToCpp(PyObject* pyIn, void* cppOut)
    {
        *((FLOAT*)cppOut) = (FLOAT) PyFloat_AsDouble(pyIn);
    }
    static PythonToCppFunc isOtherConvertible(PyObject* pyIn)
    {
        if (SbkNumber_Check(pyIn))
            return otherToCpp;
        return 0;
    }
};
template <> struct Primitive<float> : FloatPrimitive<float> {};
template <> struct Primitive<double> : FloatPrimitive<double> {};

// Boolean ---------------------------------------------------------------------------------

template <>
struct Primitive<bool> : OnePrimitive<bool>
{
    static PyObject* toPython(const void* cppIn)
    {
        return PyBool_FromLong(*((bool*)cppIn));
    }
    static PythonToCppFunc isConvertible(PyObject* pyIn)
    {
        if (SbkNumber_Check(pyIn))
            return toCpp;
        return 0;
    }
    static void toCpp(PyObject* pyIn, void* cppOut)
    {
        *((bool*)cppOut) = (bool) PyInt_AS_LONG(pyIn);
    }
};

// Characters ------------------------------------------------------------------------------

template <typename CHAR>
struct CharPrimitive : IntPrimitive<CHAR>
{
    static void toCpp(PyObject* pyIn, void* cppOut)
    {

        *((CHAR*)cppOut) = (CHAR) Shiboken::String::toCString(pyIn)[0];
    }
    static PythonToCppFunc isConvertible(PyObject* pyIn)
    {
        if (Shiboken::String::checkChar(pyIn))
            return toCpp;
        return 0;
    }
    static void otherToCpp(PyObject* pyIn, void* cppOut)
    {
        PY_LONG_LONG result = PyLong_AsLongLong(pyIn);
        if (OverFlowChecker<CHAR>::check(result))
            PyErr_SetObject(PyExc_OverflowError, 0);
        *((CHAR*)cppOut) = (CHAR) result;
    }
    static PythonToCppFunc isOtherConvertible(PyObject* pyIn)
    {
        if (SbkNumber_Check(pyIn))
            return otherToCpp;
        return 0;
    }
    static SbkConverter* createConverter()
    {
        SbkConverter* converter = IntPrimitive<CHAR>::createConverter();
        Shiboken::Conversions::addPythonToCppValueConversion(converter, CharPrimitive<CHAR>::otherToCpp, CharPrimitive<CHAR>::isOtherConvertible);
        return converter;
    }

};
template <> struct Primitive<signed char> : CharPrimitive<signed char> {};
template <> struct Primitive<unsigned char> : CharPrimitive<unsigned char> {};
template <> struct Primitive<char> : CharPrimitive<char> {
    using CharPrimitive<char>::toPython;
    static PyObject* toPython(const void* cppIn) {
        return Shiboken::String::fromCString((const char*)cppIn, 1);
    }
};



// Strings ---------------------------------------------------------------------------------

template <>
struct Primitive<const char*> : TwoPrimitive<const char*>
{
    static PyObject* toPython(const void* cppIn)
    {
        if (!cppIn)
            Py_RETURN_NONE;
        return Shiboken::String::fromCString((const char*)cppIn);
    }
    static void toCpp(PyObject* pyIn, void* cppOut)
    {
        *((const char**)cppOut) = 0;
    }
    static PythonToCppFunc isConvertible(PyObject* pyIn)
    {
        if (pyIn == Py_None)
            return toCpp;
        return 0;
    }
    static void otherToCpp(PyObject* pyIn, void* cppOut)
    {
        *((const char**)cppOut) = (const char*) Shiboken::String::toCString(pyIn);
    }
    static PythonToCppFunc isOtherConvertible(PyObject* pyIn)
    {
        if (Shiboken::String::check(pyIn))
            return otherToCpp;
        return 0;
    }
};

template <>
struct Primitive<std::string> : TwoPrimitive<std::string>
{
    static PyObject* toPython(const void* cppIn)
    {
        return Shiboken::String::fromCString(((std::string*)cppIn)->c_str());
    }
    static void toCpp(PyObject* pyIn, void* cppOut)
    {
        *((std::string*)cppOut) = std::string();
    }
    static PythonToCppFunc isConvertible(PyObject* pyIn)
    {
        if (pyIn == Py_None)
            return toCpp;
        return 0;
    }
    static void otherToCpp(PyObject* pyIn, void* cppOut)
    {
        *((std::string*)cppOut) = Shiboken::String::toCString(pyIn);
    }
    static PythonToCppFunc isOtherConvertible(PyObject* pyIn)
    {
        if (Shiboken::String::check(pyIn))
            return otherToCpp;
        return 0;
    }
};

// Void pointer ----------------------------------------------------------------------------

template <>
struct Primitive<void*> : OnePrimitive<void*>
{
    static PyObject* toPython(const void* cppIn)
    {
        SbkDbg() << cppIn;
        if (!cppIn)
            Py_RETURN_NONE;
        PyObject* result = (PyObject*) cppIn;
        Py_INCREF(result);
        return result;
    }
    static void toCpp(PyObject* pyIn, void* cppOut)
    {
        SbkDbg() << pyIn;
        *((void**)cppOut) = pyIn;
    }
    static PythonToCppFunc isConvertible(PyObject* pyIn)
    {
        return toCpp;
    }
};

#endif // SBK_CONVERTER_P_H
