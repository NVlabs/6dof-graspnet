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

#ifndef MODIFICATIONS_H
#define MODIFICATIONS_H

#include "libsamplemacros.h"
#include <utility>
#include "point.h"
#include "oddbool.h"

class ObjectType;

class LIBSAMPLE_API Modifications
{
public:
    Modifications();
    virtual ~Modifications();

    enum OverloadedModFunc {
        OverloadedNone,
        Overloaded_ibid,
        Overloaded_ibib,
        Overloaded_ibiP,
        Overloaded_ibii,
        Overloaded_ibPP
    };

    // those overloaded methods should be heavily modified
    // to push the overload decisor to its limits
    inline OverloadedModFunc overloaded(int a0, bool b0, int c0, double d0) { return Overloaded_ibid; }
    inline OverloadedModFunc overloaded(int a1, bool b1, int c1, bool d1) { return Overloaded_ibib; }
    inline OverloadedModFunc overloaded(int a2, bool b2, int c2, Point d2) { return Overloaded_ibiP; }
    inline OverloadedModFunc overloaded(int a3, bool b3, int c3 = 123, int d3 = 456) { return Overloaded_ibii; }
    inline OverloadedModFunc overloaded(int a4, bool b4, Point c4, Point d4) { return Overloaded_ibPP; }

    inline void argRemoval0(int a0, bool a1, int a2 = 123, int a3 = 456) {}
    inline void argRemoval0(int a0, bool a1, int a2, bool a3) {}

    inline void argRemoval1(int a0, bool a1, Point a2 = Point(1, 2), Point a3 = Point(3, 4), int a4 = 333) {}
    inline void argRemoval1(int a0, bool a1, int a2, bool a3) {}

    inline void argRemoval2(int a0, bool a1, Point a2 = Point(1, 2), Point a3 = Point(3, 4), int a4 = 333) {}

    inline void argRemoval3(int a0, Point a1 = Point(1, 2), bool a2 = true, Point a3 = Point(3, 4), int a4 = 333) {}

    inline void argRemoval4(int a0, Point a1, bool a2, Point a3 = Point(3, 4), int a4 = 333) {}

    inline void argRemoval5(int a0, bool a1, Point a2 = Point(1, 2), Point a3 = Point(3, 4), int a4 = 333) {}
    inline void argRemoval5(int a0, bool a1, int a2, bool a3) {}

    // 'ok' must be removed and the return value will be changed
    // to a tuple (PyObject*) containing the expected result plus
    // the 'ok' value as a Python boolean
    std::pair<double, double> pointToPair(Point pt, bool* ok);

    // same as 'pointToPair' except that this time 'ok' is the first argument
    double multiplyPointCoordsPlusValue(bool* ok, Point pt, double value);

    // completely remove 'plus' from the Python side
    int doublePlus(int value, int plus = 0);

    // the default value for both arguments must be changed in Python
    int power(int base = 1, int exponent = 0);

    // in Python set argument default value to 10
    int timesTen(int number);

    // in Python remove the argument default value
    int increment(int number = 0);

    // don't export this method to Python
    void exclusiveCppStuff();

    // change the name of this regular method
    int cppMultiply(int a, int b);

    // change the name of this virtual method
    virtual const char* className();

    Point sumPointArray(int arraySize, const Point pointArray[]);

    // Replace 'const void*' by 'ByteArray&'.
    int getSize(const void* data, int size);

    // Mark the argument with a <no-null-pointer/> tag;
    // the test implementation must expect point never to be null.
    int sumPointCoordinates(const Point* point);

    // Modify the return value of a virtual method.
    virtual double differenceOfPointCoordinates(const Point* pt, bool* ok);
    double callDifferenceOfPointCoordinates(const Point* pt, bool* ok) { return differenceOfPointCoordinates(pt, ok); }

    // Sets an ObjectType in the argument and returns true.
    bool nonConversionRuleForArgumentWithDefaultValue(ObjectType** object = 0);
    ObjectType* getObject() const { return m_object; }

    // Inject code with a %CONVERTTOPYTHON that receives an user's primitive type.
    static inline OddBool passOddBool(OddBool ob) { return ob; }

private:
    ObjectType* m_object;
};

class LIBSAMPLE_API AbstractModifications : public Modifications
{
public:
    AbstractModifications() {}
    virtual ~AbstractModifications() {}

    inline bool invert(bool value) { return !value; }

    // completely remove this method in Python
    virtual void pointlessPureVirtualMethod() = 0;
};

#endif // MODIFICATIONS_H
