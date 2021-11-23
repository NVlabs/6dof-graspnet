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

#include "fileout.h"
#include "reporthandler.h"

#include <QtCore/QTextCodec>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>

#include <cstdio>

bool FileOut::dummy = false;
bool FileOut::diff = false;

#ifdef Q_OS_LINUX
const char* colorDelete = "\033[31m";
const char* colorAdd = "\033[32m";
const char* colorInfo = "\033[36m";
const char* colorReset = "\033[0m";
#else
const char* colorDelete = "";
const char* colorAdd = "";
const char* colorInfo = "";
const char* colorReset = "";
#endif

FileOut::FileOut(QString n):
        name(n),
        stream(&tmp),
        isDone(false)
{}

static int* lcsLength(QList<QByteArray> a, QList<QByteArray> b)
{
    const int height = a.size() + 1;
    const int width = b.size() + 1;

    int *res = new int[width * height];

    for (int row = 0; row < height; row++)
        res[width * row] = 0;

    for (int col = 0; col < width; col++)
        res[col] = 0;

    for (int row = 1; row < height; row++) {
        for (int col = 1; col < width; col++) {
            if (a[row-1] == b[col-1])
                res[width * row + col] = res[width * (row-1) + col-1] + 1;
            else
                res[width * row + col] = qMax(res[width * row     + col-1],
                                              res[width * (row-1) + col]);
        }
    }
    return res;
}

enum Type {
    Add,
    Delete,
    Unchanged
};

struct Unit
{
    Unit(Type type, int pos) :
            type(type),
            start(pos),
            end(pos) {}

    Type type;
    int start;
    int end;

    void print(QList<QByteArray> a, QList<QByteArray> b)
    {
            if (type == Unchanged) {
                if ((end - start) > 9) {
                    for (int i = start; i <= start + 2; i++)
                        std::printf("  %s\n", a[i].data());
                    std::printf("%s=\n= %d more lines\n=%s\n", colorInfo, end - start - 6, colorReset);
                    for (int i = end - 2; i <= end; i++)
                        std::printf("  %s\n", a[i].data());
                } else {
                    for (int i = start; i <= end; i++)
                        std::printf("  %s\n", a[i].data());
                }
            } else if (type == Add) {
                std::printf("%s", colorAdd);
                for (int i = start; i <= end; i++)
                    std::printf("+ %s\n", b[i].data());
                std::printf("%s", colorReset);
            } else if (type == Delete) {
                std::printf("%s", colorDelete);
                for (int i = start; i <= end; i++)
                    std::printf("- %s\n", a[i].data());
                std::printf("%s", colorReset);
            }
    }
};

static QList<Unit*> *unitAppend(QList<Unit*> *res, Type type, int pos)
{
    if (!res) {
        res = new QList<Unit*>;
        res->append(new Unit(type, pos));
        return res;
    }

    Unit *last = res->last();
    if (last->type == type)
        last->end = pos;
    else
        res->append(new Unit(type, pos));

    return res;
}

static QList<Unit*> *diffHelper(int *lcs, QList<QByteArray> a, QList<QByteArray> b, int row, int col)
{
    if (row > 0 && col > 0 && (a[row-1] == b[col-1])) {
        return unitAppend(diffHelper(lcs, a, b, row - 1, col - 1), Unchanged, row - 1);
    } else {
        int width = b.size() + 1;
        if ((col > 0)
            && (row == 0 || lcs[width * row + col-1] >= lcs[width *(row-1) + col])) {
            return unitAppend(diffHelper(lcs, a, b, row, col - 1), Add, col - 1);
        } else if ((row > 0)
            && (col == 0 || lcs[width * row + col-1] < lcs[width *(row-1) + col])) {
            return unitAppend(diffHelper(lcs, a, b, row - 1, col), Delete, row - 1);
        }
    }
    delete lcs;
    return 0;
}

static void diff(QList<QByteArray> a, QList<QByteArray> b)
{
    QList<Unit*> *res = diffHelper(lcsLength(a, b), a, b, a.size(), b.size());
    for (int i = 0; i < res->size(); i++) {
        Unit *unit = res->at(i);
        unit->print(a, b);
        delete(unit);
    }
    delete(res);
}


bool FileOut::done()
{
    Q_ASSERT(!isDone);
    if (name.isEmpty())
        return false;

    isDone = true;
    bool fileEqual = false;
    QFile fileRead(name);
    QFileInfo info(fileRead);
    stream.flush();
    QByteArray original;
    if (info.exists() && (diff || (info.size() == tmp.size()))) {
        if (!fileRead.open(QIODevice::ReadOnly)) {
            ReportHandler::warning(QString("failed to open file '%1' for reading")
                                   .arg(fileRead.fileName()));
            return false;
        }

        original = fileRead.readAll();
        fileRead.close();
        fileEqual = (original == tmp);
    }

    if (!fileEqual) {
        if (!FileOut::dummy) {
            QDir dir(info.absolutePath());
            if (!dir.mkpath(dir.absolutePath())) {
                ReportHandler::warning(QString("unable to create directory '%1'")
                                       .arg(dir.absolutePath()));
                return false;
            }

            QFile fileWrite(name);
            if (!fileWrite.open(QIODevice::WriteOnly)) {
                ReportHandler::warning(QString("failed to open file '%1' for writing")
                                       .arg(fileWrite.fileName()));
                return false;
            }
            QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
            stream.setDevice(&fileWrite);
            stream << tmp;
        }
        if (diff) {
            std::printf("%sFile: %s%s\n", colorInfo, qPrintable(name), colorReset);
            ::diff(original.split('\n'), tmp.split('\n'));
            std::printf("\n");
        }
        return true;
    }
    return false;
}
