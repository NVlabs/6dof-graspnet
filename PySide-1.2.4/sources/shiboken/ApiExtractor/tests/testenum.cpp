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

#include "testenum.h"
#include <QtTest/QTest>
#include "testutil.h"

void TestEnum::testEnumCppSignature()
{
    const char* cppCode ="\
    enum GlobalEnum { A, B };\
    \
    struct A {\
        enum ClassEnum { A, B };\
        void method(ClassEnum);\
    };\
    void func(A::ClassEnum);\
    ";
    const char* xmlCode = "\
    <typesystem package=\"Foo\"> \
        <enum-type name='GlobalEnum' />\
        <value-type name='A'> \
            <enum-type name='ClassEnum' />\
        </value-type> \
        <function signature='func(A::ClassEnum)' />\
    </typesystem>";

    TestUtil t(cppCode, xmlCode);
    AbstractMetaClassList classes = t.builder()->classes();
    QCOMPARE(classes.count(), 1);

    AbstractMetaEnumList globalEnums = t.builder()->globalEnums();
    QCOMPARE(globalEnums.count(), 1);
    QCOMPARE(globalEnums.first()->name(), QString("GlobalEnum"));

    // enum as parameter of a function
    AbstractMetaFunctionList functions = t.builder()->globalFunctions();
    QCOMPARE(functions.count(), 1);
    QCOMPARE(functions.first()->arguments().count(), 1);
    QCOMPARE(functions.first()->arguments().first()->type()->cppSignature(), QString("A::ClassEnum"));

    // enum as parameter of a method
    AbstractMetaClass* classA = classes.findClass("A");
    QCOMPARE(classA->enums().count(), 1);
    AbstractMetaFunctionList funcs = classA->queryFunctionsByName("method");
    QVERIFY(!funcs.isEmpty());
    AbstractMetaFunction* method = funcs.first();
    QVERIFY(method);
    AbstractMetaArgument* arg = method->arguments().first();
    QCOMPARE(arg->type()->name(), QString("ClassEnum"));
    QCOMPARE(arg->type()->cppSignature(), QString("A::ClassEnum"));
    QCOMPARE(functions.first()->arguments().count(), 1);
    arg = functions.first()->arguments().first();
    QCOMPARE(arg->type()->name(), QString("ClassEnum"));
    QCOMPARE(arg->type()->cppSignature(), QString("A::ClassEnum"));

    AbstractMetaEnumList classEnums = classA->enums();
    QCOMPARE(classEnums.first()->name(), QString("ClassEnum"));
}

void TestEnum::testEnumWithApiVersion()
{
    const char* cppCode ="\
    struct A {\
        enum ClassEnum { EnumA, EnumB };\
        enum ClassEnum2 { EnumC, EnumD };\
    };\
    ";
    const char* xmlCode = "\
    <typesystem package=\"Foo\"> \
        <value-type name='A'> \
            <enum-type name='ClassEnum' since='0.1'/>\
            <enum-type name='ClassEnum2' since='0.2'/>\
        </value-type> \
    </typesystem>";

    TestUtil t(cppCode, xmlCode, true, "0.1");
    AbstractMetaClassList classes = t.builder()->classes();
    QCOMPARE(classes.count(), 1);
    QCOMPARE(classes[0]->enums().count(), 1);
}

