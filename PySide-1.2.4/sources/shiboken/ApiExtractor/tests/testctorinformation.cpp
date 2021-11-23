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

#include "testctorinformation.h"
#include "abstractmetabuilder.h"
#include <QtTest/QTest>
#include "testutil.h"

void TestCtorInformation::testCtorIsPrivate()
{
    const char* cppCode = "class Control { public: Control() {} };\
                           class Subject { private: Subject() {} };\
                           class CtorLess { };";
    const char* xmlCode = "<typesystem package='Foo'>\
                                <value-type name='Control'/>\
                                <object-type name='Subject'/>\
                                <value-type name='CtorLess'/>\
                           </typesystem>";
    TestUtil t(cppCode, xmlCode);
    AbstractMetaClassList classes = t.builder()->classes();
    QCOMPARE(classes.count(), 3);
    QCOMPARE(classes.findClass("Control")->hasNonPrivateConstructor(), true);
    QCOMPARE(classes.findClass("Subject")->hasNonPrivateConstructor(), false);
    QCOMPARE(classes.findClass("CtorLess")->hasNonPrivateConstructor(), true);
}

void TestCtorInformation::testHasNonPrivateCtor()
{
    const char* cppCode = "template<typename T>\
                           struct Base { Base(double) {} };\
                           typedef Base<int> Derived;\
                          ";
    const char* xmlCode = "<typesystem package='Foo'>\
                                <primitive-type name='int' />\
                                <primitive-type name='double' />\
                                <object-type name='Base' generate='no'/>\
                                <object-type name='Derived'/>\
                           </typesystem>";
    TestUtil t(cppCode, xmlCode);
    AbstractMetaClassList classes = t.builder()->classes();
    QCOMPARE(classes.count(), 2);
    AbstractMetaClass* base = classes.findClass("Base");
    QCOMPARE(base->hasNonPrivateConstructor(), true);
    AbstractMetaClass* derived = classes.findClass("Derived");
    QCOMPARE(derived->hasNonPrivateConstructor(), true);
}

QTEST_APPLESS_MAIN(TestCtorInformation)

#include "testctorinformation.moc"

