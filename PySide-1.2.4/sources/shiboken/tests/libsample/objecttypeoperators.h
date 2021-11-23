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

#ifndef OBJECTTYPEOPERATORS_H
#define OBJECTTYPEOPERATORS_H

#include "libsamplemacros.h"
#include <string>

class LIBSAMPLE_API ObjectTypeOperators
{
public:
    explicit ObjectTypeOperators(const std::string key);
    virtual ~ObjectTypeOperators() {}

    bool operator==(const ObjectTypeOperators& other) const;
    const ObjectTypeOperators& operator<(const ObjectTypeOperators& other) const;

    // chaos!
    virtual void operator>(const ObjectTypeOperators&) { m_key.append("operator>"); }

    std::string key() const { return m_key; }

private:
    std::string m_key;

    ObjectTypeOperators(ObjectTypeOperators&);
    ObjectTypeOperators& operator=(ObjectTypeOperators&);
};

LIBSAMPLE_API bool operator==(const ObjectTypeOperators* obj, const std::string& str);
LIBSAMPLE_API bool operator==(const std::string& str, const ObjectTypeOperators* obj);
LIBSAMPLE_API std::string operator+(const ObjectTypeOperators* obj, const std::string& str);
LIBSAMPLE_API std::string operator+(const std::string& str, const ObjectTypeOperators* obj);

#endif // OBJECTTYPEOPERATORS_H
