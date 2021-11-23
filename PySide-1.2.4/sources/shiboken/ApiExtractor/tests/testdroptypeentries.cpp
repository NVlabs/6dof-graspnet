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

#include "testdroptypeentries.h"
#include <QtTest/QTest>
#include "testutil.h"

static const char* cppCode ="\
    struct ValueA {};\
    struct ValueB {};\
    struct ObjectA {};\
    struct ObjectB {};\
    namespace NamespaceA {\
        struct InnerClassA {};\
        namespace InnerNamespaceA {}\
    }\
    namespace NamespaceB {}\
    enum EnumA { Value0 };\
    enum EnumB { Value1 };\
    void funcA();\
    void funcB();\
";

static const char* xmlCode = "\
<typesystem package='Foo'>\
    <value-type name='ValueA' />\
    <value-type name='ValueB' />\
    <object-type name='ObjectA' />\
    <object-type name='ObjectB' />\
    <namespace-type name='NamespaceA'>\
        <value-type name='InnerClassA' />\
        <namespace-type name='InnerNamespaceA' />\
    </namespace-type>\
    <namespace-type name='NamespaceB' />\
    <enum-type name='EnumA' />\
    <enum-type name='EnumB' />\
    <function signature='funcA()' />\
    <function signature='funcB()' />\
</typesystem>";

void TestDropTypeEntries::testDropEntries()
{
    QStringList droppedEntries("Foo.ValueB");
    droppedEntries << "Foo.ObjectB" << "Foo.NamespaceA.InnerClassA";
    droppedEntries << "Foo.NamespaceB" << "Foo.EnumB" << "Foo.funcB()";
    droppedEntries << "Foo.NamespaceA.InnerNamespaceA";
    TestUtil t(cppCode, xmlCode, false, 0, droppedEntries);

    AbstractMetaClassList classes = t.builder()->classes();
    QVERIFY(classes.findClass("ValueA"));
    QVERIFY(!classes.findClass("ValueB"));
    QVERIFY(classes.findClass("ObjectA"));
    QVERIFY(!classes.findClass("ObjectB"));
    QVERIFY(classes.findClass("NamespaceA"));
    QVERIFY(!classes.findClass("NamespaceA::InnerClassA"));
    QVERIFY(!classes.findClass("NamespaceB"));

    AbstractMetaEnumList globalEnums = t.builder()->globalEnums();
    QCOMPARE(globalEnums.count(), 1);
    QCOMPARE(globalEnums.first()->name(), QString("EnumA"));

    TypeDatabase* td = TypeDatabase::instance();
    QVERIFY(td->findType("funcA"));
    QVERIFY(!td->findType("funcB"));
}

void TestDropTypeEntries::testDontDropEntries()
{
    TestUtil t(cppCode, xmlCode, false);

    AbstractMetaClassList classes = t.builder()->classes();
    QVERIFY(classes.findClass("ValueA"));
    QVERIFY(classes.findClass("ValueB"));
    QVERIFY(classes.findClass("ObjectA"));
    QVERIFY(classes.findClass("ObjectB"));
    QVERIFY(classes.findClass("NamespaceA"));
    QVERIFY(classes.findClass("NamespaceA::InnerClassA"));
    QVERIFY(classes.findClass("NamespaceB"));

    QCOMPARE(t.builder()->globalEnums().size(), 2);

    TypeDatabase* td = TypeDatabase::instance();
    QVERIFY(td->findType("funcA"));
    QVERIFY(td->findType("funcB"));
}

static const char* cppCode2 ="\
    struct ValueA {\
        void func();\
    };\
";

static const char* xmlCode2 = "\
<typesystem package='Foo'>\
    <value-type name='ValueA'>\
        <modify-function signature='func()'>\
            <remove class='all' />\
        </modify-function>\
    </value-type>\
</typesystem>";

void TestDropTypeEntries::testDropEntryWithChildTags()
{
    QStringList droppedEntries("Foo.ValueA");
    TestUtil t(cppCode2, xmlCode2, false, 0, droppedEntries);
    QVERIFY(!t.builder()->classes().findClass("ValueA"));
}

void TestDropTypeEntries::testDontDropEntryWithChildTags()
{
    TestUtil t(cppCode2, xmlCode2, false);
    QVERIFY(t.builder()->classes().findClass("ValueA"));
}

QTEST_APPLESS_MAIN(TestDropTypeEntries)

#include "testdroptypeentries.moc"

