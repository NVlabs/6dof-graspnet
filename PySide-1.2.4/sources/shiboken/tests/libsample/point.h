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

#ifndef POINT_H
#define POINT_H

#include "complex.h"
#include <utility>

#include "libsamplemacros.h"

class LIBSAMPLE_API Point
{
public:
    Point(int x = 0, int y = 0);
    Point(double x, double y);
    ~Point() {}

    inline double x() const { return m_x; }
    inline double y() const { return m_y; }

    inline void setX(double x) { m_x = x; }
    inline void setY(double y) { m_y = y; }
    inline void setXAsUint(unsigned int x) { m_x = x; }
    inline void setYAsUint(unsigned int y) { m_y = y; }

    // This method could simply return the midpoint,
    // but the interesting part of the test is to set the
    // result in the pointer argument.
    void midpoint(const Point& other, Point* midpoint) const;

    Point* copy() const;

    inline const Point& getConstReferenceToSelf() const { return *this; }
    inline const Point* getSelf() const { return this; }

    // The != operator is not implemented for the purpose of testing
    // for the absense of the __ne__ method in the Python binding.
    bool operator==(const Point& other);

    Point operator+(const Point& other);
    Point operator-(const Point& other);
    Point operator/(int operand);

    friend LIBSAMPLE_API Point operator*(const Point& pt, double mult);
    friend LIBSAMPLE_API Point operator*(const Point& pt, int mult);
    friend LIBSAMPLE_API Point operator*(double mult, const Point& pt);
    friend LIBSAMPLE_API Point operator*(int mult, const Point& pt);
    friend LIBSAMPLE_API Point operator-(const Point& pt);
    friend LIBSAMPLE_API bool operator!(const Point& pt);

    Point& operator+=(Point &other);
    Point& operator-=(Point &other);

    void show();

private:
    double m_x;
    double m_y;
};

LIBSAMPLE_API Point operator*(const Point& pt, double mult);
LIBSAMPLE_API Point operator*(const Point& pt, int mult);
LIBSAMPLE_API Point operator*(double mult, const Point& pt);
LIBSAMPLE_API Point operator*(int mult, const Point& pt);
LIBSAMPLE_API Point operator-(const Point& pt);
LIBSAMPLE_API bool operator!(const Point& pt);

LIBSAMPLE_API Complex transmutePointIntoComplex(const Point& point);
LIBSAMPLE_API Point transmuteComplexIntoPoint(const Complex& cpx);

LIBSAMPLE_API Point operator*(const Point& pt, double multiplier);

#endif // POINT_H
