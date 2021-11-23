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

#include "objecttypeoperators.h"

ObjectTypeOperators::ObjectTypeOperators(const std::string key) : m_key(key)
{
}

bool ObjectTypeOperators::operator==(const ObjectTypeOperators& other) const
{
    return m_key == other.m_key;
}

const ObjectTypeOperators& ObjectTypeOperators::operator<(const ObjectTypeOperators& other) const
{
    return m_key < other.m_key ? *this : other;
}

bool operator==(const ObjectTypeOperators* obj, const std::string& str)
{
    return obj->key() == str;
}

bool operator==(const std::string& str, const ObjectTypeOperators* obj)
{
    return str == obj->key();
}

std::string operator+(const ObjectTypeOperators* obj, const std::string& str)
{
    return obj->key() + str;
}

std::string operator+(const std::string& str, const ObjectTypeOperators* obj)
{
    return str + obj->key();
}
