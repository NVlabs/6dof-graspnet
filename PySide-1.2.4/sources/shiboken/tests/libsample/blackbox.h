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

#ifndef BLACKBOX_H
#define BLACKBOX_H

#include "libsamplemacros.h"
#include <map>
#include "objecttype.h"
#include "point.h"

class LIBSAMPLE_API BlackBox
{
public:
    typedef std::map<int, ObjectType*> ObjectTypeMap;
    typedef std::map<int, Point*> PointMap;

    BlackBox() { m_ticket = -1;}
    ~BlackBox();

    int keepObjectType(ObjectType* object);
    ObjectType* retrieveObjectType(int ticket);
    void disposeObjectType(int ticket);

    int keepPoint(Point* point);
    Point* retrievePoint(int ticket);
    void disposePoint(int ticket);

    std::list<ObjectType*> objects();
    std::list<Point*> points();

    inline void referenceToValuePointer(Point*&) {}
    inline void referenceToObjectPointer(ObjectType*&) {}

private:
    ObjectTypeMap m_objects;
    PointMap m_points;
    int m_ticket;
};

#endif // BLACKBOX_H

