/*
 * This file is part of the PySide Tools project.
 *
 * Copyright (C) 1992-2005 Trolltech AS. All rights reserved.
 * Copyright (C) 2009-2011 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: PySide team <pyside@openbossa.org>
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


#include <qstring.h>
#include <qstringlist.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qlocale.h>
#include <qdatetime.h>
#include <qtextstream.h>
#include <qbytearray.h>
#include <qhash.h>
#include <qdir.h>
#include <qstack.h>
#include <qdom.h>
#include "rcc.h"

static bool qt_rcc_write_number(FILE *out, quint32 number, int width)
{
    int dividend = 1;
    switch (width) {
    case 2:
        dividend = 256;
        break;
    case 3:
        dividend = 65536;
        break;
    case 4:
        dividend = 16777216;
        break;
    default:
        break;
    }

    // Write <width> bytes
    while (dividend >= 1) {
        const quint8 tmp = number / dividend;

        if (tmp >= 32 && tmp < 127 && tmp != '"' && tmp != '\\') {
            /* Optimization for printable characters */
            fprintf(out, "%c", tmp);
        } else {
            fprintf(out, "\\x%02x", tmp);
        }

        number -= tmp * dividend;
        dividend /= 256;
    }
    return true;
}

bool RCCFileInfo::writeDataInfo(FILE *out)
{
    //pointer data
    if(flags & RCCFileInfo::Directory) {
        //name offset
        qt_rcc_write_number(out, nameOffset, 4);

        //flags
        qt_rcc_write_number(out, flags, 2);

        //child count
        qt_rcc_write_number(out, children.size(), 4);

        //first child offset
        qt_rcc_write_number(out, childOffset, 4);
    } else {
        //name offset
        qt_rcc_write_number(out, nameOffset, 4);

        //flags
        qt_rcc_write_number(out, flags, 2);

        //locale
        qt_rcc_write_number(out, locale.country(), 2);
        qt_rcc_write_number(out, locale.language(), 2);

        //data offset
        qt_rcc_write_number(out, dataOffset, 4);
    }
    return true;
}

qint64 RCCFileInfo::writeDataBlob(FILE *out, qint64 offset)
{
    //capture the offset
    dataOffset = offset;

    //find the data to be written
    QFile file(fileInfo.absoluteFilePath());
    if (!file.open(QFile::ReadOnly)) {
        fprintf(stderr, "Couldn't open %s\n", fileInfo.absoluteFilePath().toLatin1().constData());
        return false;
    }
    QByteArray data = file.readAll();

#ifndef QT_NO_COMPRESS
    // Check if compression is useful for this file
    if (mCompressLevel != 0 && data.size() != 0) {
        QByteArray compressed = qCompress(reinterpret_cast<uchar *>(data.data()), data.size(), mCompressLevel);

        int compressRatio = int(100.0f * (float(data.size() - compressed.size()) / float(data.size())));
        if (compressRatio >= mCompressThreshold) {
            data = compressed;
            flags |= Compressed;
        }
    }
#endif // QT_NO_COMPRESS

    //write the length
    qt_rcc_write_number(out, data.size(), 4);
    offset += 4;

    //write the payload
    for (int i=0; i<data.size(); i++) {
        qt_rcc_write_number(out, data.at(i), 1);
    }
    offset += data.size();

    //done
    return offset;
}

qint64 RCCFileInfo::writeDataName(FILE *out, qint64 offset)
{
    //capture the offset
    nameOffset = offset;

    //write the length
    qt_rcc_write_number(out, name.length(), 2);
    offset += 2;

    //write the hash
    qt_rcc_write_number(out, qHash(name), 4);
    offset += 4;

    //write the name
    const QChar *unicode = name.unicode();
    for (int i=0; i<name.length(); i++) {
        qt_rcc_write_number(out, unicode[i].unicode(), 2);
    }
    offset += name.length()*2;

    //done
    return offset;
}

RCCResourceLibrary::~RCCResourceLibrary()
{
    delete root;
}

