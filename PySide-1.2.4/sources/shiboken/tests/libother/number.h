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

#ifndef NUMBER_H
#define NUMBER_H

#include "libothermacros.h"
#include "str.h"
#include "point.h"
#include "complex.h"

class LIBOTHER_API Number
{
public:
    explicit Number(int value) : m_value(value) {};
    inline int value() const { return m_value; }

    Str toStr() const;
    inline operator Str() const { return toStr(); }

    friend LIBOTHER_API Point operator*(const Point&, const Number&);

    Complex toComplex() const;
    static Number fromComplex(Complex cpx);

private:
    int m_value;
};

LIBOTHER_API Point operator*(const Point&, const Number&);

#endif // NUMBER_H
