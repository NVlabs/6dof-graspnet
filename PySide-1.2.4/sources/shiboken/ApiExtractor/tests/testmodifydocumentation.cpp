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

#include "testmodifydocumentation.h"

#include <QCoreApplication>
#include <QtTest/QTest>
#include "testutil.h"
#include <qtdocparser.h>

void TestModifyDocumentation::testModifyDocumentation()
{
    const char* cppCode ="struct B { void b(); }; class A {};";
    const char* xmlCode = "<typesystem package=\"Foo\">\
    <value-type name='B'>\
        <modify-function signature='b()' remove='all' />\
    </value-type>\
    <value-type name='A'>\
    <modify-documentation xpath='description/para[3]'>\
    &lt;para>Some changed contents here&lt;/para>\
    </modify-documentation>\
    </value-type>\
    </typesystem>";
    TestUtil t(cppCode, xmlCode);

    AbstractMetaClass* classA = t.builder()->classes().findClass("A");
    QVERIFY(classA);
    DocModificationList docMods = classA->typeEntry()->docModifications();
    QCOMPARE(docMods.count(), 1);
    QCOMPARE(docMods[0].code().trimmed(), QString("<para>Some changed contents here</para>"));
    QCOMPARE(docMods[0].signature(), QString(""));
    QtDocParser docParser;
    docParser.setDocumentationDataDirectory(QDir::currentPath());
    docParser.fillDocumentation(classA);

    QVERIFY(!classA->documentation().value().trimmed().isEmpty());
    QCOMPARE(classA->documentation().value(), QString("<?xml version=\"1.0\"?>\n\
<description>oi\n\
                <para>Paragraph number 1</para>\n\
    <para>Paragraph number 2</para>\n\
    <para>Some changed contents here</para>\n\
</description>\n"));
}

// We expand QTEST_MAIN macro but using QCoreApplication instead of QApplication
// because this test needs an event loop but can't use QApplication to avoid a crash
// on our ARMEL/FRAMANTLE buildbot
int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    TestModifyDocumentation tc;
    return QTest::qExec(&tc, argc, argv);
}

#include "testmodifydocumentation.moc"
