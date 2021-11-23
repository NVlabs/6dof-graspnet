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


#include <qstring.h>
#include <qstringlist.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qlocale.h>
#include <qtextstream.h>
#include <qbytearray.h>
#include <qhash.h>
#include <qdir.h>
#include "rcc.h"

// Some static globals
static QString initName;
static bool verbose = false;
static int py_version = 2;
static int compressLevel = CONSTANT_COMPRESSLEVEL_DEFAULT;
static int compressThreshold = CONSTANT_COMPRESSTHRESHOLD_DEFAULT;
static QString resourceRoot;

bool processResourceFile(const QStringList &filenamesIn, const QString &filenameOut, bool list)
{
    if (verbose)
        fprintf(stderr, "PySide resource compiler\n");

    //setup
    RCCResourceLibrary library;
    library.setInputFiles(filenamesIn);
    library.setInitName(initName);
    library.setVerbose(verbose);
    library.setPythonVersion(py_version);
    library.setCompressLevel(compressLevel);
    library.setCompressThreshold(compressThreshold);
    library.setResourceRoot(resourceRoot);
    if(!library.readFiles())
        return false;

    //open output
    FILE *out_fd = stdout;
    if (!filenameOut.isEmpty() && filenameOut != QLatin1String("-")) {
#if defined(_MSC_VER) && _MSC_VER >= 1400
		if (fopen_s(&out_fd, filenameOut.toLocal8Bit().constData(), "w")) {
#else
        out_fd = fopen(filenameOut.toLocal8Bit().constData(), "w");
        if(!out_fd) {
#endif
            fprintf(stderr, "Unable to open %s for writing\n", filenameOut.toLatin1().constData());
            return false;
        }
    }

    //do the task
    bool ret = true;
    if(list) {
        const QStringList data = library.dataFiles();
        for(int i = 0; i < data.size(); ++i)
            fprintf(out_fd, "%s\n", QDir::cleanPath(data.at(i)).toLatin1().constData());
    } else {
        ret = library.output(out_fd);
    }
    if(out_fd != stdout)
        fclose(out_fd);

    //done
    return ret;
}

int showHelp(const char *argv0, const QString &error)
{
    fprintf(stderr, "PySide resource compiler\n");
    if (!error.isEmpty())
        fprintf(stderr, "%s: %s\n", argv0, error.toLatin1().constData());
    fprintf(stderr, "Usage: %s  [options] <inputs>\n\n"
            "Options:\n"
            "    -o file           Write output to file rather than stdout\n"
            "    -py2              Generate code for any Python v2.x version (default)\n"
            "    -py3              Generate code for any Python v3.x version\n"
            "    -name name        Create an external initialization function with name\n"
            "    -threshold level  Threshold to consider compressing files\n"
            "    -compress level   Compress input files by level\n"
            "    -root path        Prefix resource access path with root path\n"
            "    -no-compress      Disable all compression\n"
            "    -version          Display version\n"
            "    -help             Display this information\n",
            argv0);
    return 1;
}

int main(int argc, char *argv[])
{
    QString outFilename;
    bool helpRequested = false, list = false;
    QStringList files;

    //parse options
    QString errorMsg;
    for (int i = 1; i < argc && errorMsg.isEmpty(); i++) {
        if (argv[i][0] == '-') {   // option
            QByteArray opt = argv[i] + 1;
            if (opt == "o") {
                if (!(i < argc-1)) {
                    errorMsg = QLatin1String("Missing output name");
                    break;
                }
                outFilename = argv[++i];
            } else if(opt == "name") {
                if (!(i < argc-1)) {
                    errorMsg = QLatin1String("Missing target name");
                    break;
                }
                initName = argv[++i];
            } else if(opt == "root") {
                if (!(i < argc-1)) {
                    errorMsg = QLatin1String("Missing root path");
                    break;
                }
                resourceRoot = QDir::cleanPath(argv[++i]);
                if(resourceRoot.isEmpty() || resourceRoot[0] != '/')
                    errorMsg = QLatin1String("Root must start with a /");
            } else if(opt == "compress") {
                if (!(i < argc-1)) {
                    errorMsg = QLatin1String("Missing compression level");
                    break;
                }
                compressLevel = QString(argv[++i]).toInt();
            } else if(opt == "threshold") {
                if (!(i < argc-1)) {
                    errorMsg = QLatin1String("Missing compression threshold");
                    break;
                }
                compressThreshold = QString(argv[++i]).toInt();
            } else if(opt == "verbose") {
                verbose = true;
            } else if(opt == "py2") {
                py_version = 2;
            } else if(opt == "py3") {
                py_version = 3;
            } else if(opt == "list") {
                list = true;
            } else if(opt == "version") {
                fprintf(stderr, "Resource Compiler for Qt version %s\n", QT_VERSION_STR);
                return 1;
            } else if(opt == "help" || opt == "h") {
                helpRequested = true;
            } else if(opt == "no-compress") {
                compressLevel = -2;
            } else {
                errorMsg = QString(QLatin1String("Unknown option: '%1'")).arg(argv[i]);
            }
        } else {
            if(!QFile::exists(argv[i])) {
                qWarning("%s: File does not exist '%s'", argv[0], argv[i]);
                return 1;
            }
            files.append(argv[i]);
        }
    }

    if (!files.size() || !errorMsg.isEmpty() || helpRequested)
        return showHelp(argv[0], errorMsg);
    return int(!processResourceFile(files, outFilename, list));
}



