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

#include <numeric>
#include <cstdlib>
#include "listuser.h"

using namespace std;

std::list<int>
ListUser::callCreateList()
{
    return createList();
}

std::list<int>
ListUser::createList()
{
    std::list<int> retval;
    for (int i = 0; i < 4; i++)
        retval.push_front(rand());
    return retval;
}

std::list<Complex>
ListUser::createComplexList(Complex cpx0, Complex cpx1)
{
    std::list<Complex> retval;
    retval.push_back(cpx0);
    retval.push_back(cpx1);
    return retval;
}

double
ListUser::sumList(std::list<int> vallist)
{
    return std::accumulate(vallist.begin(), vallist.end(), 0.0);
}

double
ListUser::sumList(std::list<double> vallist)
{
    return std::accumulate(vallist.begin(), vallist.end(), 0.0);
}

ListUser::ListOfSomething
ListUser::listOfPoints(const std::list<Point>& pointlist)
{
    return ListOfPoint;
}

ListUser::ListOfSomething
ListUser::listOfPoints(const std::list<PointF>& pointlist)
{
    return ListOfPointF;
}

void
ListUser::multiplyPointList(PointList& points, double multiplier)
{
    for(PointList::iterator piter = points.begin(); piter != points.end(); piter++) {
        (*piter)->setX((*piter)->x() * multiplier);
        (*piter)->setY((*piter)->y() * multiplier);
    }
}

