/*
 * This file is part of the PySide Tools project.
 *
 * Copyright (C) 1992-2005 Trolltech AS. All rights reserved.
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef RCC_H
#define RCC_H

#define TAG_RCC "RCC"
#define TAG_RESOURCE "qresource"
#define TAG_FILE "file"

#define ATTRIBUTE_LANG "lang"
#define ATTRIBUTE_PREFIX "prefix"
#define ATTRIBUTE_ALIAS "alias"
#define ATTRIBUTE_THRESHOLD "threshold"
#define ATTRIBUTE_COMPRESS "compress"


#define CONSTANT_HEADER_SIZE 8
#define CONSTANT_COMPRESSLEVEL_DEFAULT 0
#define CONSTANT_COMPRESSTHRESHOLD_DEFAULT 70

struct RCCFileInfo;

class RCCResourceLibrary
{
public:
    inline RCCResourceLibrary();
    ~RCCResourceLibrary();

    bool output(FILE *out);

    bool readFiles();

    inline void setInputFiles(QStringList files) { mFileNames = files; }
    inline QStringList inputFiles() const { return mFileNames; }

    QStringList dataFiles() const;

    inline void setVerbose(bool b) { mVerbose = b; }
    inline bool verbose() const { return mVerbose; }

    inline void setPythonVersion(int v) { mPrefix = (v == 2 ? "" : "b"); }

    inline void setInitName(const QString &n) { mInitName = n; }
    inline QString initName() const { return mInitName; }

    inline void setCompressLevel(int c) { mCompressLevel = c; }
    inline int compressLevel() const { return mCompressLevel; }

    inline void setCompressThreshold(int t) { mCompressThreshold = t; }
    inline int compressThreshold() const { return mCompressThreshold; }

    inline void setResourceRoot(QString str) { mResourceRoot = str; }
    inline QString resourceRoot() const { return mResourceRoot; }

private:
    RCCFileInfo *root;
    bool addFile(const QString &alias, const RCCFileInfo &file);
    bool interpretResourceFile(QIODevice *inputDevice, QString file, QString currentPath = QString());

    bool writeHeader(FILE *out);
    bool writeDataBlobs(FILE *out);
    bool writeDataNames(FILE *out);
    bool writeDataStructure(FILE *out);
    bool writeInitializer(FILE *out);

    QStringList mFileNames;
    QString mResourceRoot, mInitName;
    bool mVerbose;
    int mCompressLevel;
    int mCompressThreshold;
    int mTreeOffset, mNamesOffset, mDataOffset;
    const char *mPrefix;
};

inline RCCResourceLibrary::RCCResourceLibrary()
{
    root = 0;
    mVerbose = false;
    mCompressLevel = -1;
    mCompressThreshold = 70;
    mTreeOffset = mNamesOffset = mDataOffset = 0;
}

struct RCCFileInfo
{
    enum Flags
    {
        NoFlags = 0x00,
        Compressed = 0x01,
        Directory = 0x02
    };

    inline RCCFileInfo(QString name = QString(), QFileInfo fileInfo = QFileInfo(),
                       QLocale locale = QLocale(), uint flags = NoFlags,
                       int compressLevel = CONSTANT_COMPRESSLEVEL_DEFAULT, int compressThreshold = CONSTANT_COMPRESSTHRESHOLD_DEFAULT);
    ~RCCFileInfo() { qDeleteAll(children); }
    inline QString resourceName() {
        QString resource = name;
        for(RCCFileInfo *p = parent; p; p = p->parent)
            resource = resource.prepend(p->name + "/");
        return ":" + resource;
    }

    int flags;
    QString name;
    QLocale locale;
    QFileInfo fileInfo;
    RCCFileInfo *parent;
    QHash<QString, RCCFileInfo*> children;
    int mCompressLevel;
    int mCompressThreshold;

    qint64 nameOffset, dataOffset, childOffset;
    qint64 writeDataBlob(FILE *out, qint64 offset);
    qint64 writeDataName(FILE *out, qint64 offset);
    bool   writeDataInfo(FILE *out);
};

inline RCCFileInfo::RCCFileInfo(QString name, QFileInfo fileInfo, QLocale locale, uint flags,
                                int compressLevel, int compressThreshold)
{
    this->name = name;
    this->fileInfo = fileInfo;
    this->locale = locale;
    this->flags = flags;
    this->parent = 0;
    this->nameOffset = this->dataOffset = this->childOffset = 0;
    this->mCompressLevel = compressLevel;
    this->mCompressThreshold = compressThreshold;
}

#endif
