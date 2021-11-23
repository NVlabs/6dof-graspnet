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

#include "testarrayargument.h"
#include <QtTest/QTest>
#include "testutil.h"

void TestArrayArgument::testArrayArgumentWithSizeDefinedByInteger()
{
    const char* cppCode ="\
    struct A { \
        enum SomeEnum { Value0, Value1, NValues }; \
        void method(double[3]); \
    };";
    const char* xmlCode = "\
    <typesystem package='Foo'> \
        <primitive-type name='double'/>\
        <object-type name='A'>\
            <enum-type name='SomeEnum'/>\
        </object-type>\
    </typesystem>";

    TestUtil t(cppCode, xmlCode, false);
    AbstractMetaClass* classA = t.builder()->classes().findClass("A");
    QVERIFY(classA);

    const AbstractMetaArgument* arg = classA->functions().last()->arguments().first();
    QVERIFY(arg->type()->isArray());
    QCOMPARE(arg->type()->arrayElementCount(), 3);
    QCOMPARE(arg->type()->arrayElementType()->name(), QString("double"));
}

void TestArrayArgument::testArrayArgumentWithSizeDefinedByEnumValue()
{
    const char* cppCode ="\
    struct A { \
        enum SomeEnum { Value0, Value1, NValues }; \
        void method(double[NValues]); \
    };";
    const char* xmlCode = "\
    <typesystem package='Foo'> \
        <primitive-type name='double'/>\
        <object-type name='A'>\
            <enum-type name='SomeEnum'/>\
        </object-type>\
    </typesystem>";

    TestUtil t(cppCode, xmlCode, false);
    AbstractMetaClass* classA = t.builder()->classes().findClass("A");
    QVERIFY(classA);

    AbstractMetaEnum* someEnum = classA->findEnum("SomeEnum");
    QVERIFY(someEnum);
    AbstractMetaEnumValue* nvalues = classA->findEnumValue("NValues", someEnum);
    QVERIFY(nvalues);

    const AbstractMetaArgument* arg = classA->functions().last()->arguments().first();
    QVERIFY(arg->type()->isArray());
    QCOMPARE(arg->type()->arrayElementCount(), nvalues->value());
    QCOMPARE(arg->type()->arrayElementType()->name(), QString("double"));
};

void TestArrayArgument::testArrayArgumentWithSizeDefinedByEnumValueFromGlobalEnum()
{
    const char* cppCode ="\
    enum SomeEnum { Value0, Value1, NValues }; \
    struct A { \
        void method(double[NValues]); \
    };";
    const char* xmlCode = "\
    <typesystem package='Foo'> \
        <primitive-type name='double'/>\
        <enum-type name='SomeEnum'/>\
        <object-type name='A'>\
        </object-type>\
    </typesystem>";

    TestUtil t(cppCode, xmlCode, false);
    AbstractMetaClass* classA = t.builder()->classes().findClass("A");
    QVERIFY(classA);

    AbstractMetaEnum* someEnum = t.builder()->globalEnums().first();
    QVERIFY(someEnum);
    AbstractMetaEnumValue* nvalues = 0;
    foreach (AbstractMetaEnumValue* enumValue, someEnum->values()) {
        if (enumValue->name() == "NValues") {
            nvalues = enumValue;
            break;
        }
    }
    QVERIFY(nvalues);

    const AbstractMetaArgument* arg = classA->functions().last()->arguments().first();
    QVERIFY(arg->type()->isArray());
    QCOMPARE(arg->type()->arrayElementCount(), nvalues->value());
    QCOMPARE(arg->type()->arrayElementType()->name(), QString("double"));
};

QTEST_APPLESS_MAIN(TestArrayArgument)

#include "testarrayargument.moc"