void TestEnum::testAnonymousEnum()
{
    const char* cppCode ="\
    enum { Global0, Global1 }; \
    struct A {\
        enum { A0, A1 };\
        enum { isThis = true, isThat = false };\
    };\
    ";
    const char* xmlCode = "\
    <typesystem package=\"Foo\"> \
        <!-- Uses the first value of the enum to identify it. -->\
        <enum-type identified-by-value='Global0'/>\
        <value-type name='A'> \
            <!-- Uses the second value of the enum to identify it. -->\
            <enum-type identified-by-value='A1'/>\
            <enum-type identified-by-value='isThis'/>\
        </value-type> \
    </typesystem>";

    TestUtil t(cppCode, xmlCode, false);

    AbstractMetaEnumList globalEnums = t.builder()->globalEnums();
    QCOMPARE(globalEnums.count(), 1);
    QCOMPARE(globalEnums.first()->typeEntry()->qualifiedCppName(), QString("Global0"));
    QVERIFY(globalEnums.first()->isAnonymous());

    AbstractMetaClassList classes = t.builder()->classes();
    QCOMPARE(classes.count(), 1);
    QCOMPARE(classes[0]->enums().count(), 2);

    AbstractMetaEnum* anonEnumA1 = classes[0]->findEnum("A1");
    QVERIFY(anonEnumA1);
    QVERIFY(anonEnumA1->isAnonymous());
    QCOMPARE(anonEnumA1->typeEntry()->qualifiedCppName(), QString("A::A1"));

    AbstractMetaEnumValue* enumValueA0 = anonEnumA1->values().first();
    QCOMPARE(enumValueA0->name(), QString("A0"));
    QCOMPARE(enumValueA0->value(), 0);
    QCOMPARE(enumValueA0->stringValue(), QString(""));

    AbstractMetaEnumValue* enumValueA1 = anonEnumA1->values().last();
    QCOMPARE(enumValueA1->name(), QString("A1"));
    QCOMPARE(enumValueA1->value(), 1);
    QCOMPARE(enumValueA1->stringValue(), QString(""));

    AbstractMetaEnum* anonEnumIsThis = classes[0]->findEnum("isThis");
    QVERIFY(anonEnumIsThis);
    QVERIFY(anonEnumIsThis->isAnonymous());
    QCOMPARE(anonEnumIsThis->typeEntry()->qualifiedCppName(), QString("A::isThis"));

    AbstractMetaEnumValue* enumValueIsThis = anonEnumIsThis->values().first();
    QCOMPARE(enumValueIsThis->name(), QString("isThis"));
    QCOMPARE(enumValueIsThis->value(), static_cast<int>(true));
    QCOMPARE(enumValueIsThis->stringValue(), QString("true"));

    AbstractMetaEnumValue* enumValueIsThat = anonEnumIsThis->values().last();
    QCOMPARE(enumValueIsThat->name(), QString("isThat"));
    QCOMPARE(enumValueIsThat->value(), static_cast<int>(false));
    QCOMPARE(enumValueIsThat->stringValue(), QString("false"));
}

void TestEnum::testGlobalEnums()
{
    const char* cppCode ="\
    enum EnumA { A0, A1 }; \
    enum EnumB { B0 = 2, B1 = 0x4 }; \
    ";
    const char* xmlCode = "\
    <typesystem package=\"Foo\"> \
        <enum-type name='EnumA'/>\
        <enum-type name='EnumB'/>\
    </typesystem>";

    TestUtil t(cppCode, xmlCode, false);

    AbstractMetaEnumList globalEnums = t.builder()->globalEnums();
    QCOMPARE(globalEnums.count(), 2);

    AbstractMetaEnum* enumA = globalEnums.first();
    QCOMPARE(enumA->typeEntry()->qualifiedCppName(), QString("EnumA"));

    AbstractMetaEnumValue* enumValueA0 = enumA->values().first();
    QCOMPARE(enumValueA0->name(), QString("A0"));
    QCOMPARE(enumValueA0->value(), 0);
    QCOMPARE(enumValueA0->stringValue(), QString(""));

    AbstractMetaEnumValue* enumValueA1 = enumA->values().last();
    QCOMPARE(enumValueA1->name(), QString("A1"));
    QCOMPARE(enumValueA1->value(), 1);
    QCOMPARE(enumValueA1->stringValue(), QString(""));

    AbstractMetaEnum* enumB = globalEnums.last();
    QCOMPARE(enumB->typeEntry()->qualifiedCppName(), QString("EnumB"));

    AbstractMetaEnumValue* enumValueB0 = enumB->values().first();
    QCOMPARE(enumValueB0->name(), QString("B0"));
    QCOMPARE(enumValueB0->value(), 2);
    QCOMPARE(enumValueB0->stringValue(), QString("2"));

    AbstractMetaEnumValue* enumValueB1 = enumB->values().last();
    QCOMPARE(enumValueB1->name(), QString("B1"));
    QCOMPARE(enumValueB1->value(), 4);
    QCOMPARE(enumValueB1->stringValue(), QString("0x4"));
}

