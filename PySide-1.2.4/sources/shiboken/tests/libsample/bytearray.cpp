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

#include <cstring>
#include "bytearray.h"

ByteArray::ByteArray()
{
    m_data = std::vector<char>(1);
    m_data[0] = '\0';
}

ByteArray::ByteArray(char c)
{
    m_data = std::vector<char>(2);
    m_data[0] = c;
    m_data[1] = '\0';
}

ByteArray::ByteArray(const char* data)
{
    size_t len = strlen(data);
    m_data = std::vector<char>(len + 1);
    memcpy(&m_data[0], data, len);
    m_data[len] = '\0';
}

ByteArray::ByteArray(const char* data, int len)
{
    m_data = std::vector<char>(len + 1);
    memcpy(&m_data[0], data, len);
    m_data[len] = '\0';
}

ByteArray::ByteArray(const ByteArray& other)
{
    m_data = std::vector<char>(other.size() + 1);
    memcpy(&m_data[0], &other.m_data[0], other.size());
    m_data[other.size()] = '\0';
}

int
ByteArray::size() const
{
    return m_data.size() - 1;
}

char
ByteArray::at(int pos) const
{
    return m_data[pos];
}

const char*
ByteArray::data() const
{
    return &(m_data[0]);
}

ByteArray&
ByteArray::append(char c)
{
    m_data.pop_back();
    m_data.push_back(c);
    m_data.push_back('\0');
    return *this;
}

ByteArray&
ByteArray::append(const char* data)
{
    m_data.pop_back();
    for (int i = 0; i < (int)strlen(data); ++i)
        m_data.push_back(data[i]);
    m_data.push_back('\0');
    return *this;
}

ByteArray&
ByteArray::append(const char* data, int len)
{
    m_data.pop_back();
    for (int i = 0; i < len; ++i)
        m_data.push_back(data[i]);
    m_data.push_back('\0');
    return *this;
}

ByteArray&
ByteArray::append(const ByteArray& other)
{
    m_data.pop_back();
    for (int i = 0; i < (int)other.m_data.size(); ++i)
        m_data.push_back(other.m_data[i]);
    m_data.push_back('\0');
    return *this;
}

static bool compare(const std::vector<char>& mine, const char* other)
{
    for (int i = 0; i < (int)mine.size() - 1; ++i) {
        if (mine[i] != other[i])
            return false;
    }
    return true;
}

bool
ByteArray::operator==(const ByteArray& other) const
{
    return compare(m_data, &other.m_data[0]);
}
bool
operator==(const ByteArray& ba1, const char* ba2)
{
    return compare(ba1.m_data, ba2);
}
bool
operator==(const char* ba1, const ByteArray& ba2)
{
    return compare(ba2.m_data, ba1);
}

bool
ByteArray::operator!=(const ByteArray& other) const
{
    return !(m_data == other.m_data);
}
bool
operator!=(const ByteArray& ba1, const char* ba2)
{
    return !(ba1 == ba2);
}
bool
operator!=(const char* ba1, const ByteArray& ba2)
{
    return !(ba1 == ba2);
}

ByteArray&
ByteArray::operator+=(char c)
{
    return append(c);
}
ByteArray&
ByteArray::operator+=(const char* data)
{
    return append(data);
}
ByteArray&
ByteArray::operator+=(const ByteArray& other)
{
    return append(other);
}

ByteArray
operator+(const ByteArray& ba1, const ByteArray& ba2)
{
    return ByteArray(ba1) += ba2;
}
ByteArray
operator+(const ByteArray& ba1, const char* ba2)
{
    return ByteArray(ba1) += ByteArray(ba2);
}
ByteArray
operator+(const char* ba1, const ByteArray& ba2)
{
    return ByteArray(ba1) += ba2;
}
ByteArray
operator+(const ByteArray& ba1, char ba2)
{
    return ByteArray(ba1) += ByteArray(ba2);
}
ByteArray
operator+(char ba1, const ByteArray& ba2)
{
    return ByteArray(ba1) += ba2;
}

unsigned int
ByteArray::hash(const ByteArray& byteArray)
{
    unsigned int result = 0;
    for (int i = 0; i < (int)byteArray.m_data.size(); ++i)
        result = 5 * result + byteArray.m_data[i];
    return result;
}
