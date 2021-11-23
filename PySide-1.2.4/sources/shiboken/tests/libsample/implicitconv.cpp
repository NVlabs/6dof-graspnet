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

#include "implicitconv.h"

ImplicitConv
ImplicitConv::implicitConvCommon(ImplicitConv implicit)
{
    return implicit;
}

ImplicitConv
ImplicitConv::implicitConvDefault(ImplicitConv implicit)
{
    return implicit;
}

ImplicitConv::ICOverloadedFuncEnum
ImplicitConv::implicitConvOverloading(ImplicitConv implicit, int dummyArg)
{
    return ImplicitConv::OverFunc_Ii;
}

ImplicitConv::ICOverloadedFuncEnum
ImplicitConv::implicitConvOverloading(ImplicitConv implicit, bool dummyArg)
{
    return ImplicitConv::OverFunc_Ib;
}

ImplicitConv::ICOverloadedFuncEnum
ImplicitConv::implicitConvOverloading(int dummyArg)
{
    return ImplicitConv::OverFunc_i;
}

ImplicitConv::ICOverloadedFuncEnum
ImplicitConv::implicitConvOverloading(CtorEnum dummyArg)
{
    return ImplicitConv::OverFunc_C;
}

