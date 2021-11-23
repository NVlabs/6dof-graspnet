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

#include <iostream>
#include "reference.h"

using namespace std;

void
Reference::show() const
{
    cout << "Reference.objId: " << m_objId << ", address: " << this;
}

int
Reference::usesReferenceVirtual(Reference& r, int inc)
{
    return r.m_objId + inc;
}

int
Reference::usesConstReferenceVirtual(const Reference& r, int inc)
{
    return r.m_objId + inc;
}

int
Reference::callUsesReferenceVirtual(Reference& r, int inc)
{
    return usesReferenceVirtual(r, inc);
}

int
Reference::callUsesConstReferenceVirtual(const Reference& r, int inc)
{
    return usesConstReferenceVirtual(r, inc);
}

void
Reference::alterReferenceIdVirtual(Reference& r)
{
    r.setObjId(r.objId() * Reference::multiplier());
}

void
Reference::callAlterReferenceIdVirtual(Reference& r)
{
    alterReferenceIdVirtual(r);
}

ObjTypeReference::~ObjTypeReference()
{
}
