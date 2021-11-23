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

#include "testconversionruletag.h"
#include <QtTest/QTest>
#include "testutil.h"
#include <QFile>
#include <QTemporaryFile>

void TestConversionRuleTag::testConversionRuleTagWithFile()
{
    // temp file used later
    const char conversionData[] = "Hi! I'm a conversion rule.";
    QTemporaryFile file;
    file.open();
    QCOMPARE(file.write(conversionData), qint64(sizeof(conversionData)-1));
    file.close();

    const char cppCode[] = "struct A {};";
    QString xmlCode = "\
    <typesystem package='Foo'>\
        <value-type name='A'>\
            <conversion-rule file='"+ file.fileName() +"' />\
        </value-type>\
    </typesystem>";
    TestUtil t(cppCode, xmlCode.toLocal8Bit().data());
    AbstractMetaClassList classes = t.builder()->classes();
    AbstractMetaClass* classA = classes.findClass("A");
    QVERIFY(classA);
    const ComplexTypeEntry* typeEntry = classA->typeEntry();
    QVERIFY(typeEntry->hasConversionRule());
    QCOMPARE(typeEntry->conversionRule(), QString(conversionData));
}

void TestConversionRuleTag::testConversionRuleTagReplace()
{
    const char cppCode[] = "\
    struct A {\
        A();\
        A(const char*, int);\
    };\
    struct B {\
        A createA();\
    };\
    ";
    const char* xmlCode = "\
    <typesystem package='Foo'>\
        <primitive-type name='int'/>\
        <primitive-type name='char'/>\
        <primitive-type name='A'>\
            <conversion-rule>\
                <native-to-target>\
                DoThis();\
                return ConvertFromCppToPython(%IN);\
                </native-to-target>\
                <target-to-native>\
                    <add-conversion type='TargetNone' check='%IN == Target_None'>\
                    DoThat();\
                    DoSomething();\
                    %OUT = A();\
                    </add-conversion>\
                    <add-conversion type='B' check='CheckIfInputObjectIsB(%IN)'>\
                    %OUT = %IN.createA();\
                    </add-conversion>\
                    <add-conversion type='String' check='String_Check(%IN)'>\
                    %OUT = new A(String_AsString(%IN), String_GetSize(%IN));\
                    </add-conversion>\
                </target-to-native>\
            </conversion-rule>\
        </primitive-type>\
        <value-type name='B'/>\
    </typesystem>";

    TestUtil t(cppCode, xmlCode);
    TypeDatabase* typeDb = TypeDatabase::instance();
    PrimitiveTypeEntry* typeA = typeDb->findPrimitiveType("A");
    QVERIFY(typeA);

    CustomConversion* conversion = typeA->customConversion();
    QVERIFY(conversion);

    QCOMPARE(typeA, conversion->ownerType());
    QCOMPARE(conversion->nativeToTargetConversion().trimmed(), QString("DoThis();                return ConvertFromCppToPython(%IN);"));

    QVERIFY(conversion->replaceOriginalTargetToNativeConversions());
    QVERIFY(conversion->hasTargetToNativeConversions());
    QCOMPARE(conversion->targetToNativeConversions().size(), 3);

    CustomConversion::TargetToNativeConversion* toNative = conversion->targetToNativeConversions().at(0);
    QVERIFY(toNative);
    QCOMPARE(toNative->sourceTypeName(), QString("TargetNone"));
    QVERIFY(toNative->isCustomType());
    QCOMPARE(toNative->sourceType(), (const TypeEntry*)0);
    QCOMPARE(toNative->sourceTypeCheck(), QString("%IN == Target_None"));
    QCOMPARE(toNative->conversion().trimmed(), QString("DoThat();                    DoSomething();                    %OUT = A();"));

    toNative = conversion->targetToNativeConversions().at(1);
    QVERIFY(toNative);
    QCOMPARE(toNative->sourceTypeName(), QString("B"));
    QVERIFY(!toNative->isCustomType());
    TypeEntry* typeB = typeDb->findType("B");
    QVERIFY(typeB);
    QCOMPARE(toNative->sourceType(), typeB);
    QCOMPARE(toNative->sourceTypeCheck(), QString("CheckIfInputObjectIsB(%IN)"));
    QCOMPARE(toNative->conversion().trimmed(), QString("%OUT = %IN.createA();"));

    toNative = conversion->targetToNativeConversions().at(2);
    QVERIFY(toNative);
    QCOMPARE(toNative->sourceTypeName(), QString("String"));
    QVERIFY(toNative->isCustomType());
    QCOMPARE(toNative->sourceType(), (const TypeEntry*)0);
    QCOMPARE(toNative->sourceTypeCheck(), QString("String_Check(%IN)"));
    QCOMPARE(toNative->conversion().trimmed(), QString("%OUT = new A(String_AsString(%IN), String_GetSize(%IN));"));
}

