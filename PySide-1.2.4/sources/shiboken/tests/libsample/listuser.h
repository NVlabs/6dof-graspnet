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

#ifndef LISTUSER_H
#define LISTUSER_H

#include <list>
#include "complex.h"
#include "point.h"
#include "pointf.h"

#include "libsamplemacros.h"

class LIBSAMPLE_API ListUser
{
public:
    typedef std::list<Point*> PointList;

    enum ListOfSomething {
        ListOfPoint,
        ListOfPointF
    };

    ListUser() {}
    ListUser(const ListUser& other) : m_lst(other.m_lst) {}
    virtual ~ListUser() {}

    virtual std::list<int> createList();
    std::list<int> callCreateList();

    static std::list<Complex> createComplexList(Complex cpx0, Complex cpx1);

    double sumList(std::list<int> vallist);
    double sumList(std::list<double> vallist);

    static ListOfSomething listOfPoints(const std::list<Point>& pointlist);
    static ListOfSomething listOfPoints(const std::list<PointF>& pointlist);

    static void multiplyPointList(PointList& points, double multiplier);

    inline void setList(std::list<int> lst) { m_lst = lst; }
    inline std::list<int> getList() { return m_lst; }

private:
    std::list<int> m_lst;
};

#endif // LISTUSER_H