void TestEnum::testEnumValueFromNeighbourEnum()
{
    const char* cppCode ="\
    namespace A {\
        enum EnumA { ValueA0, ValueA1 };\
        enum EnumB { ValueB0 = A::ValueA1, ValueB1 = ValueA0 };\
    };\
    ";
    const char* xmlCode = "\
    <typesystem package=\"Foo\"> \
        <namespace-type name='A'> \
            <enum-type name='EnumA'/>\
            <enum-type name='EnumB'/>\
        </namespace-type> \
    </typesystem>";

    TestUtil t(cppCode, xmlCode, false);

    AbstractMetaClassList classes = t.builder()->classes();
    QCOMPARE(classes.count(), 1);
    QCOMPARE(classes[0]->enums().count(), 2);

    AbstractMetaEnum* enumA = classes[0]->findEnum("EnumA");
    QVERIFY(enumA);
    QCOMPARE(enumA->typeEntry()->qualifiedCppName(), QString("A::EnumA"));

    AbstractMetaEnumValue* enumValueA0 = enumA->values().first();
    QCOMPARE(enumValueA0->name(), QString("ValueA0"));
    QCOMPARE(enumValueA0->value(), 0);
    QCOMPARE(enumValueA0->stringValue(), QString(""));

    AbstractMetaEnumValue* enumValueA1 = enumA->values().last();
    QCOMPARE(enumValueA1->name(), QString("ValueA1"));
    QCOMPARE(enumValueA1->value(), 1);
    QCOMPARE(enumValueA1->stringValue(), QString(""));

    AbstractMetaEnum* enumB = classes[0]->findEnum("EnumB");
    QVERIFY(enumB);
    QCOMPARE(enumB->typeEntry()->qualifiedCppName(), QString("A::EnumB"));

    AbstractMetaEnumValue* enumValueB0 = enumB->values().first();
    QCOMPARE(enumValueB0->name(), QString("ValueB0"));
    QCOMPARE(enumValueB0->value(), 1);
    QCOMPARE(enumValueB0->stringValue(), QString("A::ValueA1"));

    AbstractMetaEnumValue* enumValueB1 = enumB->values().last();
    QCOMPARE(enumValueB1->name(), QString("ValueB1"));
    QCOMPARE(enumValueB1->value(), 0);
    QCOMPARE(enumValueB1->stringValue(), QString("ValueA0"));
}

void TestEnum::testEnumValueFromExpression()
{
    const char* cppCode ="\
    struct A {\
        enum EnumA {\
            ValueA0 = 3u,\
            ValueA1 = ~3u,\
            ValueA2 = ~3,\
            ValueA3 = 0xf0,\
            ValueA4 = 8 |ValueA3,\
            ValueA5 = ValueA3|32,\
            ValueA6 = ValueA3 >> 1,\
            ValueA7 = ValueA3 << 1\
        };\
    };\
    ";
    const char* xmlCode = "\
    <typesystem package=\"Foo\"> \
        <value-type name='A'> \
            <enum-type name='EnumA'/>\
        </value-type> \
    </typesystem>";

    TestUtil t(cppCode, xmlCode, false);

    AbstractMetaClass* classA = t.builder()->classes().findClass("A");
    QVERIFY(classA);

    AbstractMetaEnum* enumA = classA->findEnum("EnumA");
    QVERIFY(enumA);
    QCOMPARE(enumA->typeEntry()->qualifiedCppName(), QString("A::EnumA"));

    AbstractMetaEnumValue* valueA0 = enumA->values().at(0);
    QCOMPARE(valueA0->name(), QString("ValueA0"));
    QCOMPARE(valueA0->stringValue(), QString("3u"));
    QCOMPARE(valueA0->value(), (int) 3u);

    AbstractMetaEnumValue* valueA1 = enumA->values().at(1);
    QCOMPARE(valueA1->name(), QString("ValueA1"));
    QCOMPARE(valueA1->stringValue(), QString("~3u"));
    QCOMPARE(valueA1->value(), (int) ~3u);

    AbstractMetaEnumValue* valueA2 = enumA->values().at(2);
    QCOMPARE(valueA2->name(), QString("ValueA2"));
    QCOMPARE(valueA2->stringValue(), QString("~3"));
    QCOMPARE(valueA2->value(), ~3);

    AbstractMetaEnumValue* valueA3 = enumA->values().at(3);
    QCOMPARE(valueA3->name(), QString("ValueA3"));
    QCOMPARE(valueA3->stringValue(), QString("0xf0"));
    QCOMPARE(valueA3->value(), 0xf0);

    AbstractMetaEnumValue* valueA4 = enumA->values().at(4);
    QCOMPARE(valueA4->name(), QString("ValueA4"));
    QCOMPARE(valueA4->stringValue(), QString("8|ValueA3"));
    QCOMPARE(valueA4->value(), 8|0xf0);

    AbstractMetaEnumValue* valueA5 = enumA->values().at(5);
    QCOMPARE(valueA5->name(), QString("ValueA5"));
    QCOMPARE(valueA5->stringValue(), QString("ValueA3|32"));
    QCOMPARE(valueA5->value(), 0xf0|32);

    AbstractMetaEnumValue* valueA6 = enumA->values().at(6);
    QCOMPARE(valueA6->name(), QString("ValueA6"));
    QCOMPARE(valueA6->stringValue(), QString("ValueA3>>1"));
    QCOMPARE(valueA6->value(), 0xf0 >> 1);

    AbstractMetaEnumValue* valueA7 = enumA->values().at(7);
    QCOMPARE(valueA7->name(), QString("ValueA7"));
    QCOMPARE(valueA7->stringValue(), QString("ValueA3<<1"));
    QCOMPARE(valueA7->value(), 0xf0 << 1);
}

