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

#ifndef GRAPH_H
#define GRAPH_H

#include <QLinkedList>
#include <QHash>
#include <QString>

/// A graph that can have their nodes topologically sorted.
class Graph
{
public:
    /// Create a new graph with \p numNodes nodes.
    Graph(int numNodes);
    ~Graph();

    /// Returns the numbed of nodes in this graph.
    int nodeCount() const;
    /// Returns true if the graph contains the edge from -> to
    bool containsEdge(int from, int to);
    /// Adds an edge to this graph.
    void addEdge(int from, int to);
    /// Removes an edge out of this graph.
    void removeEdge(int from, int to);
    /// Print this graph to stdout.
    void dump() const;
    /**
    *   Dumps a dot graph to a file named \p filename.
    *   \param nodeNames map used to translate node ids to human readable text.
    *   \param fileName file name where the output should be written.
    */
    void dumpDot(const QHash<int, QString>& nodeNames, const QString& fileName) const;

    /**
    *   Topologically sort this graph.
    *   \return A collection with all nodes topologically sorted or an empty collection if a ciclic dependency was found.
    */
    QLinkedList<int> topologicalSort() const;
private:

    struct GraphPrivate;
    GraphPrivate* m_d;
};

#endif
