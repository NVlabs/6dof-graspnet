/*
* This file is part of the API Extractor project.
*
* Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
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

#include "testremoveimplconv.h"
#include "testutil.h"
#include <QtTest/QTest>

// When a constructor able to trigger implicity conversions is removed
// it should not appear in the implicity conversion list.
void TestRemoveImplConv::testRemoveImplConv()
{
    const char* cppCode ="\
    struct A {};\
    struct B {};\
    struct C {\
        C(const A&);\
        C(const B&);\
    };\
    ";
    const char* xmlCode = "\
    <typesystem package=\"Foo\"> \
        <value-type name='A' /> \
        <value-type name='B' /> \
        <value-type name='C'> \
            <modify-function signature='C(const A&amp;)' remove='all' />\
        </value-type>\
    </typesystem>";
    TestUtil t(cppCode, xmlCode);
    AbstractMetaClassList classes = t.builder()->classes();
    QCOMPARE(classes.count(), 3);
    AbstractMetaClass* classA = classes.findClass("A");
    QVERIFY(classA);
    AbstractMetaClass* classB = classes.findClass("B");
    QVERIFY(classB);
    AbstractMetaClass* classC = classes.findClass("C");
    QVERIFY(classC);
    AbstractMetaFunctionList implConv = classC->implicitConversions();
    QCOMPARE(implConv.count(), 1);
    QCOMPARE(implConv.first()->arguments().first()->type()->typeEntry(), classB->typeEntry());
}

QTEST_APPLESS_MAIN(TestRemoveImplConv)

#include "testremoveimplconv.moc"
