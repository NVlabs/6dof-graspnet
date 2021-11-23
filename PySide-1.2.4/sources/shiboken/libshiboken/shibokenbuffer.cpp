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

#include "shibokenbuffer.h"
#include <cstdlib>
#include <cstring>

bool Shiboken::Buffer::checkType(PyObject* pyObj)
{
    return PyObject_CheckReadBuffer(pyObj);
}

void* Shiboken::Buffer::getPointer(PyObject* pyObj, Py_ssize_t* size)
{

#ifdef IS_PY3K
    Py_buffer view;
    if (PyObject_GetBuffer(pyObj, &view, PyBUF_ND) == 0) {
        if (size)
            *size = view.len;
        return view.buf;
    } else {
        return 0;
    }
#else
    const void* buffer = 0;
    Py_ssize_t bufferSize = 0;

    PyObject_AsReadBuffer(pyObj, &buffer, &bufferSize);

    if (size)
        *size = bufferSize;
    return const_cast<void*>(buffer);
#endif
}

PyObject* Shiboken::Buffer::newObject(void* memory, Py_ssize_t size, Type type)
{
    if (size == 0)
        Py_RETURN_NONE;
#ifdef IS_PY3K
    Py_buffer view;
    memset(&view, 0, sizeof(Py_buffer));
    view.buf = memory;
    view.len = size;
    view.readonly = type == Shiboken::Buffer::ReadOnly;
    view.ndim = 1;
    view.itemsize = sizeof(char);
    Py_ssize_t shape[] = { size };
    view.shape = shape;
    return PyMemoryView_FromBuffer(&view);
#else
    return type == ReadOnly ? PyBuffer_FromMemory(memory, size) : PyBuffer_FromReadWriteMemory(memory, size);
#endif
}

PyObject* Shiboken::Buffer::newObject(const void* memory, Py_ssize_t size)
{
    return newObject(const_cast<void*>(memory), size, ReadOnly);
}
