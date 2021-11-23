/*
 * Copyright 2013 Kitware, Inc.
 *
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

#include "transform.h"

#ifdef _WIN32
#include <math.h>
#include <float.h>
static inline bool isfinite(double a) { return _finite(a); }
#else
#include <cmath>
#endif

using namespace std;

Point applyHomogeneousTransform(
    const Point& in,
    double m11, double m12, double m13,
    double m21, double m22, double m23,
    double m31, double m32, double m33,
    bool* okay)
{
    double x = m11 * in.x() + m12 * in.y() + m13;
    double y = m21 * in.x() + m22 * in.y() + m23;
    double w = m31 * in.x() + m32 * in.y() + m33;

    if (isfinite(w) && fabs(w) > 1e-10)
    {
        if (okay)
            *okay = true;
        return Point(x / w, y / w);
    }
    else
    {
        if (okay)
            *okay = false;
        return Point();
    }
}
