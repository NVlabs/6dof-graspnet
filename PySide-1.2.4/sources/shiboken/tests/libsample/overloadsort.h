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

#ifndef OVERLOADSORT_H
#define OVERLOADSORT_H

#include "libsamplemacros.h"

#include <list>

class ImplicitTarget
{
public:
    ImplicitTarget(){}
};

class ImplicitBase
{
public:
    ImplicitBase(){}
    ImplicitBase(const ImplicitTarget &b){}
};

class SortedOverload
{
public:

    inline const char *overload(int x) {
        return "int";
    }

    inline const char *overload(double x) {
        return "double";
    }

    inline const char *overload(ImplicitBase x) {
        return "ImplicitBase";
    }

    inline const char *overload(ImplicitTarget x) {
        return "ImplicitTarget";
    }

    inline const char *overload(const std::list<ImplicitBase> &x) {
        return "list(ImplicitBase)";
    }

    inline int implicit_overload(const ImplicitBase &x) {
        return 1;
    }

    inline const char *overloadDeep(int x, ImplicitBase &y) {
        return "ImplicitBase";
    }


    inline const char* pyObjOverload(int, int) { return "int,int"; }
    inline const char* pyObjOverload(unsigned char*, int) { return "PyObject,int"; }

};

#endif // OVERLOADSORT_H

