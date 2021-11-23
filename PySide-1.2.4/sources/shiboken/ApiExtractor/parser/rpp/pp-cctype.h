/*
 * This file is part of the API Extractor project.
 *
 * Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
 * Copyright 2005 Roberto Raggi <roberto@kdevelop.org>
 *
 * Contact: PySide team <contact@pyside.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#ifndef PP_CCTYPE_H
#define PP_CCTYPE_H

#include <cctype>

namespace rpp
{

inline bool pp_isalpha(int __ch)
{
    return std::isalpha((unsigned char) __ch) != 0;
}

inline bool pp_isalnum(int __ch)
{
    return std::isalnum((unsigned char) __ch) != 0;
}

inline bool pp_isdigit(int __ch)
{
    return std::isdigit((unsigned char) __ch) != 0;
}

inline bool pp_isspace(int __ch)
{
    return std::isspace((unsigned char) __ch) != 0;
}

} // namespace rpp

#endif // PP_CCTYPE_H

// kate: space-indent on; indent-width 2; replace-tabs on;