bool RCCResourceLibrary::interpretResourceFile(QIODevice *inputDevice, QString fname, QString currentPath)
{
    if (!currentPath.isEmpty() && !currentPath.endsWith(QLatin1String("/")))
        currentPath += '/';

    QDomDocument document;
    {
        QString errorMsg;
        int errorLine, errorColumn;
        if(!document.setContent(inputDevice, &errorMsg, &errorLine, &errorColumn)) {
            fprintf(stderr, "pyrcc4 Parse Error:%s:%d:%d [%s]\n", fname.toLatin1().constData(),
                    errorLine, errorColumn, errorMsg.toLatin1().constData());
            return false;
        }
    }
    for(QDomElement root = document.firstChild().toElement(); !root.isNull();
        root = root.nextSibling().toElement()) {
        if (root.tagName() != QLatin1String(TAG_RCC))
            continue;

        for (QDomElement child = root.firstChild().toElement(); !child.isNull();
             child = child.nextSibling().toElement()) {
            if (child.tagName() == QLatin1String(TAG_RESOURCE)) {
                QLocale lang = QLocale::c();
                if (child.hasAttribute(ATTRIBUTE_LANG))
                    lang = QLocale(child.attribute(ATTRIBUTE_LANG));

                QString prefix;
                if (child.hasAttribute(ATTRIBUTE_PREFIX))
                    prefix = child.attribute(ATTRIBUTE_PREFIX);
                if (!prefix.startsWith(QLatin1String("/")))
                    prefix.prepend('/');
                if (!prefix.endsWith(QLatin1String("/")))
                    prefix += '/';


                for (QDomNode res = child.firstChild(); !res.isNull(); res = res.nextSibling()) {
                    if (res.toElement().tagName() == QLatin1String(TAG_FILE)) {

                        QString fileName(res.firstChild().toText().data());
                        if (fileName.isEmpty())
                            fprintf(stderr, "Warning: Null node in XML\n");

                        QString alias;
                        if (res.toElement().hasAttribute(ATTRIBUTE_ALIAS))
                            alias = res.toElement().attribute(ATTRIBUTE_ALIAS);
                        else
                            alias = fileName;

                        int compressLevel = mCompressLevel;
                        if (res.toElement().hasAttribute(ATTRIBUTE_COMPRESS))
                            compressLevel = res.toElement().attribute(ATTRIBUTE_COMPRESS).toInt();
                        int compressThreshold = mCompressThreshold;
                        if (res.toElement().hasAttribute(ATTRIBUTE_THRESHOLD))
                            compressThreshold = res.toElement().attribute(ATTRIBUTE_THRESHOLD).toInt();

                        // Special case for -no-compress. Overrides all other settings.
                        if (mCompressLevel == -2)
                            compressLevel = 0;

                        alias = QDir::cleanPath(alias);
                        while (alias.startsWith("../"))
                            alias.remove(0, 3);
                        alias = prefix + alias;

                        QFileInfo file(currentPath + fileName);
                        if (!file.exists()) {
                            fprintf(stderr, "Cannot find file: %s\n", fileName.toLatin1().constData());
                            continue ;
                        } else if (file.isFile()) {
                            addFile(alias, RCCFileInfo(alias.section('/', -1), file, lang,
                                                       RCCFileInfo::NoFlags, compressLevel, compressThreshold));
                        } else {
                            QDir dir;
                            if(file.isDir()) {
                                dir.setPath(file.filePath());
                            } else {
                                dir.setPath(file.path());
                                dir.setNameFilters(QStringList(file.fileName()));
                                if(alias.endsWith(file.fileName()))
                                    alias = alias.left(alias.length()-file.fileName().length());
                            }
                            if (!alias.endsWith(QLatin1String("/")))
                                alias += '/';
                            QFileInfoList children = dir.entryInfoList();
                            for(int i = 0; i < children.size(); ++i) {
                                if(children[i].fileName() != QLatin1String(".") &&
                                   children[i].fileName() != QLatin1String(".."))
                                    addFile(alias + children[i].fileName(),
                                            RCCFileInfo(children[i].fileName(), children[i], lang,
                                                        RCCFileInfo::NoFlags, compressLevel, compressThreshold));
                            }
                        }
                    }
                }
            }
        }
    }
    if(this->root == 0) {
        fprintf(stderr, "No resources in resource description.\n");
        return false;
    }
    return true;
}

