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

#ifndef BYTEARRAY_H
#define BYTEARRAY_H

#include "str.h"
#include "libsamplemacros.h"
#include <vector>

class LIBSAMPLE_API ByteArray
{
public:
    ByteArray();
    ByteArray(char data);
    ByteArray(const char* data);
    ByteArray(const char* data, int len);
    ByteArray(const ByteArray& other);

    int size() const;
    char at(int i) const;
    char operator[](int i) const;

    const char* data() const;

    ByteArray& append(char c);
    ByteArray& append(const char* data);
    ByteArray& append(const char* data, int len);
    ByteArray& append(const ByteArray& other);

    bool operator==(const ByteArray& other) const;
    bool operator!=(const ByteArray& other) const;

    ByteArray& operator+=(char c);
    ByteArray& operator+=(const char* data);
    ByteArray& operator+=(const ByteArray& other);

    static unsigned int hash(const ByteArray& byteArray);
private:
    std::vector<char> m_data;
    friend LIBSAMPLE_API bool operator==(const ByteArray& ba1, const char* ba2);
    friend LIBSAMPLE_API bool operator==(const char* ba1, const ByteArray& ba2);
    friend LIBSAMPLE_API bool operator!=(const ByteArray& ba1, const char* ba2);
    friend LIBSAMPLE_API bool operator!=(const char* ba1, const ByteArray& ba2);

    friend LIBSAMPLE_API ByteArray operator+(const ByteArray& ba1, const ByteArray& ba2);
    friend LIBSAMPLE_API ByteArray operator+(const ByteArray& ba1, const char* ba2);
    friend LIBSAMPLE_API ByteArray operator+(const char* ba1, const ByteArray& ba2);
    friend LIBSAMPLE_API ByteArray operator+(const ByteArray& ba1, char ba2);
    friend LIBSAMPLE_API ByteArray operator+(char ba1, const ByteArray& ba2);
};

LIBSAMPLE_API bool operator==(const ByteArray& ba1, const char* ba2);
LIBSAMPLE_API bool operator==(const char* ba1, const ByteArray& ba2);
LIBSAMPLE_API bool operator!=(const ByteArray& ba1, const char* ba2);
LIBSAMPLE_API bool operator!=(const char* ba1, const ByteArray& ba2);

LIBSAMPLE_API ByteArray operator+(const ByteArray& ba1, const ByteArray& ba2);
LIBSAMPLE_API ByteArray operator+(const ByteArray& ba1, const char* ba2);
LIBSAMPLE_API ByteArray operator+(const char* ba1, const ByteArray& ba2);
LIBSAMPLE_API ByteArray operator+(const ByteArray& ba1, char ba2);
LIBSAMPLE_API ByteArray operator+(char ba1, const ByteArray& ba2);

#endif // BYTEARRAY_H
