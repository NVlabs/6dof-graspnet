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

#include "multiple_derived.h"

MDerived1::MDerived1() : m_value(100)
{
}

MDerived2::MDerived2() : m_value(200)
{
}

MDerived3::MDerived3() : m_value(3000)
{
}

MDerived4::MDerived4()
{
}

MDerived5::MDerived5()
{
}

MDerived1*
MDerived1::transformFromBase1(Base1* self)
{
    MDerived1* ptr = dynamic_cast<MDerived1*>(self);
    return ptr;
}

MDerived1*
MDerived1::transformFromBase2(Base2* self)
{
    MDerived1* ptr = dynamic_cast<MDerived1*>(self);
    return ptr;
}

