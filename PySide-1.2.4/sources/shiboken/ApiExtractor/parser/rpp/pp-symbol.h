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

#ifndef PP_SYMBOL_H
#define PP_SYMBOL_H

#include <cassert>
#include <iterator>
#include "pp-fwd.h"
#include "parser/rxx_allocator.h"

namespace rpp
{

class pp_symbol
{
    static rxx_allocator<char> &allocator_instance() {
        static rxx_allocator<char>__allocator;
        return __allocator;
    }
    static rxx_allocator<pp_fast_string> &ppfs_allocator_instance ()
    {
        static rxx_allocator<pp_fast_string>__ppfs_allocator;
        return __ppfs_allocator;
    }

public:
    static int &N() {
        static int __N;
        return __N;
    }

    static pp_fast_string const *get(char const *__data, std::size_t __size) {
        ++N();
        char *data = allocator_instance().allocate(__size + 1);
        memcpy(data, __data, __size);
        data[__size] = '\0';

        pp_fast_string *where = ppfs_allocator_instance ().allocate (sizeof (pp_fast_string));
        return new(where) pp_fast_string(data, __size);
    }

    template <typename _InputIterator>
    static pp_fast_string const *get(_InputIterator __first, _InputIterator __last) {
        ++N();
        std::ptrdiff_t __size;
#if defined(__SUNPRO_CC)
        std::distance(__first, __last, __size);
#else
        __size = std::distance(__first, __last);
#endif
        assert(__size >= 0 && __size < 512);

        char *data = allocator_instance().allocate(__size + 1);
        std::copy(__first, __last, data);
        data[__size] = '\0';

        pp_fast_string *where = ppfs_allocator_instance ().allocate (sizeof (pp_fast_string));
        return new(where) pp_fast_string(data, __size);
    }

    static pp_fast_string const *get(std::string const &__s) {
        return get(__s.c_str(), __s.size());
    }
};

} // namespace rpp

#endif // PP_SYMBOL_H

// kate: space-indent on; indent-width 2; replace-tabs on;
