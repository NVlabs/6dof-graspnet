/*
* This file is part of the Boost Python Generator project.
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

#include "sphinxtabletest.h"
#include "qtdocgenerator.h"
#include <QtTest/QTest>
#include <QDebug>

QString SphinxTableTest::transformXml(const char* xml)
{
    return QtXmlToSphinx(m_generator, xml).result();
}

void SphinxTableTest::setUp()
{
    m_generator = new QtDocGenerator;
}

void SphinxTableTest::tearDown()
{
    delete m_generator;
}

void SphinxTableTest::testEmptyString()
{
    const char* xml = "";
    QCOMPARE(transformXml(xml), QString());
}

void SphinxTableTest::testSimpleTable()
{
    const char* xml = "\
<table>\
    <header>\
        <item>\
            <para>Header 1</para>\
        </item>\
        <item>\
            <para>Header 2</para>\
        </item>\
    </header>\
    <row>\
        <item>\
            <para>1 1</para>\
        </item>\
        <item>\
            <para>1 2</para>\
        </item>\
    </row>\
    <row>\
        <item>\
            <para>2 1</para>\
        </item>\
        <item>\
            <para>2 2</para>\
        </item>\
    </row>\
</table>";
    QCOMPARE(transformXml(xml), QString("\
    +--------+--------+\n\
    |Header 1|Header 2|\n\
    +--------+--------+\n\
    |1 1     |1 2     |\n\
    +--------+--------+\n\
    |2 1     |2 2     |\n\
    +--------+--------+\n\
\n"));
}

void SphinxTableTest::testColSpan()
{
    const char* xml = "\
<table>\
    <header>\
        <item>\
            <para>Header 1</para>\
        </item>\
        <item>\
            <para>Header 2</para>\
        </item>\
    </header>\
    <row>\
        <item colspan=\"2\">\
            <para>I'm a big text!</para>\
        </item>\
    </row>\
    <row>\
        <item>\
            <para>2 1</para>\
        </item>\
        <item>\
            <para>2 2</para>\
        </item>\
    </row>\
</table>";
    QCOMPARE(transformXml(xml), QString("\
    +---------------+--------+\n\
    |Header 1       |Header 2|\n\
    +---------------+--------+\n\
    |I'm a big text!         |\n\
    +---------------+--------+\n\
    |2 1            |2 2     |\n\
    +---------------+--------+\n\
\n"));
}


void SphinxTableTest::testRowSpan()
{
    const char* xml = "\
<table>\
    <header>\
        <item>\
            <para>Header 1</para>\
        </item>\
        <item>\
            <para>Header 2</para>\
        </item>\
    </header>\
    <row>\
        <item rowspan=\"2\">\
            <para>1.1</para>\
        </item>\
        <item>\
            <para>1.2</para>\
        </item>\
    </row>\
    <row>\
        <item>\
            <para>2 2</para>\
        </item>\
    </row>\
</table>";
    QCOMPARE(transformXml(xml), QString("\
    +--------+--------+\n\
    |Header 1|Header 2|\n\
    +--------+--------+\n\
    |1.1     |1.2     |\n\
    +        +--------+\n\
    |        |2 2     |\n\
    +--------+--------+\n\
\n"));
}


void SphinxTableTest::testComplexTable()
{
    const char* xml = "\
<table>\
    <header>\
        <item>\
            <para>Header 1</para>\
        </item>\
        <item>\
            <para>Header 2</para>\
        </item>\
        <item>\
            <para>Header 3</para>\
        </item>\
    </header>\
    <row>\
        <item rowspan=\"2\">\
            <para>1.1</para>\
        </item>\
        <item colspan=\"2\">\
            <para>1.2</para>\
        </item>\
    </row>\
    <row>\
        <item>\
            <para>2 2</para>\
        </item>\
        <item>\
            <para>2 3</para>\
        </item>\
    </row>\
</table>";
    QCOMPARE(transformXml(xml), QString("\
    +--------+--------+--------+\n\
    |Header 1|Header 2|Header 3|\n\
    +--------+--------+--------+\n\
    |1.1     |1.2              |\n\
    +        +--------+--------+\n\
    |        |2 2     |2 3     |\n\
    +--------+--------+--------+\n\
\n"));
}

void SphinxTableTest::testRowSpan2()
{
    const char* xml = "\
<table>\
    <header>\
        <item><para>h1</para></item>\
        <item><para>h2</para></item>\
        <item><para>h3</para></item>\
        <item><para>h4</para></item>\
    </header>\
    <row>\
        <item rowspan=\"6\"><para>A</para></item>\
        <item rowspan=\"6\"><para>B</para></item>\
        <item><para>C</para></item>\
        <item><para>D</para></item>\
    </row>\
    <row>\
        <item><para>E</para></item>\
        <item><para>F</para></item>\
    </row>\
    <row>\
        <item><para>E</para></item>\
        <item><para>F</para></item>\
    </row>\
    <row>\
        <item><para>E</para></item>\
        <item><para>F</para></item>\
    </row>\
    <row>\
        <item><para>E</para></item>\
        <item><para>F</para></item>\
    </row>\
    <row>\
        <item><para>E</para></item>\
        <item><para>F</para></item>\
    </row>\
</table>";
    QCOMPARE(transformXml(xml), QString("\
    +--+--+--+--+\n\
    |h1|h2|h3|h4|\n\
    +--+--+--+--+\n\
    |A |B |C |D |\n\
    +  +  +--+--+\n\
    |  |  |E |F |\n\
    +  +  +--+--+\n\
    |  |  |E |F |\n\
    +  +  +--+--+\n\
    |  |  |E |F |\n\
    +  +  +--+--+\n\
    |  |  |E |F |\n\
    +  +  +--+--+\n\
    |  |  |E |F |\n\
    +--+--+--+--+\n\
\n"));
}

void SphinxTableTest::testBrokenTable()
{
    const char* xml = "\
<table>\
    <header>\
        <item>\
            <para>Header 1</para>\
        </item>\
        <item>\
            <para>Header 2</para>\
        </item>\
    </header>\
    <row>\
        <item>\
            <para>1.1</para>\
        </item>\
        <item>\
            <para>1.2</para>\
        </item>\
    </row>\
    <row>\
        <item colspan=\"2\">\
            <para>2 2</para>\
        </item>\
        <item>\
            <para>2 3</para>\
        </item>\
        <item>\
            <para>2 4</para>\
        </item>\
        <item>\
            <para>2 5</para>\
        </item>\
    </row>\
    <row>\
        <item>\
            <para>3 1</para>\
        </item>\
        <item>\
            <para>3 2</para>\
        </item>\
        <item>\
            <para>3 3</para>\
        </item>\
    </row>\
</table>";
    QCOMPARE(transformXml(xml), QString("\
    +--------+------------+\n\
    |Header 1|Header 2    |\n\
    +--------+------------+\n\
    |1.1     |1.2         |\n\
    +--------+------------+\n\
    |2 2       2 3 2 4 2 5|\n\
    +--------+------------+\n\
    |3 1     |3 2 3 3     |\n\
    +--------+------------+\n\
\n"));
}


QTEST_APPLESS_MAIN( SphinxTableTest )

#include "sphinxtabletest.moc"
