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

#include "testimplicitconversions.h"
#include "testutil.h"
#include <QtTest/QTest>

void TestImplicitConversions::testWithPrivateCtors()
{
    const char* cppCode ="\
    class B;\
    class C;\
    class A {\
        A(const B&);\
    public:\
        A(const C&);\
    };\
    class B {};\
    class C {};\
    ";
    const char* xmlCode = "\
    <typesystem package='Foo'> \
        <value-type name='A'/> \
        <value-type name='B'/> \
        <value-type name='C'/> \
    </typesystem>";
    TestUtil t(cppCode, xmlCode);
    AbstractMetaClassList classes = t.builder()->classes();
    QCOMPARE(classes.count(), 3);

    AbstractMetaClass* classA = classes.findClass("A");
    AbstractMetaClass* classC = classes.findClass("C");
    AbstractMetaFunctionList implicitConvs = classA->implicitConversions();
    QCOMPARE(implicitConvs.count(), 1);
    QCOMPARE(implicitConvs.first()->arguments().first()->type()->typeEntry(), classC->typeEntry());
}

void TestImplicitConversions::testWithModifiedVisibility()
{
    const char* cppCode ="\
    class B;\
    class A {\
    public:\
        A(const B&);\
    };\
    class B {};\
    ";
    const char* xmlCode = "\
    <typesystem package='Foo'>\
        <value-type name='A'>\
            <modify-function signature='A(const B&amp;)'>\
                <access modifier='private' />\
            </modify-function>\
        </value-type>\
        <value-type name='B'/>\
    </typesystem>";
    TestUtil t(cppCode, xmlCode);
    AbstractMetaClassList classes = t.builder()->classes();
    QCOMPARE(classes.count(), 2);
    AbstractMetaClass* classA = classes.findClass("A");
    AbstractMetaClass* classB = classes.findClass("B");
    AbstractMetaFunctionList implicitConvs = classA->implicitConversions();
    QCOMPARE(implicitConvs.count(), 1);
    QCOMPARE(implicitConvs.first()->arguments().first()->type()->typeEntry(), classB->typeEntry());
}


void TestImplicitConversions::testWithAddedCtor()
{
    const char* cppCode ="\
    class B;\
    class A {\
    public:\
        A(const B&);\
    };\
    class B {};\
    class C {};\
    ";
    const char* xmlCode = "\
    <typesystem package='Foo'>\
        <custom-type name='TARGETLANGTYPE' />\
        <value-type name='A'>\
            <add-function signature='A(const C&amp;)' />\
        </value-type>\
        <value-type name='B'>\
            <add-function signature='B(TARGETLANGTYPE*)' />\
        </value-type>\
        <value-type name='C'/>\
    </typesystem>";
    TestUtil t(cppCode, xmlCode);
    AbstractMetaClassList classes = t.builder()->classes();
    QCOMPARE(classes.count(), 3);

    AbstractMetaClass* classA = classes.findClass("A");
    AbstractMetaFunctionList implicitConvs = classA->implicitConversions();
    QCOMPARE(implicitConvs.count(), 2);

    // Added constructors with custom types should never result in implicit converters.
    AbstractMetaClass* classB = classes.findClass("B");
    implicitConvs = classB->implicitConversions();
    QCOMPARE(implicitConvs.count(), 0);
}

void TestImplicitConversions::testWithExternalConversionOperator()
{
    const char* cppCode ="\
    class A {};\
    struct B {\
        operator A() const;\
    };\
    ";
    const char* xmlCode = "\
    <typesystem package='Foo'>\
        <value-type name='A'/>\
        <value-type name='B'/>\
    </typesystem>";
    TestUtil t(cppCode, xmlCode);
    AbstractMetaClassList classes = t.builder()->classes();
    QCOMPARE(classes.count(), 2);
    AbstractMetaClass* classA = classes.findClass("A");
    AbstractMetaClass* classB = classes.findClass("B");
    AbstractMetaFunctionList implicitConvs = classA->implicitConversions();
    QCOMPARE(implicitConvs.count(), 1);
    AbstractMetaFunctionList externalConvOps = classA->externalConversionOperators();
    QCOMPARE(externalConvOps.count(), 1);

    const AbstractMetaFunction* convOp = 0;
    foreach(const AbstractMetaFunction* func, classB->functions()) {
        if (func->isConversionOperator())
            convOp = func;
    }
    QVERIFY(convOp);
    QCOMPARE(implicitConvs.first(), convOp);
}

QTEST_APPLESS_MAIN(TestImplicitConversions)

#include "testimplicitconversions.moc"
