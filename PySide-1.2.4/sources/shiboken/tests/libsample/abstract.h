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

#ifndef ABSTRACT_H
#define ABSTRACT_H

#include "libsamplemacros.h"
#include "point.h"
#include "complex.h"

class ObjectType;

// this class is not exported to python
class HideType
{
};

class LIBSAMPLE_API Abstract
{
private:
    enum PrivateEnum {
        PrivValue0,
        PrivValue1,
        PrivValue2 = PrivValue1 + 2
    };
public:
    enum PrintFormat {
        Short,
        Verbose,
        OnlyId,
        ClassNameAndId,
        DummyItemToTestPrivateEnum1 = Abstract::PrivValue1,
        DummyItemToTestPrivateEnum2 = PrivValue2,
    };

    enum Type {
        TpAbstract, TpDerived
    };

    static const int staticPrimitiveField;
    int primitiveField;
    Complex userPrimitiveField;
    Point valueTypeField;
    ObjectType* objectTypeField;

    Abstract(int id = -1);
    virtual ~Abstract();

    inline int id() { return m_id; }

    // factory method
    inline static Abstract* createObject() { return 0; }

    // method that receives an Object Type
    inline static int getObjectId(Abstract* obj) { return obj->id(); }

    virtual void pureVirtual() = 0;
    virtual void* pureVirtualReturningVoidPtr() = 0;
    virtual void unpureVirtual();

    virtual PrintFormat returnAnEnum() = 0;
    void callVirtualGettingEnum(PrintFormat p);
    virtual void virtualGettingAEnum(PrintFormat p);

    void callPureVirtual();
    void callUnpureVirtual();

    void show(PrintFormat format = Verbose);
    virtual Type type() const { return TpAbstract; }

    virtual void hideFunction(HideType* arg) = 0;

protected:
    virtual const char* className() { return "Abstract"; }

    // Protected bit-field structure member.
    unsigned int bitField: 1;

private:
    int m_id;
};
#endif // ABSTRACT_H
