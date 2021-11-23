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

#include "graph.h"
#include <QVector>
#include <QDebug>
#include <QLinkedList>
#include <QSet>
#include <iterator>
#include <algorithm>
#include <iostream>
#include <QFile>

struct Graph::GraphPrivate
{
    enum Color { WHITE, GRAY, BLACK };
    typedef QVector<QSet<int> > Edges;
    typedef QSet<int>::const_iterator EdgeIterator;

    Edges edges;

    GraphPrivate(int numNodes) : edges(numNodes)
    {
    }

    void dfsVisit(int node, QLinkedList<int>& result, QVector<Color>& colors) const
    {
        colors[node] = GRAY;
        EdgeIterator it = edges[node].begin();
        for (; it != edges[node].end(); ++it) {
            if (colors[*it] == WHITE)
                dfsVisit(*it, result, colors);
            else if (colors[*it] == GRAY) // This is not a DAG!
                return;
        }
        colors[node] = BLACK;
        result.push_front(node);
    }
};

Graph::Graph(int numNodes) : m_d(new GraphPrivate(numNodes))
{
}

Graph::~Graph()
{
    delete m_d;
}

int Graph::nodeCount() const
{
    return m_d->edges.size();
}

QLinkedList<int> Graph::topologicalSort() const
{
    int nodeCount = Graph::nodeCount();
    QLinkedList<int> result;
    QVector<GraphPrivate::Color> colors(nodeCount, GraphPrivate::WHITE);

    for (int i = 0; i < nodeCount; ++i) {
        if (colors[i] == GraphPrivate::WHITE)
            m_d->dfsVisit(i, result, colors);
    }

    // Not a DAG!
    if (result.size() != nodeCount)
        return QLinkedList<int>();
    return result;
}

bool Graph::containsEdge(int from, int to)
{
    return m_d->edges[from].contains(to);
}

void Graph::addEdge(int from, int to)
{
    Q_ASSERT(to < (int)m_d->edges.size());
    m_d->edges[from].insert(to);
}

void Graph::removeEdge(int from, int to)
{
    m_d->edges[from].remove(to);
}

void Graph::dump() const
{
    for (int i = 0; i < m_d->edges.size(); ++i) {
        std::cout << i << " -> ";
        std::copy(m_d->edges[i].begin(), m_d->edges[i].end(), std::ostream_iterator<int>(std::cout, " "));
        std::cout << std::endl;
    }
}

void Graph::dumpDot(const QHash< int, QString >& nodeNames, const QString& fileName) const
{
    QFile output(fileName);
    if (!output.open(QIODevice::WriteOnly))
        return;
    QTextStream s(&output);
    s << "digraph D {\n";
    for (int i = 0; i < m_d->edges.size(); ++i) {
        GraphPrivate::EdgeIterator it = m_d->edges[i].begin();
        for (;it != m_d->edges[i].end(); ++it)
            s << '"' << nodeNames[i] << "\" -> \"" << nodeNames[*it] << "\"\n";
    }
    s << "}\n";
}
