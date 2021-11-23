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

#ifndef POLYGON_H
#define POLYGON_H

#include <list>
#include "point.h"

#include "libsamplemacros.h"

class LIBSAMPLE_API Polygon
{
public:
    typedef std::list<Point> PointList;

    Polygon() {}
    Polygon(double x, double y);
    Polygon(Point point);
    Polygon(PointList points);
    ~Polygon() {}

    void addPoint(Point point);

    inline const PointList& points() const { return m_points; }

    // This method intentionally receives and returns copies of a Polygon object.
    static Polygon doublePolygonScale(Polygon polygon);

    // This method invalidates the argument to be used for Polygon(Point) implicit conversion.
    static void stealOwnershipFromPython(Point* point);

    // This method invalidates the argument to be used in a call to doublePolygonScale(Polygon).
    static void stealOwnershipFromPython(Polygon* polygon);

private:
    PointList m_points;
};

#endif // POLYGON_H

