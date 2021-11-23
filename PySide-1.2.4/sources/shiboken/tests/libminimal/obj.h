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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef OBJ_H
#define OBJ_H

#include "libminimalmacros.h"

class LIBMINIMAL_API Obj
{
public:
    explicit Obj(int objId);
    virtual ~Obj();

    int objId() { return m_objId; }
    void setObjId(int objId) { m_objId = objId; }

    virtual bool virtualMethod(int val);
    bool callVirtualMethod(int val) { return virtualMethod(val); }

    virtual Obj* passObjectType(Obj* obj) { return obj; }
    Obj* callPassObjectType(Obj* obj) { return passObjectType(obj); }

    virtual Obj* passObjectTypeReference(Obj& obj) { return &obj; }
    Obj* callPassObjectTypeReference(Obj& obj) { return passObjectTypeReference(obj); }

private:
    Obj(const Obj&);
    Obj& operator=(const Obj&);
    int m_objId;
};

#endif // OBJ_H

