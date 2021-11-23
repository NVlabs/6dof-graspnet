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

#include "testnumericaltypedef.h"
#include <QtTest/QTest>
#include "testutil.h"

void TestNumericalTypedef::testNumericalTypedef()
{
    const char* cppCode ="\
    typedef double real;\
    void funcDouble(double);\
    void funcReal(real);\
    ";
    const char* xmlCode = "\
    <typesystem package='Foo'> \
        <primitive-type name='double' /> \
        <primitive-type name='real' /> \
        <function signature='funcDouble(double)' />\
        <function signature='funcReal(real)' />\
    </typesystem>";
    TestUtil t(cppCode, xmlCode, false);

    QCOMPARE(t.builder()->globalFunctions().size(), 2);
    const AbstractMetaFunction* funcDouble = t.builder()->globalFunctions().first();
    QVERIFY(funcDouble);
    const AbstractMetaFunction* funcReal = t.builder()->globalFunctions().last();
    QVERIFY(funcReal);

    if (funcDouble->name() == "funcReal")
        std::swap(funcDouble, funcReal);

    QCOMPARE(funcDouble->minimalSignature(), QString("funcDouble(double)"));
    QCOMPARE(funcReal->minimalSignature(), QString("funcReal(real)"));

    const AbstractMetaType* doubleType = funcDouble->arguments().first()->type();
    QVERIFY(doubleType);
    QCOMPARE(doubleType->cppSignature(), QString("double"));
    QVERIFY(doubleType->isPrimitive());
    QVERIFY(doubleType->typeEntry()->isCppPrimitive());

    const AbstractMetaType* realType = funcReal->arguments().first()->type();
    QVERIFY(realType);
    QCOMPARE(realType->cppSignature(), QString("real"));
    QVERIFY(realType->isPrimitive());
    QVERIFY(realType->typeEntry()->isCppPrimitive());
}

void TestNumericalTypedef::testUnsignedNumericalTypedef()
{
    const char* cppCode ="\
    typedef unsigned short ushort;\
    void funcUnsignedShort(unsigned short);\
    void funcUShort(ushort);\
    ";
    const char* xmlCode = "\
    <typesystem package='Foo'> \
        <primitive-type name='short' /> \
        <primitive-type name='unsigned short' /> \
        <primitive-type name='ushort' /> \
        <function signature='funcUnsignedShort(unsigned short)' />\
        <function signature='funcUShort(ushort)' />\
    </typesystem>";
    TestUtil t(cppCode, xmlCode, false);

    QCOMPARE(t.builder()->globalFunctions().size(), 2);
    const AbstractMetaFunction* funcUnsignedShort = t.builder()->globalFunctions().first();
    QVERIFY(funcUnsignedShort);
    const AbstractMetaFunction* funcUShort = t.builder()->globalFunctions().last();
    QVERIFY(funcUShort);

    if (funcUnsignedShort->name() == "funcUShort")
        std::swap(funcUnsignedShort, funcUShort);

    QCOMPARE(funcUnsignedShort->minimalSignature(), QString("funcUnsignedShort(unsigned short)"));
    QCOMPARE(funcUShort->minimalSignature(), QString("funcUShort(ushort)"));

    const AbstractMetaType* unsignedShortType = funcUnsignedShort->arguments().first()->type();
    QVERIFY(unsignedShortType);
    QCOMPARE(unsignedShortType->cppSignature(), QString("unsigned short"));
    QVERIFY(unsignedShortType->isPrimitive());
    QVERIFY(unsignedShortType->typeEntry()->isCppPrimitive());

    const AbstractMetaType* ushortType = funcUShort->arguments().first()->type();
    QVERIFY(ushortType);
    QCOMPARE(ushortType->cppSignature(), QString("ushort"));
    QVERIFY(ushortType->isPrimitive());
    QVERIFY(ushortType->typeEntry()->isCppPrimitive());
}

QTEST_APPLESS_MAIN(TestNumericalTypedef)

#include "testnumericaltypedef.moc"
