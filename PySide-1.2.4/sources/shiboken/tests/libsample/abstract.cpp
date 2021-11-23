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
#include "abstract.h"
#include "objecttype.h"

using namespace std;

const int Abstract::staticPrimitiveField = 0;

Abstract::Abstract(int id) : m_id(id)
{
    primitiveField = 123;
    valueTypeField = Point(12, 34);
    objectTypeField = 0;
    bitField = 0;
}

Abstract::~Abstract()
{
}

void
Abstract::unpureVirtual()
{
}

void
Abstract::callUnpureVirtual()
{
    this->unpureVirtual();
}


void
Abstract::callPureVirtual()
{
    this->pureVirtual();
}

void
Abstract::show(PrintFormat format)
{
    cout << '<';
    switch(format) {
        case Short:
            cout << this;
            break;
        case Verbose:
            cout << "class " << className() << " | cptr: " << this;
            cout << ", id: " << m_id;
            break;
        case OnlyId:
            cout << "id: " << m_id;
            break;
        case ClassNameAndId:
            cout << className() << " - id: " << m_id;
            break;
    }
    cout << '>';
}

void Abstract::callVirtualGettingEnum(PrintFormat p)
{
    virtualGettingAEnum(p);
}

void Abstract::virtualGettingAEnum(Abstract::PrintFormat p)
{
}

