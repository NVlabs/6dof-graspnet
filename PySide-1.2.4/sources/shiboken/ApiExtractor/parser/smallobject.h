/*
 * This file is part of the API Extractor project.
 *
 * Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Copyright (C) 2002-2005 Roberto Raggi <roberto@kdevelop.org>
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


#ifndef SMALLOBJECT_H
#define SMALLOBJECT_H

#include "rxx_allocator.h"
#include <cstring>

class pool
{
    rxx_allocator<char> __alloc;

public:
    inline void *allocate(std::size_t __size);
    inline void *allocate(std::size_t __size, std::size_t __stride);
};

inline void *pool::allocate(std::size_t __size)
{
    return __alloc.allocate(__size);
}

inline void *pool::allocate(std::size_t __size, std::size_t __stride)
{
  return __alloc.allocate(__size, __stride);
}

#endif

// kate: space-indent on; indent-width 2; replace-tabs on;
