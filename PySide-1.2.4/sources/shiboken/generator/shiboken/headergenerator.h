/*
 * This file is part of the Shiboken Python Bindings Generator project.
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

#ifndef HEADERGENERATOR_H
#define HEADERGENERATOR_H

#include "shibokengenerator.h"

/**
 *   The HeaderGenerator generate the declarations of C++ bindings classes.
 */
class HeaderGenerator : public ShibokenGenerator
{
public:
    QMap<QString, QString> options() const { return QMap<QString, QString>(); }
protected:
    QString fileNameForClass(const AbstractMetaClass* metaClass) const;
    void generateClass(QTextStream& s, const AbstractMetaClass* metaClass);
    void finishGeneration();

private:
    void writeCopyCtor(QTextStream &s, const AbstractMetaClass* metaClass) const;
    void writeProtectedFieldAccessors(QTextStream& s, const AbstractMetaField* field) const;
    void writeFunction(QTextStream& s, const AbstractMetaFunction* func);
    void writeSbkTypeFunction(QTextStream& s, const AbstractMetaEnum* cppEnum);
    void writeSbkTypeFunction(QTextStream& s, const AbstractMetaClass* cppClass);
    void writeTypeIndexDefineLine(QTextStream& s, const TypeEntry* typeEntry);
    void writeTypeIndexDefine(QTextStream& s, const AbstractMetaClass* metaClass);
    void writeProtectedEnumSurrogate(QTextStream& s, const AbstractMetaEnum* cppEnum);
    void writeInheritedOverloads(QTextStream& s);

    QSet<const AbstractMetaFunction*> m_inheritedOverloads;
};

#endif // HEADERGENERATOR_H

