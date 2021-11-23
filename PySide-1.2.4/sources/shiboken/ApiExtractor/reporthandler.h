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

#ifndef REPORTHANDLER_H
#define REPORTHANDLER_H

class QString;

class ReportHandler
{
public:
    enum DebugLevel { NoDebug, SparseDebug, MediumDebug, FullDebug };

    static void setContext(const QString &context);

    static DebugLevel debugLevel();
    static void setDebugLevel(DebugLevel level);

    static int warningCount();

    static int suppressedCount();

    static void warning(const QString &str);

    template <typename T>
    static void setProgressReference(T collection)
    {
        setProgressReference(collection.count());
    }

    static void setProgressReference(int max);

    static void progress(const QString &str, ...);

    static void debugSparse(const QString &str)
    {
        debug(SparseDebug, str);
    }
    static void debugMedium(const QString &str)
    {
        debug(MediumDebug, str);
    }
    static void debugFull(const QString &str)
    {
        debug(FullDebug, str);
    }
    static void debug(DebugLevel level, const QString &str);

    static bool isSilent();
    static void setSilent(bool silent);
    static void flush();
};

#endif // REPORTHANDLER_H
