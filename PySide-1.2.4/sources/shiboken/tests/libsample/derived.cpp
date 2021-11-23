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
#include "derived.h"

using namespace std;

Derived::Derived(int id) : Abstract(id)
{
}

Derived::~Derived()
{
}

Abstract*
Derived::createObject()
{
    static int id = 100;
    return new Derived(id++);
}

void
Derived::pureVirtual()
{
}

void*
Derived::pureVirtualReturningVoidPtr()
{
    return 0;
}

void
Derived::unpureVirtual()
{
}

bool
Derived::singleArgument(bool b)
{
    return !b;
}

double
Derived::defaultValue(int n)
{
    return ((double) n) + 0.1;
}

OverloadedFuncEnum
Derived::overloaded(int i, int d)
{
    return OverloadedFunc_ii;
}

OverloadedFuncEnum
Derived::overloaded(double n)
{
    return OverloadedFunc_d;
}

Derived::OtherOverloadedFuncEnum
Derived::otherOverloaded(int a, int b, bool c, double d)
{
    return OtherOverloadedFunc_iibd;
}

Derived::OtherOverloadedFuncEnum
Derived::otherOverloaded(int a, double b)
{
    return OtherOverloadedFunc_id;
}

struct SecretClass : public Abstract {
    virtual void pureVirtual() {}
    virtual void* pureVirtualReturningVoidPtr() { return 0; }
    virtual PrintFormat returnAnEnum() { return Short; }
    void hideFunction(HideType*){};
};

Abstract* Derived::triggerImpossibleTypeDiscovery()
{
    return new SecretClass;
}

struct AnotherSecretClass : public Derived {
};

Abstract* Derived::triggerAnotherImpossibleTypeDiscovery()
{
    return new AnotherSecretClass;
}