bool RCCResourceLibrary::addFile(const QString &alias, const RCCFileInfo &file)
{
    if (file.fileInfo.size() > 0xffffffff) {
        fprintf(stderr, "File too big: %s",
                file.fileInfo.absoluteFilePath().toLatin1().constData());
        return false;
    }
    if(!root)
        root = new RCCFileInfo("", QFileInfo(), QLocale(), RCCFileInfo::Directory);

    RCCFileInfo *parent = root;
    const QStringList nodes = alias.split('/');
    for(int i = 1; i < nodes.size()-1; ++i) {
        const QString node = nodes.at(i);
        if(!parent->children.contains(node)) {
            RCCFileInfo *s = new RCCFileInfo(node, QFileInfo(), QLocale(), RCCFileInfo::Directory);
            s->parent = parent;
            parent->children.insert(node, s);
            parent = s;
        } else {
            parent = parent->children[node];
        }
    }

    const QString filename = nodes.at(nodes.size()-1);
    RCCFileInfo *s = new RCCFileInfo(file);
    s->parent = parent;
    parent->children.insertMulti(filename, s);
    return true;
}

bool RCCResourceLibrary::readFiles()
{
    //read in data
    if (mVerbose)
        fprintf(stderr, "Processing %d files\n", mFileNames.size());
    for (int i=0; i<mFileNames.size(); ++i) {
        QFile fileIn;
        QString fname = mFileNames.at(i), pwd;
        if(fname == "-") {
            fname = "(stdin)";
            pwd = QDir::currentPath();
            fileIn.setFileName(fname);
            if (!fileIn.open(stdin, QIODevice::ReadOnly)) {
                fprintf(stderr, "Unable to open file: %s\n", fname.toLatin1().constData());
                return false;
            }
        } else {
            pwd = QFileInfo(fname).path();
            fileIn.setFileName(fname);
            if (!fileIn.open(QIODevice::ReadOnly)) {
                fprintf(stderr, "Unable to open file: %s\n", fname.toLatin1().constData());
                return false;
            }
        }
        if (mVerbose)
            fprintf(stderr, "Interpreting %s\n", fname.toLatin1().constData());

        if (!interpretResourceFile(&fileIn, fname, pwd))
            return false;
    }
    return true;
}

QStringList
RCCResourceLibrary::dataFiles() const
{
    QStringList ret;
    QStack<RCCFileInfo*> pending;

    if (!root)
        return ret;

    pending.push(root);
    while(!pending.isEmpty()) {
        RCCFileInfo *file = pending.pop();
        for(QHash<QString, RCCFileInfo*>::iterator it = file->children.begin();
            it != file->children.end(); ++it) {
            RCCFileInfo *child = it.value();
            if(child->flags & RCCFileInfo::Directory)
                pending.push(child);
            ret.append(child->fileInfo.filePath());
        }
    }
    return ret;
}

bool RCCResourceLibrary::output(FILE *out)
{
    //write out
    if (mVerbose)
        fprintf(stderr, "Outputting code\n");
    if (!writeHeader(out)) {
        fprintf(stderr, "Couldn't write header\n");
        return false;
    }
    if (!writeDataBlobs(out)) {
        fprintf(stderr, "Couldn't write data blob\n");
        return false;
    }
    if (!writeDataNames(out)) {
        fprintf(stderr, "Couldn't write file names\n");
        return false;
    }
    if (!writeDataStructure(out)) {
        fprintf(stderr, "Couldn't write data tree\n");
        return false;
    }
    if (!writeInitializer(out)) {
        fprintf(stderr, "Couldn't write footer\n");
        return false;
    }
    return true;
}

bool
RCCResourceLibrary::writeHeader(FILE *out)
{
    fprintf(out, "# -*- coding: utf-8 -*-\n\n");
    fprintf(out, "# Resource object code\n");
    fprintf(out, "#\n");
    fprintf(out, "# Created: %s\n", QDateTime::currentDateTime().toString().toUtf8().constData());
    fprintf(out, "#      by: The Resource Compiler for PySide (Qt v%s)\n", QT_VERSION_STR);
    fprintf(out, "#\n");
    fprintf(out, "# WARNING! All changes made in this file will be lost!\n");
    fprintf(out, "\n");
    fprintf(out, "from PySide import QtCore\n\n");

    return true;
}

