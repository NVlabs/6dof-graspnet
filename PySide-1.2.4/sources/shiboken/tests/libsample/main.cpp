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
#include <list>
#include "abstract.h"
#include "derived.h"
#include "kindergarten.h"
#include "complex.h"
#include "point.h"
#include "size.h"
#include "listuser.h"
#include "samplenamespace.h"

using namespace std;

int
main(int argv, char **argc)
{
    cout << endl;

    Derived derived;

    cout << endl;

    derived.unpureVirtual();
    derived.pureVirtual();
    derived.callPureVirtual();

    cout << endl;
    Abstract* abs;
    abs = Abstract::createObject();
    cout << "Abstract::createObject(): " << abs << endl << endl;
    delete abs;

    abs = Derived::createObject();
    cout << "Derived::createObject() : ";
    abs->show();
    cout << endl;
    delete abs;
    cout << endl;

    abs = Derived::createObject();
    cout << "Derived::createObject() : ";
    abs->show();
    cout << endl;
    delete abs;
    cout << endl;

    cout << endl << "-----------------------------------------" << endl;

    KinderGarten kg;
    Derived* d[] = { 0, 0, 0 };

    for (int i = 0; i < 3; i++) {
        d[i] = new Derived(i);
        d[i]->show();
        cout << endl;
        kg.addChild(d[i]);
    }

    kg.show();
    cout << endl;

    cout << endl << "* kill child ";
    d[2]->show();
    cout << " ----------------" << endl;
    kg.killChild(d[2]);
    kg.show();
    cout << endl;

    cout << endl << "* release child ";
    d[1]->show();
    cout << " -------------" << endl;
    Abstract* released = kg.releaseChild(d[1]);
    cout << "released: ";
    released->show();
    cout << endl;
    kg.show();
    cout << endl;

    cout << endl << "* kill children ------------------------------------" << endl;
    kg.killChildren();
    kg.show();
    cout << endl << endl;

    cout << "-----------------------------------------" << endl;
    ListUser lu;
    cout << "ListUser::createList()" << endl;
    std::list<int> intlist = lu.createList();
    for (std::list<int>::iterator it = intlist.begin(); it != intlist.end(); it++) {
        cout << "* " << *it << endl;
    }

    cout << "ListUser::createComplexList" << endl;
    std::list<Complex> cpxlist = ListUser::createComplexList(Complex(1.1, 2.2), Complex(3.3, 4.4));
    for (std::list<Complex>::iterator it = cpxlist.begin(); it != cpxlist.end(); it++) {
        cout << "* ";
        (*it).show();
        cout << endl;
    }
    cout << endl;

    cout << "-----------------------------------------" << endl;
    cout << "SampleNamespace" << endl;

    cout << "SampleNamespace::RandomNumber: ";
    cout << SampleNamespace::getNumber(SampleNamespace::RandomNumber);
    cout << endl;
    cout << "SampleNamespace::UnixTime: ";
    cout << SampleNamespace::getNumber(SampleNamespace::UnixTime);
    cout << endl;
    double val_d = 1.3;
    cout << "SampleNamespace::powerOfTwo(" << val_d << "): ";
    cout << SampleNamespace::powerOfTwo(val_d) << endl;
    int val_i = 7;
    cout << "SampleNamespace::powerOfTwo(" << val_i << "): ";
    cout << SampleNamespace::powerOfTwo(val_i) << endl;
    cout << endl;

    cout << "-----------------------------------------" << endl;
    cout << "Point" << endl;

    Point p1(1.1, 2.2);
    cout << "p1: ";
    p1.show();
    cout << endl;

    Point p2(3.4, 5.6);
    cout << "p2: ";
    p2.show();
    cout << endl;

    cout << "p1 + p2 == ";
    (p1 + p2).show();
    cout << endl;

    cout << "p1 * 2.0 == ";
    (p1 * 2.0).show();
    cout << endl;

    cout << "1.5 * p2 == ";
    (1.5 * p2).show();
    cout << endl;

    cout << "p1: ";
    p1.show();
    cout << endl << "p2: ";
    p2.show();
    cout << endl << "p1 += p2" << endl;
    p1 += p2;
    cout << "p1: ";
    p1.show();
    cout << endl;

    cout << "p1 == p2 ? " << ((p1 == p2) ? "true" : "false") << endl;
    cout << "p1 == p1 ? " << ((p1 == p1) ? "true" : "false") << endl;
    cout << "p2 == p2 ? " << ((p2 == p2) ? "true" : "false") << endl;

    cout << "-----------------------------------------" << endl;
    cout << "Size" << endl;

    Size s1(2, 2);
    cout << "s1: ";
    s1.show();
    cout << ", area: " << s1.calculateArea();
    cout << endl;

    Size s2(3, 5);
    cout << "s2: ";
    s2.show();
    cout << ", area: " << s2.calculateArea();
    cout << endl;

    cout << endl;

    cout << "s1 == s2 ? " << ((s1 == s2) ? "true" : "false") << endl;
    cout << "s1 != s2 ? " << ((s1 != s2) ? "true" : "false") << endl;

    cout << "s1 <  s2 ? " << ((s1 <  s2) ? "true" : "false") << endl;
    cout << "s1 <= s2 ? " << ((s1 <= s2) ? "true" : "false") << endl;
    cout << "s1 >  s2 ? " << ((s1 >  s2) ? "true" : "false") << endl;
    cout << "s1 >= s2 ? " << ((s1 >= s2) ? "true" : "false") << endl;

    cout << "s1 <  10 ? " << ((s1 <  10) ? "true" : "false") << endl;
    cout << "s1 <= 10 ? " << ((s1 <= 10) ? "true" : "false") << endl;
    cout << "s1 >  10 ? " << ((s1 >  10) ? "true" : "false") << endl;
    cout << "s1 >= 10 ? " << ((s1 >= 10) ? "true" : "false") << endl;
    cout << "s2 <  10 ? " << ((s2 <  10) ? "true" : "false") << endl;
    cout << "s2 <= 10 ? " << ((s2 <= 10) ? "true" : "false") << endl;
    cout << "s2 >  10 ? " << ((s2 >  10) ? "true" : "false") << endl;
    cout << "s2 >= 10 ? " << ((s2 >= 10) ? "true" : "false") << endl;
    cout << endl;

    cout << "s1: ";
    s1.show();
    cout << endl << "s2: ";
    s2.show();
    cout << endl << "s1 += s2" << endl;
    s1 += s2;
    cout << "s1: ";
    s1.show();
    cout << endl;

    cout << endl;

    cout << "s1: ";
    s1.show();
    cout << endl << "s1 *= 2.0" << endl;
    s1 *= 2.0;
    cout << "s1: ";
    s1.show();
    cout << endl;

    cout << endl;

    return 0;
}

