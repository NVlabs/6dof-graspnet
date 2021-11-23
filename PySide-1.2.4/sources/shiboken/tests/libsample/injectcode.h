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

#ifndef INJECTCODE_H
#define INJECTCODE_H

#include "libsamplemacros.h"
#include <utility>
#include <string>

class LIBSAMPLE_API InjectCode
{
public:
    InjectCode();
    virtual ~InjectCode();

    const char* simpleMethod1(int arg0, int arg1);
    const char* simpleMethod2();
    const char* simpleMethod3(int argc, char** argv);

    const char* overloadedMethod(int argc, char** argv);
    const char* overloadedMethod(int arg0, double arg1);
    const char* overloadedMethod(int arg0, bool arg1);

    virtual int arrayMethod(int count, int* values) const;
    inline int callArrayMethod(int count, int *values) const { return arrayMethod(count, values); }
    virtual const char* virtualMethod(int arg);
    int sumArrayAndLength(int* values) const;
private:
    // This attr is just to retain the memory pointed by all return values,
    // So, the memory returned by all methods will be valid until someone call
    // another method of this class.
    std::string m_valueHolder;

    template<typename T>
    const char* toStr(const T& value);
};

#endif // INJECTCODE_H

