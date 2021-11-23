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

#include "testcodeinjection.h"
#include <QFileInfo>
#include <QDir>
#include <QtTest/QTest>
#include "testutil.h"

void TestCodeInjections::testReadFileUtf8()
{
    const char* cppCode ="struct A {};";
    int argc = 0;
    char *argv[] = {NULL};
    QCoreApplication app(argc, argv);
    QString filePath = QDir::currentPath();
    QString xmlCode = "\
    <typesystem package=\"Foo\"> \
        <value-type name='A'> \
            <conversion-rule file='"+filePath+"/utf8code.txt'/>\
            <inject-code class='target' file='"+filePath+"/utf8code.txt' />\
        </value-type>\
        <value-type name='A::B'/> \
    </typesystem>";
    TestUtil t(cppCode, xmlCode.toLocal8Bit().constData());
    AbstractMetaClassList classes = t.builder()->classes();
    AbstractMetaClass* classA = classes.findClass("A");
    QCOMPARE(classA->typeEntry()->codeSnips().count(), 1);
    QString code = classA->typeEntry()->codeSnips().first().code();
    QString utf8Data = QString::fromUtf8("\xC3\xA1\xC3\xA9\xC3\xAD\xC3\xB3\xC3\xBA");
    QVERIFY(code.indexOf(utf8Data) != -1);
    code = classA->typeEntry()->conversionRule();
    QVERIFY(code.indexOf(utf8Data) != -1);
}

void TestCodeInjections::testInjectWithValidApiVersion()
{
    const char* cppCode ="struct A {};";
    const char* xmlCode = "\
    <typesystem package='Foo'> \
        <value-type name='A'> \
            <inject-code class='target' since='1.0'>\
                test Inject code\
            </inject-code>\
        </value-type>\
    </typesystem>";

    TestUtil t(cppCode, xmlCode, true, "1.0");

    AbstractMetaClassList classes = t.builder()->classes();
    AbstractMetaClass* classA = classes.findClass("A");
    QCOMPARE(classA->typeEntry()->codeSnips().count(), 1);
}

void TestCodeInjections::testInjectWithInvalidApiVersion()
{
    const char* cppCode ="struct A {};";
    const char* xmlCode  = "\
    <typesystem package=\"Foo\"> \
        <value-type name='A'> \
            <inject-code class='target' since='1.0'>\
                test Inject code\
            </inject-code>\
        </value-type>\
    </typesystem>";

    TestUtil t(cppCode, xmlCode, true, "0.1");

    AbstractMetaClassList classes = t.builder()->classes();
    AbstractMetaClass* classA = classes.findClass("A");
    QCOMPARE(classA->typeEntry()->codeSnips().count(), 0);
}



QTEST_APPLESS_MAIN(TestCodeInjections)

#include "testcodeinjection.moc"
