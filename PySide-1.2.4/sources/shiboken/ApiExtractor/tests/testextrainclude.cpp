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

#include "testextrainclude.h"
#include <QtTest/QTest>
#include "testutil.h"

void TestExtraInclude::testClassExtraInclude()
{
    const char* cppCode ="struct A {};";
    const char* xmlCode = "\
    <typesystem package='Foo'> \
        <value-type name='A'> \
            <extra-includes>\
                <include file-name='header.h' location='global' />\
            </extra-includes>\
        </value-type>\
    </typesystem>";

    TestUtil t(cppCode, xmlCode, false);
    AbstractMetaClassList classes = t.builder()->classes();
    const AbstractMetaClass* classA = classes.findClass("A");
    QVERIFY(classA);

    QList<Include> includes = classA->typeEntry()->extraIncludes();
    QCOMPARE(includes.count(), 1);
    QCOMPARE(includes.first().name(), QString("header.h"));
}

void TestExtraInclude::testGlobalExtraIncludes()
{
    const char* cppCode ="struct A {};";
    const char* xmlCode = "\
    <typesystem package='Foo'>\
        <extra-includes>\
            <include file-name='header1.h' location='global' />\
            <include file-name='header2.h' location='global' />\
        </extra-includes>\
        <value-type name='A' />\
    </typesystem>";

    TestUtil t(cppCode, xmlCode, false);
    AbstractMetaClassList classes = t.builder()->classes();
    QVERIFY(classes.findClass("A"));

    TypeDatabase* td = TypeDatabase::instance();
    TypeEntry* module = td->findType("Foo");
    QVERIFY(module);

    QList<Include> includes = module->extraIncludes();
    QCOMPARE(includes.count(), 2);
    QCOMPARE(includes.first().name(), QString("header1.h"));
    QCOMPARE(includes.last().name(), QString("header2.h"));
}

QTEST_APPLESS_MAIN(TestExtraInclude)

#include "testextrainclude.moc"

