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

#ifndef POINTF_H
#define POINTF_H

#include "point.h"
#include <utility>

#include "libsamplemacros.h"

class LIBSAMPLE_API PointF
{
public:
    PointF(const Point& point);
    PointF(double x = 0.0, double y = 0.0);
    ~PointF() {}

    inline double x() const { return m_x; }
    inline double y() const { return m_y; }

    inline void setX(double x) { m_x = x; }
    inline void setY(double y) { m_y = y; }

    // This method could simply return the midpoint,
    // but the interesting part of the test is to set the
    // result in the pointer argument.
    void midpoint(const PointF& other, PointF* midpoint) const;

    // The != operator is not implemented for the purpose of testing
    // for the absence of the __ne__ method in the Python binding.
    bool operator==(const PointF& other);

    PointF operator+(const PointF& other);
    PointF operator-(const PointF& other);

    friend LIBSAMPLE_API PointF operator*(const PointF& pt, double mult);
    friend LIBSAMPLE_API PointF operator*(const PointF& pt, int mult);
    friend LIBSAMPLE_API PointF operator*(double mult, const PointF& pt);
    friend LIBSAMPLE_API PointF operator*(int mult, const PointF& pt);
    friend LIBSAMPLE_API PointF operator-(const PointF& pt);
    friend LIBSAMPLE_API bool operator!(const PointF& pt);

    PointF& operator+=(PointF &other);
    PointF& operator-=(PointF &other);

    void show();

private:
    double m_x;
    double m_y;
};

LIBSAMPLE_API PointF operator*(const PointF& pt, double mult);
LIBSAMPLE_API PointF operator*(const PointF& pt, int mult);
LIBSAMPLE_API PointF operator*(double mult, const PointF& pt);
LIBSAMPLE_API PointF operator*(int mult, const PointF& pt);
LIBSAMPLE_API PointF operator-(const PointF& pt);
LIBSAMPLE_API bool operator!(const PointF& pt);

LIBSAMPLE_API PointF operator*(const PointF& pt, double multiplier);

#endif // POINTF_H
