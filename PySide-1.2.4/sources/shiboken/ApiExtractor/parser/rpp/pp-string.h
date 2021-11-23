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

#ifndef PP_STRING_H
#define PP_STRING_H

namespace rpp
{

template <typename _CharT>
class pp_string
{
    typedef std::char_traits<_CharT> traits_type;
    typedef std::size_t size_type;

    _CharT const *_M_begin;
    std::size_t _M_size;

public:
    inline pp_string():
            _M_begin(0), _M_size(0) {}

    explicit pp_string(std::string const &__s):
            _M_begin(__s.c_str()), _M_size(__s.size()) {}

    inline pp_string(_CharT const *__begin, std::size_t __size):
            _M_begin(__begin), _M_size(__size) {}

    inline _CharT const *begin() const {
        return _M_begin;
    }
    inline _CharT const *end() const {
        return _M_begin + _M_size;
    }

    inline _CharT at(std::size_t index) const {
        return _M_begin [index];
    }

    inline std::size_t size() const {
        return _M_size;
    }

    inline int compare(pp_string const &__other) const {
        size_type const __size = this->size();
        size_type const __osize = __other.size();
        size_type const __len = std::min(__size,  __osize);

        int __r = traits_type::compare(_M_begin, __other._M_begin, __len);
        if (!__r)
            __r = (int)(__size - __osize);

        return __r;
    }

    inline bool operator == (pp_string const &__other) const {
        return compare(__other) == 0;
    }

    inline bool operator != (pp_string const &__other) const {
        return compare(__other) != 0;
    }

    inline bool operator < (pp_string const &__other) const {
        return compare(__other) < 0;
    }

    inline bool operator == (char const *s) const {
        std::size_t n = strlen(s);

        if (n != _M_size)
            return false;

        return ! strncmp(_M_begin, s, n);
    }

    inline bool operator != (char const *s) const {
        return ! operator == (s);
    }
};

} // namespace rpp

#endif // PP_STRING_H

// kate: space-indent on; indent-width 2; replace-tabs on;
