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

#ifndef SPHINXTABLETEST_H
#define SPHINXTABLETEST_H

#include <QObject>

class QtDocGenerator;
class SphinxTableTest : public QObject {
    Q_OBJECT

private slots:
    void setUp();
    void tearDown();
    void testEmptyString();
    void testSimpleTable();
    void testRowSpan();
    void testColSpan();
    void testComplexTable();
    void testRowSpan2();
    void testBrokenTable();
private:
    QtDocGenerator* m_generator;

    QString transformXml(const char* xml);
};

#endif
