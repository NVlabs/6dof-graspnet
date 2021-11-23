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
#include "polygon.h"

using namespace std;

Polygon::Polygon(double x, double y)
{
    m_points.push_back(Point(x, y));
}

Polygon::Polygon(Point point)
{
    m_points.push_back(point);
}

Polygon::Polygon(PointList points)
{
    m_points = points;
}

void
Polygon::addPoint(Point point)
{
    m_points.push_back(point);
}

Polygon
Polygon::doublePolygonScale(Polygon polygon)
{
    Polygon result;
    for(PointList::const_iterator piter = result.points().begin(); piter != result.points().end(); piter++)
        result.addPoint((*piter) * 2.0);
    return result;
}

void
Polygon::stealOwnershipFromPython(Point* point)
{
    delete point;
}

void
Polygon::stealOwnershipFromPython(Polygon* polygon)
{
    delete polygon;
}

