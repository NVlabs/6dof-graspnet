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

#include "blackbox.h"

using namespace std;

BlackBox::~BlackBox()
{
    // Free all maps.
    while (!m_objects.empty()) {
        delete (*m_objects.begin()).second;
        m_objects.erase(m_objects.begin());
    }
    while (!m_points.empty()) {
        delete (*m_points.begin()).second;
        m_points.erase(m_points.begin());
    }
}

int
BlackBox::keepObjectType(ObjectType* object)
{
    m_ticket++;
    std::pair<int, ObjectType*> item(m_ticket, object);
    m_objects.insert(item);
    object->setParent(0);

    return m_ticket;
}

ObjectType*
BlackBox::retrieveObjectType(int ticket)
{
    map<int, ObjectType*>::iterator it = m_objects.find(ticket);
    if (it != m_objects.end()) {
        ObjectType* second = it->second;
        m_objects.erase(it);
        return second;
    }
    return 0;
}

void
BlackBox::disposeObjectType(int ticket)
{
    ObjectType* object = retrieveObjectType(ticket);
    if (object)
        delete object;
}

int
BlackBox::keepPoint(Point* point)
{
    m_ticket++;
    std::pair<int, Point*> item(m_ticket, point);
    m_points.insert(item);

    return m_ticket;
}

Point*
BlackBox::retrievePoint(int ticket)
{
    map<int, Point*>::iterator it = m_points.find(ticket);
    if (it != m_points.end()) {
        Point* second = it->second;
        m_points.erase(it);
        return second;
    }
    return 0;
}

void
BlackBox::disposePoint(int ticket)
{
    Point* point = retrievePoint(ticket);
    if (point)
        delete point;
}


std::list<ObjectType*>
BlackBox::objects()
{
    std::list<ObjectType*> l;
    map<int, ObjectType*>::iterator it;

    for ( it = m_objects.begin() ; it != m_objects.end(); it++ )
        l.push_back((*it).second);

    return l;
}

std::list<Point*>
BlackBox::points()
{
    std::list<Point*> l;
    map<int, Point*>::iterator it;

    for ( it = m_points.begin() ; it != m_points.end(); it++ )
        l.push_back((*it).second);

    return l;
}

