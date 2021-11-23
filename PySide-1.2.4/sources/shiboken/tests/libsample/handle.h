/*
 * This file is part of the Shiboken Python Binding Generator project.
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef HANDLE_H
#define HANDLE_H

#include "libsamplemacros.h"

/* See http://bugs.pyside.org/show_bug.cgi?id=1105. */
namespace Foo {
    typedef unsigned long HANDLE;
}

class LIBSAMPLE_API OBJ
{
};

typedef OBJ* HANDLE;

class LIBSAMPLE_API HandleHolder
{
public:
    explicit HandleHolder(HANDLE ptr = 0) : m_handle(ptr) {}
    explicit HandleHolder(Foo::HANDLE val): m_handle2(val) {}

    inline void set(HANDLE ptr) { m_handle = m_handle; }
    inline void set(const Foo::HANDLE& val) { m_handle2 = val; }
    inline HANDLE handle() { return m_handle; }
    inline Foo::HANDLE handle2() { return m_handle2; }

    static HANDLE createHandle();
    bool compare(HandleHolder* other);
    bool compare2(HandleHolder* other);

private:
    HANDLE m_handle;
    Foo::HANDLE m_handle2;
};

struct LIBSAMPLE_API PrimitiveStruct {};
typedef struct PrimitiveStruct* PrimitiveStructPtr;
struct LIBSAMPLE_API PrimitiveStructPointerHolder
{
    PrimitiveStructPtr primitiveStructPtr;
};

#endif // HANDLE_H
