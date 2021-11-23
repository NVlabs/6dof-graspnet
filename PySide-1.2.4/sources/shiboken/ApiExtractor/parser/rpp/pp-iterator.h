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

#ifndef PP_ITERATOR_H
#define PP_ITERATOR_H

#include <iterator>

namespace rpp
{

class pp_null_output_iterator
        : public std::iterator<std::output_iterator_tag, void, void, void, void>
{
public:
    pp_null_output_iterator() {}

    template <typename _Tp>
    pp_null_output_iterator &operator=(_Tp const &) {
        return *this;
    }

    inline pp_null_output_iterator &operator *() {
        return *this;
    }
    inline pp_null_output_iterator &operator ++ () {
        return *this;
    }
    inline pp_null_output_iterator operator ++ (int) {
        return *this;
    }
};

template <typename _Container>
class pp_output_iterator
        : public std::iterator<std::output_iterator_tag, void, void, void, void>
{
    std::string &_M_result;

public:
    explicit pp_output_iterator(std::string &__result):
            _M_result(__result) {}

    inline pp_output_iterator<_Container>& operator=(const pp_output_iterator<_Container>& other)
    {
        _M_result = other._M_result;
        return *this;
    }

    inline pp_output_iterator &operator=(typename _Container::const_reference __v) {
        if (_M_result.capacity() == _M_result.size())
            _M_result.reserve(_M_result.capacity() << 2);

        _M_result.push_back(__v);
        return *this;
    }

    inline pp_output_iterator &operator *() {
        return *this;
    }
    inline pp_output_iterator &operator ++ () {
        return *this;
    }
    inline pp_output_iterator operator ++ (int) {
        return *this;
    }
};

} // namespace rpp

#endif // PP_ITERATOR_H

// kate: space-indent on; indent-width 2; replace-tabs on;
