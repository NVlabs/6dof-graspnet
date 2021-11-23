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

#ifndef VAL_H
#define VAL_H

#include "libminimalmacros.h"

class LIBMINIMAL_API Val
{
public:
    explicit Val(int valId) : m_valId(valId) {}
    virtual ~Val() {}

    int valId() { return m_valId; }
    void setValId(int valId) { m_valId = valId; }

    virtual Val passValueType(Val val) { return val; }
    Val callPassValueType(Val val) { return passValueType(val); }

    virtual Val* passValueTypePointer(Val* val) { return val; }
    Val* callPassValueTypePointer(Val* val) { return passValueTypePointer(val); }

    virtual Val* passValueTypeReference(Val& val) { return &val; }
    Val* callPassValueTypeReference(Val& val) { return passValueTypeReference(val); }

    enum ValEnum { One, Other };
    ValEnum oneOrTheOtherEnumValue(ValEnum enumValue) { return enumValue == One ? Other : One; }
private:
    int m_valId;
};

#endif // VAL_H

