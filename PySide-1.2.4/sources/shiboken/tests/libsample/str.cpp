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

#include "str.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sstream>

using namespace std;

Str::Str(const Str& s)
{
    init(s.cstring());
}

Str::Str(char c)
{
    char str[2] = { c, 0 };
    init(str);
}

Str::Str(const char* cstr)
{
    init(cstr);
}

void
Str::init(const char* cstr)
{
    if (cstr)
        m_str = cstr;
}

Str::~Str()
{
}

Str
Str::arg(const Str& s) const
{
    size_t idx = m_str.find_first_of("%VAR");
    if (idx == std::string::npos) {
        return *this;
    } else {
        std::string result = m_str;
        result.replace(idx, 4, s.m_str);
        return result.c_str();
    }
}

Str&
Str::append(const Str& s)
{
    m_str += s.m_str;
    return *this;
}

Str&
Str::prepend(const Str& s)
{
    m_str = s.m_str + m_str;
    return *this;
}

const char*
Str::cstring() const
{
    return m_str.c_str();
}

int
Str::toInt(bool* ok, int base) const
{
    bool my_ok;
    int result = 0;
    istringstream conv(m_str);
    switch (base) {
        case 8:
            conv >> std::oct >> result;
            break;
        case 10:
            conv >> std::dec >> result;
            break;
        case 16:
            conv >> std::hex >> result;
            break;
    }
    my_ok = istringstream::eofbit & conv.rdstate();
    if (!my_ok)
        result = 0;
    if (ok)
        *ok = my_ok;
    return result;
}

void
Str::show() const
{
    printf("%s", cstring());
}

char
Str::get_char(int pos) const
{
    return m_str[pos];
}

bool
Str::set_char(int pos, char ch)
{
    m_str[pos] = ch;
    return true;
}

Str Str::operator+(int number) const
{
    ostringstream in;
    in << m_str << number;
    return in.str().c_str();
}

bool Str::operator==(const Str& other) const
{
    return m_str == other.m_str;
}

Str operator+(int number, const Str& str)
{
    ostringstream in;
    in << number << str.m_str;
    return in.str().c_str();
}

bool Str::operator<(const Str& other) const
{
    return m_str < other.m_str;
}

unsigned int strHash(const Str& str)
{
    unsigned int result = 0;
    const std::string& cppStr = str.m_str;
    std::string::const_iterator it = cppStr.begin();
    for (; it != cppStr.end(); ++it)
        result = 5 * result + *it;
    return result;
}

void changePStr(PStr* pstr, const char* suffix)
{
    pstr->append(suffix);
}

void duplicatePStr(PStr* pstr)
{
    if (!pstr)
        return;
    pstr->append(*pstr);
}
