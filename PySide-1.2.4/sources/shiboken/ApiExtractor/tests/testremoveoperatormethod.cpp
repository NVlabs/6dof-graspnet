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

#include "testremoveoperatormethod.h"
#include <QtTest/QTest>
#include "testutil.h"

void TestRemoveOperatorMethod::testRemoveOperatorMethod()
{
    const char* cppCode ="\
    struct A {\
        A& operator>>(char&);\
        A& operator>>(char*);\
        A& operator>>(signed short&);\
        A& operator>>(unsigned short&);\
        A& operator>>(signed int&);\
        A& operator>>(unsigned int&);\
        A& operator>>(signed long&);\
        A& operator>>(unsigned long&);\
        A& operator>>(__int64&);\
        A& operator>>(unsigned __int64&);\
        A& operator>>(float&);\
        A& operator>>(double&);\
        A& operator>>(Char&);\
        A& operator>>(ByteArray&);\
        A& operator>>(String&);\
    };\
    struct Char {};\
    struct ByteArray {};\
    struct String {};\
    ";
    const char* xmlCode = "\
    <typesystem package='Foo'>\
        <primitive-type name='char' />\
        <primitive-type name='signed short' />\
        <primitive-type name='unsigned short' />\
        <primitive-type name='signed int' />\
        <primitive-type name='unsigned int' />\
        <primitive-type name='signed long' />\
        <primitive-type name='unsigned long' />\
        <primitive-type name='__int64' />\
        <primitive-type name='unsigned __int64' />\
        <primitive-type name='float' />\
        <primitive-type name='double' />\
        <primitive-type name='Char' />\
        <primitive-type name='String' />\
        <value-type name='ByteArray' />\
        <object-type name='A'>\
            <modify-function signature='operator&gt;&gt;(char&amp;)' remove='all'/>\
            <modify-function signature='operator&gt;&gt;(char*)' remove='all'/>\
            <modify-function signature='operator&gt;&gt;(signed short&amp;)' remove='all'/>\
            <modify-function signature='operator&gt;&gt;(unsigned short&amp;)' remove='all'/>\
            <modify-function signature='operator&gt;&gt;(signed int&amp;)' remove='all'/>\
            <modify-function signature='operator&gt;&gt;(unsigned int&amp;)' remove='all'/>\
            <modify-function signature='operator&gt;&gt;(signed long&amp;)' remove='all'/>\
            <modify-function signature='operator&gt;&gt;(unsigned long&amp;)' remove='all'/>\
            <modify-function signature='operator&gt;&gt;(__int64&amp;)' remove='all'/>\
            <modify-function signature='operator&gt;&gt;(unsigned __int64&amp;)' remove='all'/>\
            <modify-function signature='operator&gt;&gt;(float&amp;)' remove='all'/>\
            <modify-function signature='operator&gt;&gt;(double&amp;)' remove='all'/>\
            <modify-function signature='operator&gt;&gt;(Char&amp;)' remove='all'/>\
            <modify-function signature='operator&gt;&gt;(String&amp;)' remove='all'/>\
        </object-type>\
    </typesystem>";
    TestUtil t(cppCode, xmlCode, false);
    AbstractMetaClassList classes = t.builder()->classes();
    AbstractMetaClass* classA = classes.findClass("A");
    QVERIFY(classA);
    QCOMPARE(classA->functions().size(), 15);
    QStringList removedSignatures;
    removedSignatures.append("operator>>(char&)");
    removedSignatures.append("operator>>(char*)");
    removedSignatures.append("operator>>(signed short&)");
    removedSignatures.append("operator>>(unsigned short&)");
    removedSignatures.append("operator>>(signed int&)");
    removedSignatures.append("operator>>(unsigned int&)");
    removedSignatures.append("operator>>(signed long&)");
    removedSignatures.append("operator>>(unsigned long&)");
    removedSignatures.append("operator>>(__int64&)");
    removedSignatures.append("operator>>(unsigned __int64&)");
    removedSignatures.append("operator>>(float&)");
    removedSignatures.append("operator>>(double&)");
    removedSignatures.append("operator>>(Char&)");
    removedSignatures.append("operator>>(String&)");
    int notRemoved = classA->functions().size();
    foreach (const AbstractMetaFunction* f, classA->functions()) {
        QCOMPARE(f->isModifiedRemoved(), bool(removedSignatures.contains(f->minimalSignature())));
        notRemoved -= int(f->isModifiedRemoved());
    }
    QCOMPARE(notRemoved, 2);
}

QTEST_APPLESS_MAIN(TestRemoveOperatorMethod)

#include "testremoveoperatormethod.moc"
