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

#include "othermultiplederived.h"

VirtualMethods OtherMultipleDerived::returnUselessClass()
{
    return VirtualMethods();
}

Base1* OtherMultipleDerived::createObject(const std::string& objName)
{
    if (objName == "Base1")
        return new Base1;
    else if (objName == "MDerived1")
        return new MDerived1;
    else if (objName == "SonOfMDerived1")
        return new SonOfMDerived1;
    else if (objName == "MDerived3")
        return new MDerived3;
    else if (objName == "OtherMultipleDerived")
        return new OtherMultipleDerived;
    return 0;
}

