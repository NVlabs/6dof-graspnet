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

#include "testresolvetype.h"
#include <QtTest/QTest>
#include "testutil.h"

void TestResolveType::testResolveReturnTypeFromParentScope()
{
    const char* cppCode = "\
    namespace A {\
        struct B {\
            struct C {};\
        };\
        struct D : public B::C {\
            C* foo = 0;\
            C* method();\
        };\
    };";
    const char* xmlCode = "\
    <typesystem package='Foo'> \
        <namespace-type name='A' />\
        <value-type name='A::B' /> \
        <value-type name='A::B::C' /> \
        <value-type name='A::D' /> \
    </typesystem>";
    TestUtil t(cppCode, xmlCode, false);
    AbstractMetaClassList classes = t.builder()->classes();
    AbstractMetaClass* classD = classes.findClass("A::D");
    QVERIFY(classD);
    const AbstractMetaFunction* meth = classD->findFunction("method");
    QVERIFY(meth);
}

QTEST_APPLESS_MAIN(TestResolveType)

#include "testresolvetype.moc"