void TestConversionRuleTag::testConversionRuleTagAdd()
{
    const char cppCode[] = "\
    struct Date {\
        Date();\
        Date(int, int, int);\
    };\
    ";
    const char* xmlCode = "\
    <typesystem package='Foo'>\
        <primitive-type name='int'/>\
        <value-type name='Date'>\
            <conversion-rule>\
                <target-to-native replace='no'>\
                    <add-conversion type='TargetDate' check='TargetDate_Check(%IN)'>\
                    if (!TargetDateTimeAPI) TargetDateTime_IMPORT;\
                    %OUT = new Date(TargetDate_Day(%IN), TargetDate_Month(%IN), TargetDate_Year(%IN));\
                    </add-conversion>\
                </target-to-native>\
            </conversion-rule>\
        </value-type>\
    </typesystem>";

    TestUtil t(cppCode, xmlCode);
    AbstractMetaClass* classA = t.builder()->classes().findClass("Date");
    QVERIFY(classA);

    CustomConversion* conversion = classA->typeEntry()->customConversion();
    QVERIFY(conversion);

    QCOMPARE(conversion->nativeToTargetConversion(), QString());

    QVERIFY(!conversion->replaceOriginalTargetToNativeConversions());
    QVERIFY(conversion->hasTargetToNativeConversions());
    QCOMPARE(conversion->targetToNativeConversions().size(), 1);

    CustomConversion::TargetToNativeConversion* toNative = conversion->targetToNativeConversions().first();
    QVERIFY(toNative);
    QCOMPARE(toNative->sourceTypeName(), QString("TargetDate"));
    QVERIFY(toNative->isCustomType());
    QCOMPARE(toNative->sourceType(), (const TypeEntry*)0);
    QCOMPARE(toNative->sourceTypeCheck(), QString("TargetDate_Check(%IN)"));
    QCOMPARE(toNative->conversion().trimmed(), QString("if (!TargetDateTimeAPI) TargetDateTime_IMPORT;                    %OUT = new Date(TargetDate_Day(%IN), TargetDate_Month(%IN), TargetDate_Year(%IN));"));
}

void TestConversionRuleTag::testConversionRuleTagWithInsertTemplate()
{
    const char cppCode[] = "struct A {};";
    const char* xmlCode = "\
    <typesystem package='Foo'>\
        <primitive-type name='int'/>\
        <template name='native_to_target'>\
        return ConvertFromCppToPython(%IN);\
        </template>\
        <template name='target_to_native'>\
        %OUT = %IN.createA();\
        </template>\
        <primitive-type name='A'>\
            <conversion-rule>\
                <native-to-target>\
                    <insert-template name='native_to_target'/>\
                </native-to-target>\
                <target-to-native>\
                    <add-conversion type='TargetType'>\
                        <insert-template name='target_to_native'/>\
                    </add-conversion>\
                </target-to-native>\
            </conversion-rule>\
        </primitive-type>\
    </typesystem>";

    TestUtil t(cppCode, xmlCode);
    TypeDatabase* typeDb = TypeDatabase::instance();
    PrimitiveTypeEntry* typeA = typeDb->findPrimitiveType("A");
    QVERIFY(typeA);

    CustomConversion* conversion = typeA->customConversion();
    QVERIFY(conversion);

    QCOMPARE(typeA, conversion->ownerType());
    QCOMPARE(conversion->nativeToTargetConversion().trimmed(),
             QString("// TEMPLATE - native_to_target - START        return ConvertFromCppToPython(%IN);        // TEMPLATE - native_to_target - END"));

    QVERIFY(conversion->hasTargetToNativeConversions());
    QCOMPARE(conversion->targetToNativeConversions().size(), 1);

    CustomConversion::TargetToNativeConversion* toNative = conversion->targetToNativeConversions().first();
    QVERIFY(toNative);
    QCOMPARE(toNative->conversion().trimmed(),
             QString("// TEMPLATE - target_to_native - START        %OUT = %IN.createA();        // TEMPLATE - target_to_native - END"));
}

QTEST_APPLESS_MAIN(TestConversionRuleTag)

#include "testconversionruletag.moc"
