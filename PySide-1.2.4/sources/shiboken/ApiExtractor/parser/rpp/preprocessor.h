/*
 * This file is part of the API Extractor project.
 *
 * Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
 * Copyright 2005 Harald Fernengel <harry@kdevelop.org>
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

#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include <QtCore/qglobal.h>
#include <QtCore/qstring.h>
#include <QtCore/qstringlist.h>

class QByteArray;
class PreprocessorPrivate;

class Preprocessor
{
public:
    Preprocessor();
    ~Preprocessor();

    void processFile(const QString &fileName);
    void processString(const QByteArray &str);

    void addIncludePaths(const QStringList &includePaths);

    QByteArray result() const;

    QStringList macroNames() const;

    struct MacroItem {
        QString name;
        QStringList parameters;
        QString definition;
        bool isFunctionLike;
#ifdef PP_WITH_MACRO_POSITION
        QString fileName;
#endif
    };
    QList<MacroItem> macros() const;

private:
    Q_DISABLE_COPY(Preprocessor)
    PreprocessorPrivate *d;
};

#endif
