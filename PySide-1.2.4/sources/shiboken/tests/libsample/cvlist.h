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

#ifndef CONSTVALUELIST_H
#define CONSTVALUELIST_H

#include <list>
#include "libsamplemacros.h"

class CVValueType
{
    CVValueType();
};

typedef std::list<const CVValueType*> const_ptr_value_list;

// This tests binding generation for a container of a const value type. The
// class doesn't need to do anything; this is just to verify that the generated
// binding code (the container conversion in particular) is const-valid.

class CVListUser
{
public:
    static const_ptr_value_list produce() { return const_ptr_value_list(); }
    static void consume(const const_ptr_value_list& l) { (void)l; }
};

#endif // LIST_H