bool
RCCResourceLibrary::writeDataBlobs(FILE *out)
{
    fprintf(out, "qt_resource_data = %s\"", mPrefix);
    QStack<RCCFileInfo*> pending;

    if (!root)
        return false;

    pending.push(root);
    qint64 offset = 0;
    while(!pending.isEmpty()) {
        RCCFileInfo *file = pending.pop();
        for(QHash<QString, RCCFileInfo*>::iterator it = file->children.begin();
            it != file->children.end(); ++it) {
            RCCFileInfo *child = it.value();
            if(child->flags & RCCFileInfo::Directory)
                pending.push(child);
            else
                offset = child->writeDataBlob(out, offset);
        }
    }
    fprintf(out, "\"\n");
    return true;
}

bool
RCCResourceLibrary::writeDataNames(FILE *out)
{
    fprintf(out, "qt_resource_name = %s\"", mPrefix);

    QHash<QString, int> names;
    QStack<RCCFileInfo*> pending;

    if (!root)
        return false;

    pending.push(root);
    qint64 offset = 0;
    while(!pending.isEmpty()) {
        RCCFileInfo *file = pending.pop();
        for(QHash<QString, RCCFileInfo*>::iterator it = file->children.begin();
            it != file->children.end(); ++it) {
            RCCFileInfo *child = it.value();
            if(child->flags & RCCFileInfo::Directory)
                pending.push(child);
            if(names.contains(child->name)) {
                child->nameOffset = names.value(child->name);
            } else {
                names.insert(child->name, offset);
                offset = child->writeDataName(out, offset);
            }
        }
    }
    fprintf(out, "\"\n");
    return true;
}

static bool qt_rcc_compare_hash(const RCCFileInfo *left, const RCCFileInfo *right)
{
    return qHash(left->name) < qHash(right->name);
}

bool
RCCResourceLibrary::writeDataStructure(FILE *out)
{
    fprintf(out, "qt_resource_struct = %s\"", mPrefix);
    QStack<RCCFileInfo*> pending;

    if (!root)
        return false;

    //calculate the child offsets (flat)
    pending.push(root);
    int offset = 1;
    while(!pending.isEmpty()) {
        RCCFileInfo *file = pending.pop();
        file->childOffset = offset;

        //sort by hash value for binary lookup
        QList<RCCFileInfo*> children = file->children.values();
        qSort(children.begin(), children.end(), qt_rcc_compare_hash);

        //write out the actual data now
        for(int i = 0; i < children.size(); ++i) {
            RCCFileInfo *child = children.at(i);
            ++offset;
            if(child->flags & RCCFileInfo::Directory)
                pending.push(child);
        }
    }

    //write out the structure (ie iterate again!)
    pending.push(root);
    root->writeDataInfo(out);
    while(!pending.isEmpty()) {
        RCCFileInfo *file = pending.pop();

        //sort by hash value for binary lookup
        QList<RCCFileInfo*> children = file->children.values();
        qSort(children.begin(), children.end(), qt_rcc_compare_hash);

        //write out the actual data now
        for(int i = 0; i < children.size(); ++i) {
            RCCFileInfo *child = children.at(i);
            child->writeDataInfo(out);
            if(child->flags & RCCFileInfo::Directory)
                pending.push(child);
        }
    }
    fprintf(out, "\"\n");

    return true;
}

bool
RCCResourceLibrary::writeInitializer(FILE *out)
{
    fprintf(out, "def qInitResources():\n");
    fprintf(out, "    QtCore.qRegisterResourceData(0x01, qt_resource_struct, qt_resource_name, qt_resource_data)\n");

    fprintf(out, "\n");

    fprintf(out, "def qCleanupResources():\n");
    fprintf(out, "    QtCore.qUnregisterResourceData(0x01, qt_resource_struct, qt_resource_name, qt_resource_data)\n");

    fprintf(out, "\n");

    fprintf(out, "qInitResources()\n");

    return true;
}
