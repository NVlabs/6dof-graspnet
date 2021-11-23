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

#include "testrefcounttag.h"
#include <QtTest/QTest>
#include "testutil.h"

void TestRefCountTag::testReferenceCountTag()
{
    const char* cppCode ="\
    struct A {};\
    struct B {\
        void keepObject(B* b);\
    };\
    ";
    const char* xmlCode = "\
    <typesystem package=\"Foo\"> \
        <object-type name='A' /> \
        <object-type name='B'> \
        <modify-function signature='keepObject(B*)'>\
            <modify-argument index='1'>\
                <reference-count action='add' /> \
            </modify-argument>\
        </modify-function>\
        </object-type>\
    </typesystem>";
    TestUtil t(cppCode, xmlCode, false);
    AbstractMetaClassList classes = t.builder()->classes();
    AbstractMetaClass* classB = classes.findClass("B");
    const AbstractMetaFunction* func = classB->findFunction("keepObject");
    QVERIFY(func);
    ReferenceCount refCount = func->modifications().first().argument_mods.first().referenceCounts.first();
    QCOMPARE(refCount.action, ReferenceCount::Add);
}

void TestRefCountTag::testWithApiVersion()
{
    const char* cppCode ="\
    struct A {};\
    struct B {\
        void keepObject(B*, B*);\
    };\
    ";
    const char* xmlCode = "\
    <typesystem package=\"Foo\"> \
        <object-type name='A' /> \
        <object-type name='B'> \
        <modify-function signature='keepObject(B*, B*)'>\
            <modify-argument index='1' since='0.1'>\
                <reference-count action='add' /> \
            </modify-argument>\
            <modify-argument index='2' since='0.2'>\
                <reference-count action='add' /> \
            </modify-argument>\
        </modify-function>\
        </object-type>\
    </typesystem>";

    TestUtil t(cppCode, xmlCode, false, "0.1");
    AbstractMetaClassList classes = t.builder()->classes();
    AbstractMetaClass* classB = classes.findClass("B");
    const AbstractMetaFunction* func = classB->findFunction("keepObject");
    QVERIFY(func);
    ReferenceCount refCount = func->modifications().first().argument_mods.first().referenceCounts.first();
    QCOMPARE(refCount.action, ReferenceCount::Add);

    QCOMPARE(func->modifications().size(), 1);
}


QTEST_APPLESS_MAIN(TestRefCountTag)

#include "testrefcounttag.moc"

