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

#ifndef RECT_H
#define RECT_H

#include "libsamplemacros.h"

class LIBSAMPLE_API Rect
{
public:
    Rect()
    {
        m_left = m_top = 0;
        m_right = m_bottom = -1;
    }
    Rect(int left, int top, int right, int bottom)
        : m_left(left), m_top(top), m_right(right), m_bottom(bottom) { }
    ~Rect() {}
    inline int left() const { return m_left; }
    inline int top() const { return m_top; }
    inline int right() const { return m_right; }
    inline int bottom() const { return m_bottom; }
private:
    int m_left;
    int m_top;
    int m_right;
    int m_bottom;
};

class LIBSAMPLE_API RectF
{
public:
    RectF()
    {
        m_left = m_top = 0;
        m_right = m_bottom = -1;
    }
    RectF(int left, int top, int right, int bottom)
        : m_left(left), m_top(top), m_right(right), m_bottom(bottom) { }
    RectF(const Rect& other)
    {
        m_left = other.left();
        m_top = other.top();
        m_right = other.right();
        m_bottom = other.bottom();
    }
    ~RectF() {}
    inline double left() const { return m_left; }
    inline double top() const { return m_top; }
    inline double right() const { return m_right; }
    inline double bottom() const { return m_bottom; }
private:
    double m_left;
    double m_top;
    double m_right;
    double m_bottom;
};

#endif // RECT_H

