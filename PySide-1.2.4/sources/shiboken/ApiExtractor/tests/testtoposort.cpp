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

#include "testtoposort.h"
#include <QtTest/QTest>
#include "graph.h"
#include <QDebug>

void TestTopoSort::testTopoSort()
{
    QLinkedList<int> result;
    {
        Graph g(3);
        g.addEdge(1, 2);
        g.addEdge(0, 1);
        result = g.topologicalSort();
        QCOMPARE(result.size(), 3);
        QLinkedList<int>::iterator it = result.begin();
        QCOMPARE(*it, 0);
        QCOMPARE(*(++it), 1);
        QCOMPARE(*(++it), 2);
    }
    {
        Graph g(2);
        result = g.topologicalSort();
        QCOMPARE(result.size(), 2);
        QLinkedList<int>::iterator it = result.begin();
        QCOMPARE(*it, 1);
        QCOMPARE(*(++it), 0);
    }
}

void TestTopoSort::testCiclicGraph()
{
    Graph g(3);
    g.addEdge(0, 1);
    g.addEdge(1, 2);
    g.addEdge(2, 0);
    QLinkedList<int> result = g.topologicalSort();
    QVERIFY(result.isEmpty());
}

QTEST_APPLESS_MAIN(TestTopoSort)

#include "testtoposort.moc"
