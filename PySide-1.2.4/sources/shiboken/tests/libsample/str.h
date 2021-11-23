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

#ifndef STR_H
#define STR_H
#include <string>

#include "libsamplemacros.h"

class LIBSAMPLE_API Str
{
public:
    Str(const Str& s);
    Str(char c);
    Str(const char* cstr = "");
    ~Str();

    Str arg(const Str& s) const;

    Str& append(const Str& s);
    Str& prepend(const Str& s);

    const char* cstring() const;
    char get_char(int pos) const;
    bool set_char(int pos, char ch);

    int toInt(bool* ok = 0, int base = 10) const;

    void show() const;

    inline int size() const { return m_str.size(); }

    // nonsense operator just to test reverse operators
    Str operator+(int number) const;
    bool operator==(const Str& other) const;
    bool operator<(const Str& other) const;

private:
    void init(const char* cstr);
    std::string m_str;

    friend LIBSAMPLE_API Str operator+(int number, const Str& str);
    friend LIBSAMPLE_API unsigned int strHash(const Str& str);
};

LIBSAMPLE_API Str operator+(int number, const Str& str);
LIBSAMPLE_API unsigned int strHash(const Str& str);

typedef Str PStr;
LIBSAMPLE_API void changePStr(PStr* pstr, const char* suffix);
LIBSAMPLE_API void duplicatePStr(PStr* pstr = 0);

#endif // STR_H
