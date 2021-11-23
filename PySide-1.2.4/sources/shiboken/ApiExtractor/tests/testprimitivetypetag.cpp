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

#include "testprimitivetypetag.h"
#include <QtTest/QTest>
#include "testutil.h"

void TestPrimitiveTypeTag::testPrimitiveTypeDefaultConstructor()
{
    const char* cppCode ="\
    struct A {};\
    struct B {};\
    ";
    const char* xmlCode = "\
    <typesystem package=\"Foo\"> \
        <primitive-type name='A' default-constructor='A()'/> \
        <object-type name='B' /> \
    </typesystem>";
    TestUtil t(cppCode, xmlCode, false);

    AbstractMetaClassList classes = t.builder()->classes();
    QCOMPARE(classes.count(), 1);
    AbstractMetaClass* classB = classes.findClass("B");
    QVERIFY(classB);

    PrimitiveTypeEntry* typeEntry = TypeDatabase::instance()->findPrimitiveType("A");
    QVERIFY(typeEntry);
    QVERIFY(typeEntry->hasDefaultConstructor());
    QCOMPARE(typeEntry->defaultConstructor(), QString("A()"));
}

QTEST_APPLESS_MAIN(TestPrimitiveTypeTag)

#include "testprimitivetypetag.moc"

