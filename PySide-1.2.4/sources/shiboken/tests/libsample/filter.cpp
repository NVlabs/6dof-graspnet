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

#include <string>
#include "filter.h"
Data::Data(Field field, std::string value)
    : m_field(field), m_value(value)
{
}

Union::Union(const Data& filter)
{
    m_filters.push_back(filter);
}

Union::Union(const Intersection& filter)
{
    m_filters.push_back(filter);
}

Union::Union(const Union& filter)
{
    m_filters = filter.filters();
}

Intersection::Intersection(const Data& filter)
{
    m_filters.push_back(filter);
}

Intersection::Intersection(const Union& filter)
{
    m_filters.push_back(filter);
}

Intersection::Intersection(const Intersection& filter)
{
    m_filters = filter.filters();
}

Intersection operator&(const Intersection& a, const Intersection& b)
{
    Intersection filter;
    filter.addFilter(a);
    filter.addFilter(b);

    return filter;
}
