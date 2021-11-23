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

#include "testmultipleinheritance.h"
#include <QtTest/QTest>
#include "testutil.h"

void TestMultipleInheritance::testVirtualClass()
{
    const char* cppCode ="\
    struct A {\
        virtual ~A();\
        virtual void theBug();\
    };\
    struct B {\
        virtual ~B();\
    };\
    struct C : A, B {\
    };\
    struct D : C {\
    };\
    ";
    const char* xmlCode = "\
    <typesystem package=\"Foo\"> \
        <object-type name='A' /> \
        <object-type name='B' /> \
        <object-type name='C' /> \
        <object-type name='D' /> \
    </typesystem>";

    TestUtil t(cppCode, xmlCode);
    AbstractMetaClassList classes = t.builder()->classes();
    QCOMPARE(classes.count(), 4);

    AbstractMetaClass* classD = classes.findClass("D");
    bool functionFound = false;
    foreach (AbstractMetaFunction* f, classD->functions()) {
        if (f->name() == "theBug") {
            functionFound = true;
            break;
        }
    }
    QVERIFY(functionFound);

}

QTEST_APPLESS_MAIN(TestMultipleInheritance)

#include "testmultipleinheritance.moc"
