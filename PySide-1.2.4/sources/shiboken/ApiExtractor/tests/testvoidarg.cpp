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

#include "testvoidarg.h"
#include <QtTest/QTest>
#include "testutil.h"


void TestVoidArg::testVoidParsedFunction()
{
    const char cppCode[] = "struct A { void a(void); };";
    const char xmlCode[] = "\
    <typesystem package=\"Foo\">\
        <value-type name='A'/>\
    </typesystem>";
    TestUtil t(cppCode, xmlCode);
    AbstractMetaClassList classes = t.builder()->classes();
    AbstractMetaClass* classA = classes.findClass("A");
    QVERIFY(classA);
    const AbstractMetaFunction* addedFunc = classA->findFunction("a");
    QCOMPARE(addedFunc->arguments().count(), 0);
}

void TestVoidArg::testVoidAddedFunction()
{
    const char cppCode[] = "struct A { };";
    const char xmlCode[] = "\
    <typesystem package=\"Foo\">\
        <value-type name='A' >\
            <add-function signature=\"a(void)\"/>\
        </value-type>\
    </typesystem>";
    TestUtil t(cppCode, xmlCode);
    AbstractMetaClassList classes = t.builder()->classes();
    AbstractMetaClass* classA = classes.findClass("A");
    QVERIFY(classA);
    const AbstractMetaFunction* addedFunc = classA->findFunction("a");
    QCOMPARE(addedFunc->arguments().count(), 0);

}

void TestVoidArg::testVoidPointerParsedFunction()
{
    const char cppCode[] = "struct A { void a(void*); };";
    const char xmlCode[] = "\
    <typesystem package=\"Foo\">\
        <value-type name='A' />\
    </typesystem>";
    TestUtil t(cppCode, xmlCode);
    AbstractMetaClassList classes = t.builder()->classes();
    AbstractMetaClass* classA = classes.findClass("A");
    QVERIFY(classA);
    const AbstractMetaFunction* addedFunc = classA->findFunction("a");
    QCOMPARE(addedFunc->arguments().count(), 1);

}

QTEST_APPLESS_MAIN(TestVoidArg)

#include "testvoidarg.moc"

