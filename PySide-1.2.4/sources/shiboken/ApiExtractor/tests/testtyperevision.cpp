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
*/

#include "testtyperevision.h"
#include <QtTest/QTest>
#include "testutil.h"


void TestTypeRevision::testRevisionAttr()
{
    const char* cppCode = "class Rev_0 {};"
                          "class Rev_1 {};"
                          "class Rev_2 { public: enum Rev_3 { X }; enum Rev_5 { Y }; };";
    const char* xmlCode = "<typesystem package=\"Foo\">"
                        "<value-type name=\"Rev_0\"/>"
                        "<value-type name=\"Rev_1\" revision=\"1\"/>"
                        "<object-type name=\"Rev_2\" revision=\"2\">"
                        "    <enum-type name=\"Rev_3\" revision=\"3\" flags=\"Flag_4\" flags-revision=\"4\" />"
                        "    <enum-type name=\"Rev_5\" revision=\"5\" flags=\"Flag_5\" />"
                        "</object-type>"
                        "</typesystem>";
    TestUtil t(cppCode, xmlCode);
    AbstractMetaClassList classes = t.builder()->classes();
    AbstractMetaClass* rev0 = classes.findClass("Rev_0");
    QCOMPARE(getTypeRevision(rev0->typeEntry()), 0);

    AbstractMetaClass* rev1 = classes.findClass("Rev_1");
    QCOMPARE(getTypeRevision(rev1->typeEntry()), 1);

    AbstractMetaClass* rev2 = classes.findClass("Rev_2");
    QCOMPARE(getTypeRevision(rev2->typeEntry()), 2);

    AbstractMetaEnum* rev3 = rev2->findEnum("Rev_3");
    QCOMPARE(getTypeRevision(rev3->typeEntry()), 3);
    FlagsTypeEntry* rev4 = rev3->typeEntry()->flags();
    QCOMPARE(getTypeRevision(rev4), 4);
    AbstractMetaEnum* rev5 = rev2->findEnum("Rev_5");
    QCOMPARE(getTypeRevision(rev5->typeEntry()), 5);
    QCOMPARE(getTypeRevision(rev5->typeEntry()->flags()), 5);
}

QTEST_APPLESS_MAIN(TestTypeRevision)

#include "testtyperevision.moc"

