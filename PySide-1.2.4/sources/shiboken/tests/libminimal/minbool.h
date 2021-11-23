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

#ifndef MINBOOL_H
#define MINBOOL_H

#include "libminimalmacros.h"

class LIBMINIMAL_API MinBool
{
public:
    inline explicit MinBool(bool b) : m_value(b) {}
    bool value() const { return m_value; }
    inline MinBool operator!() const { return MinBool(!m_value); }
    inline MinBool& operator|=(const MinBool& other) {
        m_value = m_value | other.m_value;
        return *this;
    }
private:
    bool m_value;
};

inline bool operator==(MinBool b1, bool b2) { return !b1 == !b2; }
inline bool operator==(bool b1, MinBool b2) { return !b1 == !b2; }
inline bool operator==(MinBool b1, MinBool b2) { return !b1 == !b2; }
inline bool operator!=(MinBool b1, bool b2) { return !b1 != !b2; }
inline bool operator!=(bool b1, MinBool b2) { return !b1 != !b2; }
inline bool operator!=(MinBool b1, MinBool b2) { return !b1 != !b2; }

class LIBMINIMAL_API MinBoolUser
{
public:
    MinBoolUser() : m_minbool(MinBool(false)) {}
    virtual ~MinBoolUser() {}
    inline MinBool minBool() { return m_minbool; }
    inline void setMinBool(MinBool minBool) { m_minbool = minBool; }
    virtual MinBool invertedMinBool() { return !m_minbool; }
    inline MinBool callInvertedMinBool() { return invertedMinBool(); }
private:
    MinBool m_minbool;
};

#endif
