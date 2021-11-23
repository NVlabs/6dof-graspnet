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

#include "sbkstring.h"
#include "autodecref.h"

namespace Shiboken
{

namespace String
{

bool checkType(PyTypeObject* type)
{
    return type == &PyUnicode_Type
#if PY_MAJOR_VERSION < 3
            || type == &PyString_Type
#endif
    ;
}

bool check(PyObject* obj)
{
    return obj == Py_None ||
#if PY_MAJOR_VERSION < 3
        PyString_Check(obj) ||
#endif
        PyUnicode_Check(obj);
}

bool checkChar(PyObject* pyobj)
{
    if (check(pyobj) && (len(pyobj) == 1))
        return true;

    return false;
}

bool isConvertible(PyObject* obj)
{
    return check(obj);
}

PyObject* fromCString(const char* value)
{
#ifdef IS_PY3K
    return PyUnicode_FromString(value);
#else
    return PyBytes_FromString(value);
#endif
}

PyObject* fromCString(const char* value, int len)
{
#ifdef IS_PY3K
    return PyUnicode_FromStringAndSize(value, len);
#else
    return PyBytes_FromStringAndSize(value, len);
#endif
}

const char* toCString(PyObject* str, Py_ssize_t* len)
{
    if (str == Py_None)
        return NULL;
#ifdef IS_PY3K
    if (PyUnicode_Check(str)) {
        if (len) {
            // We need to encode the unicode string into utf8 to know the size of returned char*.
            Shiboken::AutoDecRef uniStr(PyUnicode_AsUTF8String(str));
            *len = PyBytes_GET_SIZE(uniStr.object());
        }
        // Return unicode from str instead of uniStr, because the lifetime of the returned pointer
        // depends on the lifetime of str.
        return _PyUnicode_AsString(str);
    }
#endif
    if (PyBytes_Check(str)) {
        if (len)
            *len = PyBytes_GET_SIZE(str);
        return PyBytes_AS_STRING(str);
    }
    return 0;
}

bool concat(PyObject** val1, PyObject* val2)
{
    if (PyUnicode_Check(*val1) && PyUnicode_Check(val2)) {
        PyObject* result = PyUnicode_Concat(*val1, val2);
        Py_DECREF(*val1);
        *val1 = result;
        return true;
    }

    if (PyBytes_Check(*val1) && PyBytes_Check(val2)) {
        PyBytes_Concat(val1, val2);
        return true;
    }

#if PY_MAJOR_VERSION < 3
    if (PyString_Check(*val1) && PyString_Check(val2)) {
        PyString_Concat(val1, val2);
        return true;
    }
#endif
    return false;
}

PyObject* fromFormat(const char* format, ...)
{
    va_list argp;
    va_start(argp, format);
    PyObject* result = 0;
#ifdef IS_PY3K
    result = PyUnicode_FromFormatV(format, argp);
#else
    result = PyString_FromFormatV(format, argp);
#endif
    va_end(argp);
    return result;
}

PyObject* fromStringAndSize(const char* str, Py_ssize_t size)
{
#ifdef IS_PY3K
    return PyUnicode_FromStringAndSize(str, size);
#else
    return PyString_FromStringAndSize(str, size);
#endif
}

int compare(PyObject* val1, const char* val2)
{
    if (PyUnicode_Check(val1))
#ifdef IS_PY3K
       return PyUnicode_CompareWithASCIIString(val1, val2);
#else
    {
        PyObject* uVal2 = PyUnicode_FromString(val2);
        bool result =  PyUnicode_Compare(val1, uVal2);
        Py_XDECREF(uVal2);
        return result;
    }
    if (PyString_Check(val1))
        return strcmp(PyString_AS_STRING(val1), val2);
#endif
    return 0;

}

Py_ssize_t len(PyObject* str)
{
    if (str == Py_None)
        return 0;

    if (PyUnicode_Check(str))
        return PyUnicode_GET_SIZE(str);

    if (PyBytes_Check(str))
        return PyBytes_GET_SIZE(str);
    return 0;
}

} // namespace String

} // namespace Shiboken
