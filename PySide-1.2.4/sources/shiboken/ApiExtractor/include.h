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

#ifndef INCLUDE_H
#define INCLUDE_H

#include <QString>
#include <QList>

class QTextStream;

class Include
{
public:
    enum IncludeType {
        IncludePath,
        LocalPath,
        TargetLangImport
    };

    Include() : m_type(IncludePath) {}
    Include(IncludeType t, const QString &nam) : m_type(t), m_name(nam) {};

    bool isValid() const
    {
        return !m_name.isEmpty();
    }

    IncludeType type() const
    {
        return m_type;
    }

    QString name() const
    {
        return m_name;
    }

    QString toString() const;

    bool operator<(const Include& other) const
    {
        return m_name < other.m_name;
    }

    bool operator==(const Include& other) const
    {
        return m_type == other.m_type && m_name == other.m_name;
    }

    friend uint qHash(const Include&);
    private:
        IncludeType m_type;
        QString m_name;
};

uint qHash(const Include& inc);
QTextStream& operator<<(QTextStream& out, const Include& include);

typedef QList<Include> IncludeList;

#endif
