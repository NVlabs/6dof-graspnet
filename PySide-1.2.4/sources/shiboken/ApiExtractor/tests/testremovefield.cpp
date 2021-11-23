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

#include "testremovefield.h"
#include <QtTest/QTest>
#include "testutil.h"

void TestRemoveField::testRemoveField()
{
    const char* cppCode ="\
    struct A {\
        int fieldA;\
        int fieldB;\
    };\
    ";
    const char* xmlCode = "\
    <typesystem package=\"Foo\"> \
        <primitive-type name='int' />\
        <value-type name='A'> \
            <modify-field name='fieldB' remove='all' />\
        </value-type>\
    </typesystem>";
    TestUtil t(cppCode, xmlCode, false);
    AbstractMetaClassList classes = t.builder()->classes();
    AbstractMetaClass* classA = classes.findClass("A");
    QVERIFY(classA);
    QCOMPARE(classA->fields().size(), 1);
    const AbstractMetaField* fieldA = classA->fields().first();
    QVERIFY(fieldA);
    QCOMPARE(fieldA->name(), QString("fieldA"));
}

QTEST_APPLESS_MAIN(TestRemoveField)

#include "testremovefield.moc"

