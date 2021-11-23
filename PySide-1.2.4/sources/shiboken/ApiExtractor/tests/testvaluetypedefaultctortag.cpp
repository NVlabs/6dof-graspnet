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

#include "testvaluetypedefaultctortag.h"
#include <QtTest/QTest>
#include "testutil.h"

void TestValueTypeDefaultCtorTag::testValueTypeDefaultCtorTagArgument()
{
    const char* cppCode ="\
    struct A {\
        A(int,int);\
    };\
    struct B {};\
    ";
    const char* xmlCode = "\
    <typesystem package='Foo'>\
        <primitive-type name='int' />\
        <value-type name='A' default-constructor='A(0, 0)' />\
        <value-type name='B' />\
    </typesystem>";
    TestUtil t(cppCode, xmlCode, false);
    AbstractMetaClassList classes = t.builder()->classes();

    AbstractMetaClass* classA = classes.findClass("A");
    QVERIFY(classA);
    QVERIFY(classA->typeEntry()->hasDefaultConstructor());
    QCOMPARE(classA->typeEntry()->defaultConstructor(), QString("A(0, 0)"));

    AbstractMetaClass* classB = classes.findClass("B");
    QVERIFY(classB);
    QVERIFY(!classB->typeEntry()->hasDefaultConstructor());
}

QTEST_APPLESS_MAIN(TestValueTypeDefaultCtorTag)

#include "testvaluetypedefaultctortag.moc"
