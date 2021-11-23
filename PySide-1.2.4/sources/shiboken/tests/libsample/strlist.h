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

#ifndef STRLIST_H
#define STRLIST_H

#include <list>
#include "str.h"

#include "libsamplemacros.h"

class LIBSAMPLE_API StrList : public std::list<Str>
{
public:
    enum CtorEnum {
        NoParamsCtor,
        StrCtor,
        CopyCtor,
        ListOfStrCtor
    };

    inline StrList() : m_ctorUsed(NoParamsCtor) {}
    inline explicit StrList(const Str& str) : m_ctorUsed(StrCtor) { push_back(str); }
    inline StrList(const StrList& lst) : std::list<Str>(lst), m_ctorUsed(CopyCtor) {}
    inline StrList(const std::list<Str>& lst) : std::list<Str>(lst), m_ctorUsed(ListOfStrCtor) {}

    inline void append(Str str) { push_back(str); }
    Str join(const Str& sep) const;

    bool operator==(const std::list<Str>& other) const;
    inline bool operator!=(const std::list<Str>& other) const { return !(*this == other); }

    CtorEnum constructorUsed() { return m_ctorUsed; }
private:
    CtorEnum m_ctorUsed;
};

typedef StrList PStrList;

#endif // STRLIST_H
