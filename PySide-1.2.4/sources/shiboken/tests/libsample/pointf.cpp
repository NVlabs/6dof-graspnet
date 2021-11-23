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
#include "pointf.h"

using namespace std;

PointF::PointF(const Point& point) : m_x(point.x()), m_y(point.y())
{
}

PointF::PointF(double x, double y) : m_x(x), m_y(y)
{
}

void
PointF::midpoint(const PointF& other, PointF* midpoint) const
{
    if (!midpoint)
        return;
    midpoint->setX((m_x + other.m_x) / 2.0);
    midpoint->setY((m_y + other.m_y) / 2.0);
}

void
PointF::show()
{
    cout << "(x: " << m_x << ", y: " << m_y << ")";
}

bool
PointF::operator==(const PointF& other)
{
    return m_x == other.m_x && m_y == other.m_y;
}

PointF
PointF::operator+(const PointF& other)
{
    return PointF(m_x + other.m_x, m_y + other.m_y);
}

PointF
PointF::operator-(const PointF& other)
{
    return PointF(m_x - other.m_x, m_y - other.m_y);
}

PointF&
PointF::operator+=(PointF &other)
{
    m_x += other.m_x;
    m_y += other.m_y;
    return *this;
}

PointF&
PointF::operator-=(PointF &other)
{
    m_x -= other.m_x;
    m_y -= other.m_y;
    return *this;
}

PointF
operator*(const PointF& pt, double mult)
{
    return PointF(pt.m_x * mult, pt.m_y * mult);
}

PointF
operator*(const PointF& pt, int mult)
{
    return PointF(((int) pt.m_x) * mult, ((int) pt.m_y) * mult);
}

PointF
operator*(double mult, const PointF& pt)
{
    return PointF(pt.m_x * mult, pt.m_y * mult);
}

PointF
operator*(int mult, const PointF& pt)
{
    return PointF(((int) pt.m_x) * mult, ((int) pt.m_y) * mult);
}

PointF
operator-(const PointF& pt)
{
    return PointF(-pt.m_x, -pt.m_y);
}

bool
operator!(const PointF& pt)
{
    return (pt.m_x == 0.0 && pt.m_y == 0.0);
}

