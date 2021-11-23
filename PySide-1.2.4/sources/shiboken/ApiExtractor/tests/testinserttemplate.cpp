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

#include "testinserttemplate.h"
#include <QtTest/QTest>
#include "testutil.h"

void TestInsertTemplate::testInsertTemplateOnClassInjectCode()
{
    const char* cppCode ="struct A{};";
    const char* xmlCode = "\
    <typesystem package='Foo'>\
        <template name='code_template'>\
        code template content\
        </template>\
        <value-type name='A'>\
            <inject-code class='native'>\
                <insert-template name='code_template'/>\
            </inject-code>\
        </value-type>\
    </typesystem>";
    TestUtil t(cppCode, xmlCode, false);
    AbstractMetaClassList classes = t.builder()->classes();
    QCOMPARE(classes.count(), 1);
    AbstractMetaClass* classA = classes.findClass("A");
    QVERIFY(classA);
    QCOMPARE(classA->typeEntry()->codeSnips().count(), 1);
    QString code = classA->typeEntry()->codeSnips().first().code();
    QVERIFY(code.contains("code template content"));
}

void TestInsertTemplate::testInsertTemplateOnModuleInjectCode()
{
    const char* cppCode ="";
    const char* xmlCode = "\
    <typesystem package='Foo'>\
        <template name='code_template'>\
        code template content\
        </template>\
        <inject-code class='native'>\
            <insert-template name='code_template'/>\
        </inject-code>\
    </typesystem>";
    TestUtil t(cppCode, xmlCode, false);
    AbstractMetaClassList classes = t.builder()->classes();
    QVERIFY(classes.isEmpty());

    TypeEntry* module = TypeDatabase::instance()->findType("Foo");
    QVERIFY(module);
    QCOMPARE(module->codeSnips().count(), 1);
    QString code = module->codeSnips().first().code().trimmed();
    QVERIFY(code.contains("code template content"));
}

void TestInsertTemplate::testInvalidTypeSystemTemplate()
{
    const char* cppCode ="";
    const char* xmlCode = "\
    <typesystem package='Foo'>\
        <inject-code class='native'>\
            <insert-template name='this_code_template_does_not_exists'/>\
        </inject-code>\
    </typesystem>";
    TestUtil t(cppCode, xmlCode, false);
    AbstractMetaClassList classes = t.builder()->classes();
    QVERIFY(classes.isEmpty());

    TypeEntry* module = TypeDatabase::instance()->findType("Foo");
    QVERIFY(module);
    QCOMPARE(module->codeSnips().count(), 1);
    QString code = module->codeSnips().first().code().trimmed();
    QVERIFY(code.isEmpty());
}

void TestInsertTemplate::testValidAndInvalidTypeSystemTemplate()
{
    const char* cppCode ="";
    const char* xmlCode = "\
    <typesystem package='Foo'>\
        <template name='code_template'>\
        code template content\
        </template>\
        <inject-code class='native'>\
            <insert-template name='this_code_template_does_not_exists'/>\
            <insert-template name='code_template'/>\
        </inject-code>\
    </typesystem>";
    TestUtil t(cppCode, xmlCode, false);
    AbstractMetaClassList classes = t.builder()->classes();
    QVERIFY(classes.isEmpty());

    TypeEntry* module = TypeDatabase::instance()->findType("Foo");
    QVERIFY(module);
    QCOMPARE(module->codeSnips().count(), 1);
    QString code = module->codeSnips().first().code().trimmed();
    QVERIFY(code.contains("code template content"));
}

QTEST_APPLESS_MAIN(TestInsertTemplate)

#include "testinserttemplate.moc"
