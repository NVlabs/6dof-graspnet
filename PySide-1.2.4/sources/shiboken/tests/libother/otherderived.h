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

#ifndef OTHERDERIVED_H
#define OTHERDERIVED_H

#include "libothermacros.h"
#include "abstract.h"
#include "derived.h"
#include "objecttype.h"
#include "complex.h"

class ObjectType;

class LIBOTHER_API OtherDerived : public Abstract
{
public:
    OtherDerived(int id = -1);
    virtual ~OtherDerived();
    virtual void pureVirtual();
    virtual void* pureVirtualReturningVoidPtr();
    virtual void unpureVirtual();
    virtual PrintFormat returnAnEnum() { return Short; }

    inline void useObjectTypeFromOtherModule(ObjectType*) {}
    inline Event useValueTypeFromOtherModule(const Event& e) { return e; }
    inline Complex useValueTypeFromOtherModule(const Complex& c) { return c; }
    inline void useEnumTypeFromOtherModule(OverloadedFuncEnum) {}

    // factory method
    static Abstract* createObject();

    void hideFunction(HideType*) {}

protected:
    inline const char* getClassName() { return className(); }
    virtual const char* className() { return "OtherDerived"; }
};
#endif // OTHERDERIVED_H

