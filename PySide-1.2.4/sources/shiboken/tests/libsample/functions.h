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

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "libsamplemacros.h"
#include <list>
#include <utility>
#include "oddbool.h"
#include "complex.h"
#include "objecttype.h"

enum GlobalEnum {
    NoThing,
    FirstThing,
    SecondThing,
    ThirdThing
};

enum GlobalOverloadFuncEnum {
    GlobalOverloadFunc_i,
    GlobalOverloadFunc_d
};

LIBSAMPLE_API void printSomething();
LIBSAMPLE_API int gimmeInt();
LIBSAMPLE_API double gimmeDouble();
LIBSAMPLE_API double multiplyPair(std::pair<double, double> pair);
LIBSAMPLE_API std::list<Complex> gimmeComplexList();
LIBSAMPLE_API Complex sumComplexPair(std::pair<Complex, Complex> cpx_pair);

LIBSAMPLE_API int countCharacters(const char* text);
LIBSAMPLE_API char* makeCString();
LIBSAMPLE_API const char* returnCString();

LIBSAMPLE_API char* returnNullPrimitivePointer();
LIBSAMPLE_API ObjectType* returnNullObjectTypePointer();
LIBSAMPLE_API Event* returnNullValueTypePointer();

// Tests overloading on functions (!methods)
LIBSAMPLE_API GlobalOverloadFuncEnum overloadedFunc(int val);
LIBSAMPLE_API GlobalOverloadFuncEnum overloadedFunc(double val);

LIBSAMPLE_API unsigned int doubleUnsignedInt(unsigned int value);
LIBSAMPLE_API long long doubleLongLong(long long value);
LIBSAMPLE_API unsigned long long doubleUnsignedLongLong(unsigned long long value);
LIBSAMPLE_API short doubleShort(short value);

LIBSAMPLE_API int acceptInt(int x);
LIBSAMPLE_API unsigned int acceptUInt(unsigned int x);
LIBSAMPLE_API long acceptLong(long x);
LIBSAMPLE_API unsigned long acceptULong(unsigned long x);
LIBSAMPLE_API double acceptDouble(double x);

LIBSAMPLE_API int acceptIntReference(int& x);
LIBSAMPLE_API OddBool acceptOddBoolReference(OddBool& x);

#endif // FUNCTIONS_H
