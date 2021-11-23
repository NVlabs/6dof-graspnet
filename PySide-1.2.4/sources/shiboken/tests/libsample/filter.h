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

#ifndef FILTER_H
#define FILTER_H

#include <string>
#include <list>

#include "libsamplemacros.h"

class Intersection;

class LIBSAMPLE_API Filter
{
};

class LIBSAMPLE_API Data : public Filter
{

public:
    enum Field {
        Name,
        Album,
        Year
    };

    Data(Field field, std::string value);

    Field field() const { return m_field; }
    std::string value() const { return m_value; }

private:
    Field m_field;
    std::string m_value;
};

class LIBSAMPLE_API Union : public Filter
{
public:

    Union(const Data&);
    Union(const Intersection&);
    Union() {};
    Union(const Union&);

    std::list<Filter> filters() const { return m_filters; }
    void addFilter(const Filter& data) { m_filters.push_back(data); }

private:
    std::list<Filter> m_filters;
};

class LIBSAMPLE_API Intersection : public Filter
{
public:

    Intersection(const Data&);
    Intersection(const Union&);
    Intersection() {};
    Intersection(const Intersection&);

    std::list<Filter> filters() const { return m_filters; }
    void addFilter(const Filter& data) { m_filters.push_back(data); }

private:
    std::list<Filter> m_filters;
};

LIBSAMPLE_API Intersection operator&(const Intersection& a, const Intersection& b);

#endif // FILTER_H


