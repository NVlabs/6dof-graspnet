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

#include "collector.h"

void Collector::clear()
{
    m_items.clear();
}

Collector& Collector::operator<<(unsigned long item)
{
    m_items.push_back(item);
    return *this;
}

Collector& Collector::operator<<(const ObjectType *obj)
{
    m_items.push_back(obj->identifier());
    return *this;
}

std::list<unsigned long> Collector::items()
{
    return m_items;
}

int Collector::size()
{
    return (int) m_items.size();
}

Collector &operator<<(Collector &s, const IntWrapper &w)
{
    s << w.value;
    return s;
}