void TestEnum::testPrivateEnum()
{
    const char* cppCode ="\
    class A {\
    private:\
        enum PrivateEnum { Priv0 = 0x0f, Priv1 = 0xf0 };\
    public:\
        enum PublicEnum { Pub0 = Priv0, Pub1 = A::Priv1 };\
    };\
    ";
    const char* xmlCode = "\
    <typesystem package=\"Foo\"> \
        <value-type name='A'> \
            <enum-type name='PublicEnum'/>\
        </value-type> \
    </typesystem>";

    TestUtil t(cppCode, xmlCode, false);

    AbstractMetaClass* classA = t.builder()->classes().findClass("A");
    QVERIFY(classA);
    QCOMPARE(classA->enums().count(), 2);

    AbstractMetaEnum* privateEnum = classA->findEnum("PrivateEnum");
    QVERIFY(privateEnum);
    QVERIFY(privateEnum->isPrivate());
    QCOMPARE(privateEnum->typeEntry()->qualifiedCppName(), QString("A::PrivateEnum"));

    AbstractMetaEnum* publicEnum = classA->findEnum("PublicEnum");
    QVERIFY(publicEnum);
    QCOMPARE(publicEnum->typeEntry()->qualifiedCppName(), QString("A::PublicEnum"));

    AbstractMetaEnumValue* pub0 = publicEnum->values().first();
    QCOMPARE(pub0->name(), QString("Pub0"));
    QCOMPARE(pub0->value(), 0x0f);
    QCOMPARE(pub0->stringValue(), QString("Priv0"));

    AbstractMetaEnumValue* pub1 = publicEnum->values().last();
    QCOMPARE(pub1->name(), QString("Pub1"));
    QCOMPARE(pub1->value(), 0xf0);
    QCOMPARE(pub1->stringValue(), QString("A::Priv1"));
}

void TestEnum::testTypedefEnum()
{
    const char* cppCode ="\
    typedef enum EnumA { \
        A0, \
        A1, \
    } EnumA ; \
    ";
    const char* xmlCode = "\
    <typesystem package=\"Foo\"> \
        <enum-type name='EnumA'/>\
    </typesystem>";

    TestUtil t(cppCode, xmlCode, false);

    AbstractMetaEnumList globalEnums = t.builder()->globalEnums();
    QEXPECT_FAIL("", "APIExtractor does not handle typedef enum correctly yet", Abort);
    QCOMPARE(globalEnums.count(), 1);

    AbstractMetaEnum* enumA = globalEnums.first();
    QCOMPARE(enumA->typeEntry()->qualifiedCppName(), QString("EnumA"));

    AbstractMetaEnumValue* enumValueA0 = enumA->values().first();
    QCOMPARE(enumValueA0->name(), QString("A0"));
    QCOMPARE(enumValueA0->value(), 0);
    QCOMPARE(enumValueA0->stringValue(), QString(""));

    AbstractMetaEnumValue* enumValueA1 = enumA->values().last();
    QCOMPARE(enumValueA1->name(), QString("A1"));
    QCOMPARE(enumValueA1->value(), 1);
    QCOMPARE(enumValueA1->stringValue(), QString(""));
}

QTEST_APPLESS_MAIN(TestEnum)

#include "testenum.moc"
