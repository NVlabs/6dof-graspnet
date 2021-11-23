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

#ifndef PP_MACRO_H
#define PP_MACRO_H

#include <vector>
#include "pp-fwd.h"

namespace rpp
{

struct pp_macro {
#if defined (PP_WITH_MACRO_POSITION)
    pp_fast_string const *file;
#endif
    pp_fast_string const *name;
    pp_fast_string const *definition;
    std::vector<pp_fast_string const *> formals;

    union {
        int unsigned state;

        struct {
            int unsigned hidden: 1;
            int unsigned function_like: 1;
            int unsigned variadics: 1;
        };
    };

    int lines;
    pp_macro *next;
    std::size_t hash_code;

    inline pp_macro():
#if defined (PP_WITH_MACRO_POSITION)
            file(0),
#endif
            name(0),
            definition(0),
            state(0),
            lines(0),
            next(0),
            hash_code(0) {}
};

} // namespace rpp

#endif // PP_MACRO_H

// kate: space-indent on; indent-width 2; replace-tabs on;
