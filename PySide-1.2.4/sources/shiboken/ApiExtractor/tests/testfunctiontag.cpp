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

#include "testfunctiontag.h"
#include <QtTest/QTest>
#include "testutil.h"

void TestFunctionTag::testFunctionTagForSpecificSignature()
{
    const char cppCode[] = "void globalFunction(int); void globalFunction(float); void dummy()";
    const char xmlCode[] = "\
    <typesystem package=\"Foo\">\
        <primitive-type name='int'/> \
        <primitive-type name='float'/> \
        <function signature='globalFunction(int)'/>\
    </typesystem>";
    TestUtil t(cppCode, xmlCode, false);

    FunctionTypeEntry* func = (FunctionTypeEntry*) TypeDatabase::instance()->findType("globalFunction");
    QVERIFY(func);
    QCOMPARE(t.builder()->globalFunctions().size(), 1);
}

void TestFunctionTag::testFunctionTagForAllSignatures()
{
    const char cppCode[] = "void globalFunction(int); void globalFunction(float); void dummy();";
    const char xmlCode[] = "\
    <typesystem package=\"Foo\">\
        <primitive-type name='int'/> \
        <primitive-type name='float'/> \
        <function signature='globalFunction(int)'/>\
        <function signature='globalFunction(float)'/>\
    </typesystem>";
    TestUtil t(cppCode, xmlCode, false);

    FunctionTypeEntry* func = (FunctionTypeEntry*) TypeDatabase::instance()->findType("globalFunction");
    QVERIFY(func);
    QCOMPARE(t.builder()->globalFunctions().size(), 2);
}

void TestFunctionTag::testRenameGlobalFunction()
{
    const char* cppCode ="void global_function_with_ugly_name();";
    const char* xmlCode = "\
    <typesystem package='Foo'> \
        <function signature='global_function_with_ugly_name()' rename='smooth' />\
    </typesystem>";
    TestUtil t(cppCode, xmlCode, false);

    FunctionTypeEntry* func = (FunctionTypeEntry*) TypeDatabase::instance()->findType("global_function_with_ugly_name");
    QVERIFY(func);

    QCOMPARE(t.builder()->globalFunctions().size(), 1);
    const AbstractMetaFunction* metaFunc = t.builder()->globalFunctions().first();

    QVERIFY(metaFunc);
    QCOMPARE(metaFunc->modifications().size(), 1);
    QVERIFY(metaFunc->modifications().first().isRenameModifier());
    QCOMPARE(metaFunc->modifications().first().renamedTo(), QString("smooth"));

    QCOMPARE(metaFunc->name(), QString("smooth"));
    QCOMPARE(metaFunc->originalName(), QString("global_function_with_ugly_name"));
    QCOMPARE(metaFunc->minimalSignature(), QString("global_function_with_ugly_name()"));
}

QTEST_APPLESS_MAIN(TestFunctionTag)

#include "testfunctiontag.moc"

