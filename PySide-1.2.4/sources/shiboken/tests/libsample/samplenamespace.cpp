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

#include <iostream>
#include <cstdlib>
#include <time.h>
#include "samplenamespace.h"

using namespace std;

namespace SampleNamespace
{

OutValue
enumInEnumOut(InValue in)
{
    OutValue retval;
    switch(in) {
        case ZeroIn:
            retval = ZeroOut;
            break;
        case OneIn:
            retval = OneOut;
            break;
        case TwoIn:
            retval = TwoOut;
            break;
        default:
            retval = (OutValue) -1;
    }
    return retval;
}

Option
enumArgumentWithDefaultValue(Option opt)
{
    return opt;
}

int
getNumber(Option opt)
{
    int retval;
    switch(opt) {
        case RandomNumber:
            retval = rand() % 100;
            break;
        case UnixTime:
            retval = (int) time(0);
            break;
        default:
            retval = 0;
    }
    return retval;
}

void
doSomethingWithArray(const unsigned char* data, unsigned int size, const char* format)
{
    // This function does nothing in fact.
    // It is here as a dummy copy of QPixmap.loadFromData method
    // to check compilation issues, i.e. if it compiles, it's ok.
}

int
enumItemAsDefaultValueToIntArgument(int value)
{
    return value;
}

void
forceDecisorSideA(ObjectType* object)
{
}

void
forceDecisorSideA(const Point& pt, const Str& text, ObjectType* object)
{
}

void
forceDecisorSideB(int a, ObjectType* object)
{
}

void
forceDecisorSideB(int a, const Point& pt, const Str& text, ObjectType* object)
{
}

double
passReferenceToValueType(const Point& point, double multiplier)
{
    return (point.x() + point.y()) * multiplier;
}

int
passReferenceToObjectType(const ObjectType& obj, int multiplier)
{
    return obj.objectName().size() * multiplier;
}

} // namespace SampleNamespace
