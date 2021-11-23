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

#ifndef ODDBOOL_H
#define ODDBOOL_H

#include "libsamplemacros.h"

class OddBool
{

public:
    inline explicit OddBool(bool b) : m_value(b) {}
    bool value() const { return m_value; }

    inline OddBool operator!() const { return OddBool(!m_value); }

private:
    bool m_value;
};

inline bool operator==(OddBool b1, bool b2) { return !b1 == !b2; }
inline bool operator==(bool b1, OddBool b2) { return !b1 == !b2; }
inline bool operator==(OddBool b1, OddBool b2) { return !b1 == !b2; }
inline bool operator!=(OddBool b1, bool b2) { return !b1 != !b2; }
inline bool operator!=(bool b1, OddBool b2) { return !b1 != !b2; }
inline bool operator!=(OddBool b1, OddBool b2) { return !b1 != !b2; }

class OddBoolUser
{
public:
    OddBoolUser() : m_oddbool(OddBool(false)) {}
    OddBoolUser(const OddBool& oddBool) : m_oddbool(oddBool) {}
    virtual ~OddBoolUser() {}

    inline OddBool oddBool() { return m_oddbool; }
    inline void setOddBool(OddBool oddBool) { m_oddbool = oddBool; }

    virtual OddBool invertedOddBool()
    {
        return !m_oddbool;
    }

    inline OddBool callInvertedOddBool()
    {
        return invertedOddBool();
    }

    static inline OddBool getOddBool(const OddBoolUser& oddBoolUser)
    {
        return oddBoolUser.m_oddbool;
    }

private:
    OddBool m_oddbool;
};

#endif
