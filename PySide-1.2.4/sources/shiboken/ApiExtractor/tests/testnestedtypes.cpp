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

#include "testnestedtypes.h"
#include <QtTest/QTest>
#include "testutil.h"

void TestNestedTypes::testNestedTypesModifications()
{
    const char* cppCode ="\
    namespace OuterNamespace {\
        namespace InnerNamespace {\
            struct SomeClass {\
                void method() {}\
            };\
        };\
    };\
    ";
    const char* xmlCode = "\
    <typesystem package='Foo'> \
        <namespace-type name='OuterNamespace'>\
            <namespace-type name='InnerNamespace'>\
                <inject-code class='native'>custom_code1();</inject-code>\
                <add-function signature='method()' return-type='OuterNamespace::InnerNamespace::SomeClass'>\
                    <inject-code class='target'>custom_code2();</inject-code>\
                </add-function>\
                <object-type name='SomeClass' target-lang-name='RenamedSomeClass'>\
                    <modify-function signature='method()' remove='all'/>\
                </object-type>\
            </namespace-type>\
        </namespace-type>\
    </typesystem>";

    TestUtil t(cppCode, xmlCode, false);
    AbstractMetaClassList classes = t.builder()->classes();

    AbstractMetaClass* ons = classes.findClass("OuterNamespace");
    QVERIFY(ons);

    AbstractMetaClass* ins = classes.findClass("OuterNamespace::InnerNamespace");
    QVERIFY(ins);
    QCOMPARE(ins->functions().count(), 1);
    QCOMPARE(ins->typeEntry()->codeSnips().count(), 1);
    CodeSnip snip = ins->typeEntry()->codeSnips().first();
    QCOMPARE(snip.code(), QString("custom_code1();"));

    AbstractMetaFunction* addedFunc = ins->functions().first();
    QVERIFY(addedFunc->isUserAdded());
    QCOMPARE(addedFunc->visibility(), uint(AbstractMetaFunction::Public));
    QCOMPARE(addedFunc->functionType(), AbstractMetaFunction::NormalFunction);
    QCOMPARE(addedFunc->type()->minimalSignature(), QString("OuterNamespace::InnerNamespace::SomeClass"));

    QCOMPARE(addedFunc->modifications().size(), 1);
    QVERIFY(addedFunc->modifications().first().isCodeInjection());
    snip = addedFunc->modifications().first().snips.first();
    QCOMPARE(snip.code(), QString("custom_code2();"));

    AbstractMetaClass* sc = classes.findClass("OuterNamespace::InnerNamespace::SomeClass");
    QVERIFY(ins);
    QCOMPARE(sc->functions().count(), 2); // default constructor and removed method
    AbstractMetaFunction* removedFunc = sc->functions().last();
    QVERIFY(removedFunc->isModifiedRemoved());
}


void TestNestedTypes::testDuplicationOfNestedTypes()
{
    const char* cppCode ="\
    namespace Namespace {\
        class SomeClass {};\
    };";
    const char* xmlCode = "\
    <typesystem package='Foo'> \
        <namespace-type name='Namespace'>\
            <value-type name='SomeClass'>\
                <add-function signature='createSomeClass(Namespace::SomeClass)'/>\
            </value-type>\
        </namespace-type>\
    </typesystem>";

    TestUtil t(cppCode, xmlCode, false);
    AbstractMetaClassList classes = t.builder()->classes();
    QCOMPARE(classes.count(), 2);
    AbstractMetaClass* nspace = classes.findClass("Namespace");
    QVERIFY(nspace);
    AbstractMetaClass* cls1 = classes.findClass("SomeClass");
    QVERIFY(cls1);
    AbstractMetaClass* cls2 = classes.findClass("Namespace::SomeClass");
    QVERIFY(cls2);
    QCOMPARE(cls1, cls2);
    QCOMPARE(cls1->name(), QString("SomeClass"));
    QCOMPARE(cls1->qualifiedCppName(), QString("Namespace::SomeClass"));

    TypeEntry* t1 = TypeDatabase::instance()->findType("Namespace::SomeClass");
    QVERIFY(t1);
    TypeEntry* t2 = TypeDatabase::instance()->findType("SomeClass");
    QVERIFY(!t2);
}

QTEST_APPLESS_MAIN(TestNestedTypes)

#include "testnestedtypes.moc"
