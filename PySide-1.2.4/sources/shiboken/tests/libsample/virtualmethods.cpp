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

#include "virtualmethods.h"

int VirtualDtor::dtor_called = 0;

double
VirtualMethods::virtualMethod0(Point pt, int val, Complex cpx, bool b)
{
    return (pt.x() * pt.y() * val) + cpx.imag() + ((int) b);
}

bool
VirtualMethods::createStr(const char* text, Str*& ret)
{
    if (!text) {
        ret = 0;
        return false;
    }

    ret = new Str(text);
    return true;
}

void
VirtualMethods::getMargins(int* left, int* top, int* right, int* bottom) const
{
    *left = m_left;
    *top = m_top;
    *right = m_right;
    *bottom = m_bottom;
}

