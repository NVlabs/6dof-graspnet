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

#include "testreverseoperators.h"
#include <QtTest/QTest>
#include "testutil.h"

void TestReverseOperators::testReverseSum()
{
    const char cppCode[] = "struct A {\
            A& operator+(int);\
        };\
        A& operator+(int, const A&);";
    const char xmlCode[] = "\
    <typesystem package=\"Foo\">\
        <primitive-type name='int' />\
        <value-type name='A' />\
    </typesystem>";

    TestUtil t(cppCode, xmlCode, false);
    AbstractMetaClassList classes = t.builder()->classes();
    AbstractMetaClass* classA = classes.findClass("A");
    QVERIFY(classA);
    QCOMPARE(classA->functions().count(), 4);

    const AbstractMetaFunction* reverseOp = 0;
    const AbstractMetaFunction* normalOp = 0;
    foreach(const AbstractMetaFunction* func, classA->functions()) {
        if (func->name() == "operator+") {
            if (func->isReverseOperator())
                reverseOp = func;
            else
                normalOp = func;
        }
    }

    QVERIFY(normalOp);
    QVERIFY(!normalOp->isReverseOperator());
    QCOMPARE(normalOp->arguments().count(), 1);
    QVERIFY(reverseOp);
    QVERIFY(reverseOp->isReverseOperator());
    QCOMPARE(reverseOp->arguments().count(), 1);
}

void TestReverseOperators::testReverseSumWithAmbiguity()
{
    const char cppCode[] = "\
    struct A { A operator+(int); };\
    A operator+(int, const A&);\
    struct B {};\
    B operator+(const A&, const B&);\
    B operator+(const B&, const A&);\
    int operator-(int, const A*);\
    int operator/(const A*, int);\
    ";
    const char xmlCode[] = "\
    <typesystem package=\"Foo\">\
        <primitive-type name='int' />\
        <value-type name='A' />\
        <value-type name='B' />\
    </typesystem>";

    TestUtil t(cppCode, xmlCode, false);
    AbstractMetaClassList classes = t.builder()->classes();
    AbstractMetaClass* classA = classes.findClass("A");
    QVERIFY(classA);
    QCOMPARE(classA->functions().count(), 6);

    AbstractMetaClass* classB = classes.findClass("B");
    QVERIFY(classB);
    QCOMPARE(classB->functions().count(), 4);

    const AbstractMetaFunction* reverseOp = 0;
    const AbstractMetaFunction* normalOp = 0;
    foreach(const AbstractMetaFunction* func, classB->functions()) {
        if (func->name() == "operator+") {
            if (func->isReverseOperator())
                reverseOp = func;
            else
                normalOp = func;
        }
    }
    QVERIFY(normalOp);
    QVERIFY(!normalOp->isReverseOperator());
    QCOMPARE(normalOp->arguments().count(), 1);
    QCOMPARE(normalOp->minimalSignature(), QString("operator+(B,A)"));
    QVERIFY(reverseOp);
    QVERIFY(reverseOp->isReverseOperator());
    QCOMPARE(reverseOp->arguments().count(), 1);
    QCOMPARE(reverseOp->minimalSignature(), QString("operator+(A,B)"));

    reverseOp = classA->findFunction("operator-");
    QVERIFY(reverseOp);
    QCOMPARE(reverseOp->arguments().count(), 1);
    QVERIFY(reverseOp->isPointerOperator());
    QVERIFY(reverseOp->isReverseOperator());

    normalOp = classA->findFunction("operator/");
    QVERIFY(normalOp);
    QCOMPARE(normalOp->arguments().count(), 1);
    QVERIFY(normalOp->isPointerOperator());
    QVERIFY(!normalOp->isReverseOperator());

}



QTEST_APPLESS_MAIN(TestReverseOperators)

#include "testreverseoperators.moc"
