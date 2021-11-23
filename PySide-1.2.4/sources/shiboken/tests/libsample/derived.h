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

#ifndef DERIVED_H
#define DERIVED_H

#include "libsamplemacros.h"
#include "abstract.h"

enum OverloadedFuncEnum {
    OverloadedFunc_ii,
    OverloadedFunc_d
};

class LIBSAMPLE_API Derived : public Abstract
{
public:
    enum OtherOverloadedFuncEnum {
        OtherOverloadedFunc_iibd,
        OtherOverloadedFunc_id
    };

    class SomeInnerClass {
    public:
        void uselessMethod() {}
        SomeInnerClass operator+(const SomeInnerClass& other) { return other; }
        bool operator==(const SomeInnerClass& other) { return true; }
    };

    Derived(int id = -1);
    virtual ~Derived();
    virtual void pureVirtual();
    virtual void* pureVirtualReturningVoidPtr();
    virtual void unpureVirtual();

    virtual PrintFormat returnAnEnum() { return Short; }
    virtual Type type() const { return TpDerived; }

    // factory method
    static Abstract* createObject();

    // single argument
    bool singleArgument(bool b);

    // method with default value
    double defaultValue(int n = 0);

    // overloads
    OverloadedFuncEnum overloaded(int i = 0, int d = 0);
    OverloadedFuncEnum overloaded(double n);

    // more overloads
    OtherOverloadedFuncEnum otherOverloaded(int a, int b, bool c, double d);
    OtherOverloadedFuncEnum otherOverloaded(int a, double b);

    inline SomeInnerClass returnMyParameter(const SomeInnerClass& s) { return s; }

    static Abstract* triggerImpossibleTypeDiscovery();
    static Abstract* triggerAnotherImpossibleTypeDiscovery();

    void hideFunction(HideType*) {}
protected:
    const char* getClassName() { return className(); }
    virtual const char* className() { return "Derived"; }
};
#endif // DERIVED_H

