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

#include "testdtorinformation.h"
#include "abstractmetabuilder.h"
#include <QtTest/QTest>
#include "testutil.h"

void TestDtorInformation::testDtorIsPrivate()
{
    const char* cppCode ="class Control { public: ~Control() {} }; class Subject { private: ~Subject() {} };";
    const char* xmlCode = "<typesystem package=\"Foo\"><value-type name=\"Control\"/><value-type name=\"Subject\"/></typesystem>";
    TestUtil t(cppCode, xmlCode);
    AbstractMetaClassList classes = t.builder()->classes();
    QCOMPARE(classes.count(), 2);
    QCOMPARE(classes.findClass("Control")->hasPrivateDestructor(), false);
    QCOMPARE(classes.findClass("Subject")->hasPrivateDestructor(), true);
}

void TestDtorInformation::testDtorIsProtected()
{
    const char* cppCode ="class Control { public: ~Control() {} }; class Subject { protected: ~Subject() {} };";
    const char* xmlCode = "<typesystem package=\"Foo\"><value-type name=\"Control\"/><value-type name=\"Subject\"/></typesystem>";
    TestUtil t(cppCode, xmlCode);
    AbstractMetaClassList classes = t.builder()->classes();
    QCOMPARE(classes.count(), 2);
    QCOMPARE(classes.findClass("Control")->hasProtectedDestructor(), false);
    QCOMPARE(classes.findClass("Subject")->hasProtectedDestructor(), true);
}

void TestDtorInformation::testDtorIsVirtual()
{
    const char* cppCode ="class Control { public: ~Control() {} }; class Subject { protected: virtual ~Subject() {} };";
    const char* xmlCode = "<typesystem package=\"Foo\"><value-type name=\"Control\"/><value-type name=\"Subject\"/></typesystem>";
    TestUtil t(cppCode, xmlCode);
    AbstractMetaClassList classes = t.builder()->classes();
    QCOMPARE(classes.count(), 2);
    QCOMPARE(classes.findClass("Control")->hasVirtualDestructor(), false);
    QCOMPARE(classes.findClass("Subject")->hasVirtualDestructor(), true);
}

void TestDtorInformation::testClassWithVirtualDtorIsPolymorphic()
{
    const char* cppCode ="class Control { public: virtual ~Control() {} }; class Subject { protected: virtual ~Subject() {} };";
    const char* xmlCode = "<typesystem package=\"Foo\"><value-type name=\"Control\"/><value-type name=\"Subject\"/></typesystem>";
    TestUtil t(cppCode, xmlCode);
    AbstractMetaClassList classes = t.builder()->classes();
    QCOMPARE(classes.count(), 2);
    QCOMPARE(classes.findClass("Control")->isPolymorphic(), true);
    QCOMPARE(classes.findClass("Subject")->isPolymorphic(), true);
}

QTEST_APPLESS_MAIN(TestDtorInformation)

#include "testdtorinformation.moc"

