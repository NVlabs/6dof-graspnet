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

#include "testreferencetopointer.h"
#include <QtTest/QTest>
#include "testutil.h"

void TestReferenceToPointer::testReferenceToPointerArgument()
{
    const char* cppCode ="\
    struct A {};\
    struct B {\
        void dummy(A*&);\
    };\
    ";
    const char* xmlCode = "\
    <typesystem package=\"Foo\"> \
        <object-type name='A' /> \
        <object-type name='B' /> \
    </typesystem>";
    TestUtil t(cppCode, xmlCode, false);
    AbstractMetaClassList classes = t.builder()->classes();
    AbstractMetaClass* classB = classes.findClass("B");
    QVERIFY(classB);
    const AbstractMetaFunction* func = classB->findFunction("dummy");
    QVERIFY(func);
    QCOMPARE(func->arguments().first()->type()->minimalSignature(), QString("A*&"));
}

QTEST_APPLESS_MAIN(TestReferenceToPointer)

#include "testreferencetopointer.moc"

