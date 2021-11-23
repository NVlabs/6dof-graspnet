/*
 * This file is part of the PySide project.
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

#include <QCoreApplication>
#include <QLinkedList>
#include <QLibrary>
#include <QDomDocument>
#include <iostream>
#include <apiextractor.h>
#include "generator.h"
#include "shibokenconfig.h"
#include "cppgenerator.h"
#include "headergenerator.h"
#include "qtdocgenerator.h"

#ifdef _WINDOWS
    #define PATH_SPLITTER ";"
#else
    #define PATH_SPLITTER ":"
#endif

namespace {

class ArgsHandler
{
public:
    explicit ArgsHandler(const QMap<QString, QString>& other);
    virtual ~ArgsHandler();

    inline QMap<QString, QString>& args() const
    {
        return *m_args;
    }

    inline bool argExists(const QString& s) const
    {
        return m_args->contains(s);
    }

    QString removeArg(const QString& s);
    bool argExistsRemove(const QString& s);

    inline QString argValue(const QString& s) const
    {
        return m_args->value(s);
    }

    inline bool noArgs() const
    {
        return m_args->isEmpty();
    }

private:
    QMap<QString, QString>* m_args;
};

ArgsHandler::ArgsHandler(const QMap<QString, QString>& other)
    : m_args(new QMap<QString, QString>(other))
{
}

ArgsHandler::~ArgsHandler()
{
    delete m_args;
}

QString ArgsHandler::removeArg(const QString& s)
{
    QString retval;

    if (argExists(s)) {
        retval = argValue(s);
        m_args->remove(s);
    }

    return retval;
}

bool ArgsHandler::argExistsRemove(const QString& s)
{
    bool retval = false;

    if (argExists(s)) {
        retval = true;
        m_args->remove(s);
    }

    return retval;
}

}

static void printOptions(QTextStream& s, const QMap<QString, QString>& options)
{
    QMap<QString, QString>::const_iterator it = options.constBegin();
    s.setFieldAlignment(QTextStream::AlignLeft);
    for (; it != options.constEnd(); ++it) {
        s << "  --";
        s.setFieldWidth(38);
        s << it.key() << it.value();
        s.setFieldWidth(0);
        s << endl;
    }
}

typedef void (*getGeneratorsFunc)(QLinkedList<Generator*>*);

static bool processProjectFile(QFile& projectFile, QMap<QString, QString>& args)
{
    QByteArray line = projectFile.readLine().trimmed();
    if (line.isEmpty() || line != "[generator-project]")
        return false;

    QStringList includePaths;
    QStringList typesystemPaths;
    QStringList apiVersions;

    while (!projectFile.atEnd()) {
        line = projectFile.readLine().trimmed();
        if (line.isEmpty())
            continue;

        int split = line.indexOf("=");
        QString key;
        QString value;
        if (split > 0) {
            key = line.left(split - 1).trimmed();
            value = line.mid(split + 1).trimmed();
        } else {
            key = line;
        }

        if (key == "include-path")
            includePaths << QDir::toNativeSeparators(value);
        else if (key == "typesystem-path")
            typesystemPaths << QDir::toNativeSeparators(value);
        else if (key == "api-version")
            apiVersions << value;
        else if (key == "header-file")
            args["arg-1"] = value;
        else if (key == "typesystem-file")
            args["arg-2"] = value;
        else
            args[key] = value;
    }

    if (!includePaths.isEmpty())
        args["include-paths"] = includePaths.join(PATH_SPLITTER);

    if (!typesystemPaths.isEmpty())
        args["typesystem-paths"] = typesystemPaths.join(PATH_SPLITTER);
    if (!apiVersions.isEmpty())
        args["api-version"] = apiVersions.join("|");
    return true;
}

static QMap<QString, QString> getInitializedArguments()
{
    QMap<QString, QString> args;
    QStringList arguments = QCoreApplication::arguments();
    QString appName = arguments.first();
    arguments.removeFirst();

    QString projectFileName;
    foreach (const QString& arg, arguments) {
        if (arg.startsWith("--project-file")) {
            int split = arg.indexOf("=");
            if (split > 0)
                projectFileName = arg.mid(split + 1).trimmed();
            break;
        }
    }

    if (projectFileName.isNull())
        return args;

    if (!QFile::exists(projectFileName)) {
        std::cerr << qPrintable(appName) << ": Project file \"";
        std::cerr << qPrintable(projectFileName) << "\" not found.";
        std::cerr << std::endl;
        return args;
    }

    QFile projectFile(projectFileName);
    if (!projectFile.open(QIODevice::ReadOnly))
        return args;

    if (!processProjectFile(projectFile, args)) {
        std::cerr << qPrintable(appName) << ": first line of project file \"";
        std::cerr << qPrintable(projectFileName) << "\" must be the string \"[generator-project]\"";
        std::cerr << std::endl;
        return args;
    }

    return args;
}

static QMap<QString, QString> getCommandLineArgs()
{
    QMap<QString, QString> args = getInitializedArguments();
    QStringList arguments = QCoreApplication::arguments();
    arguments.removeFirst();

    int argNum = 0;
    foreach (QString arg, arguments) {
        arg = arg.trimmed();
        if (arg.startsWith("--")) {
            int split = arg.indexOf("=");
            if (split > 0)
                args[arg.mid(2).left(split-2)] = arg.mid(split + 1).trimmed();
            else
                args[arg.mid(2)] = QString();
        } else if (arg.startsWith("-")) {
            args[arg.mid(1)] = QString();
        } else {
            argNum++;
            args[QString("arg-%1").arg(argNum)] = arg;
        }
    }
    return args;
}

void printUsage(const GeneratorList& generators)
{
    QTextStream s(stdout);
    s << "Usage:\n  "
      << "shiboken [options] header-file typesystem-file\n\n"
      << "General options:\n";
    QMap<QString, QString> generalOptions;
    generalOptions.insert("project-file=<file>", "text file containing a description of the binding project. Replaces and overrides command line arguments");
    generalOptions.insert("debug-level=[sparse|medium|full]", "Set the debug level");
    generalOptions.insert("silent", "Avoid printing any message");
    generalOptions.insert("help", "Display this help and exit");
    generalOptions.insert("no-suppress-warnings", "Show all warnings");
    generalOptions.insert("output-directory=<path>", "The directory where the generated files will be written");
    generalOptions.insert("include-paths=<path>[" PATH_SPLITTER "<path>" PATH_SPLITTER "...]", "Include paths used by the C++ parser");
    generalOptions.insert("typesystem-paths=<path>[" PATH_SPLITTER "<path>" PATH_SPLITTER "...]", "Paths used when searching for typesystems");
    generalOptions.insert("documentation-only", "Do not generates any code, just the documentation");
    generalOptions.insert("license-file=<license-file>", "File used for copyright headers of generated files");
    generalOptions.insert("version", "Output version information and exit");
    generalOptions.insert("generator-set=<\"generator module\">", "generator-set to be used. e.g. qtdoc");
    generalOptions.insert("api-version=<\"package mask\">,<\"version\">", "Specify the supported api version used to generate the bindings");
    generalOptions.insert("drop-type-entries=\"<TypeEntry0>[;TypeEntry1;...]\"", "Semicolon separated list of type system entries (classes, namespaces, global functions and enums) to be dropped from generation.");
    printOptions(s, generalOptions);

    foreach (Generator* generator, generators) {
        QMap<QString, QString> options = generator->options();
        if (!options.isEmpty()) {
            s << endl << generator->name() << " options:\n";
            printOptions(s, generator->options());
        }
    }
}

static inline void printVerAndBanner()
{
    std::cout << "shiboken v" SHIBOKEN_VERSION << std::endl;
    std::cout << "Copyright (C) 2009-2012 Nokia Corporation and/or its subsidiary(-ies)" << std::endl;
}

static inline void errorPrint(const QString& s,
                              const bool& verAndBanner = false)
{
    if (verAndBanner)
        printVerAndBanner();

    std::cerr << s.toAscii().constData() << std::endl;
}

int main(int argc, char *argv[])
{
    // needed by qxmlpatterns
    QCoreApplication app(argc, argv);

    // Store command arguments in a map
    QMap<QString, QString> args = getCommandLineArgs();
    ArgsHandler argsHandler(args);
    GeneratorList generators;

    if (argsHandler.argExistsRemove("version")) {
        printVerAndBanner();
        return EXIT_SUCCESS;
    }

    QString generatorSet = argsHandler.removeArg("generator-set");
    // Also check "generatorSet" command line argument for backward compatibility.
    if (generatorSet.isEmpty())
        generatorSet = argsHandler.removeArg("generatorSet");

    // Pre-defined generator sets.
    if (generatorSet == "qtdoc") {
#ifndef DOCSTRINGS_ENABLED
        errorPrint("shiboken: Doc strings extractions was not enabled in this shiboken build.");
        return EXIT_FAILURE;
#else
        generators << new QtDocGenerator;
#endif
    } else if (generatorSet.isEmpty() || generatorSet == "shiboken") {
        generators << new CppGenerator << new HeaderGenerator;
    } else {
        errorPrint("shiboken: Unknown generator set, try \"shiboken\" or \"qtdoc\".");
        return EXIT_FAILURE;
    }

    if (argsHandler.argExistsRemove("help")) {
        printUsage(generators);
        return EXIT_SUCCESS;
    }

    QString licenseComment;
    QString licenseFileName = argsHandler.removeArg("license-file");
    if (!licenseFileName.isEmpty()) {
        if (QFile::exists(licenseFileName)) {
            QFile licenseFile(licenseFileName);
            if (licenseFile.open(QIODevice::ReadOnly))
                licenseComment = licenseFile.readAll();
        } else {
            errorPrint(QString("Couldn't find the file containing the license heading: %1").
                       arg(qPrintable(licenseFileName)));
            return EXIT_FAILURE;
        }
    }

    QString outputDirectory = argsHandler.removeArg("output-directory");
    if (outputDirectory.isEmpty())
        outputDirectory = "out";

    if (!QDir(outputDirectory).exists()) {
        if (!QDir().mkpath(outputDirectory)) {
            ReportHandler::warning("Can't create output directory: "+outputDirectory);
            return EXIT_FAILURE;
        }
    }

    // Create and set-up API Extractor
    ApiExtractor extractor;
    extractor.setLogDirectory(outputDirectory);

    if (argsHandler.argExistsRemove("silent")) {
        extractor.setSilent(true);
    } else {
        QString level = argsHandler.removeArg("debug-level");
        if (!level.isEmpty()) {
            if (level == "sparse")
                extractor.setDebugLevel(ReportHandler::SparseDebug);
            else if (level == "medium")
                extractor.setDebugLevel(ReportHandler::MediumDebug);
            else if (level == "full")
                extractor.setDebugLevel(ReportHandler::FullDebug);
        }
    }
    if (argsHandler.argExistsRemove("no-suppress-warnings"))
        extractor.setSuppressWarnings(false);

    if (argsHandler.argExists("api-version")) {
        QStringList versions = argsHandler.removeArg("api-version").split("|");
        foreach (QString fullVersion, versions) {
            QStringList parts = fullVersion.split(",");
            QString package;
            QString version;
            package = parts.count() == 1 ? "*" : parts.first();
            version = parts.last();
            extractor.setApiVersion(package, version.toAscii());
        }
    }

    if (argsHandler.argExists("drop-type-entries"))
        extractor.setDropTypeEntries(argsHandler.removeArg("drop-type-entries"));

    QString path = argsHandler.removeArg("typesystem-paths");
    if (!path.isEmpty())
        extractor.addTypesystemSearchPath(path.split(PATH_SPLITTER));

    path = argsHandler.removeArg("include-paths");
    if (!path.isEmpty())
        extractor.addIncludePath(path.split(PATH_SPLITTER));

    QString cppFileName = argsHandler.removeArg("arg-1");
    QString typeSystemFileName = argsHandler.removeArg("arg-2");

    /* Make sure to remove the project file's arguments (if any) and
     * --project-file, also the arguments of each generator before
     * checking if there isn't any existing arguments in argsHandler.
     */
    argsHandler.removeArg("project-file");
    QMap<QString, QString> projectFileArgs = getInitializedArguments();
    if (!projectFileArgs.isEmpty()) {
        QMap<QString, QString>::const_iterator it =
                                projectFileArgs.constBegin();
        for ( ; it != projectFileArgs.constEnd(); ++it)
            argsHandler.removeArg(it.key());
    }
    foreach (Generator* generator, generators) {
        QMap<QString, QString> options = generator->options();
        if (!options.isEmpty()) {
            QMap<QString, QString>::const_iterator it = options.constBegin();
            for ( ; it != options.constEnd(); ++it)
                argsHandler.removeArg(it.key());
        }
    }

    if (!argsHandler.noArgs()) {
        errorPrint("shiboken: Called with wrong arguments.");
        std::cout << "Note: use --help option for more information." << std::endl;
        return EXIT_FAILURE;
    }

    extractor.setCppFileName(cppFileName);
    extractor.setTypeSystem(typeSystemFileName);
    if (!extractor.run())
        return EXIT_FAILURE;

    if (!extractor.classCount())
        ReportHandler::warning("No C++ classes found!");

    foreach (Generator* g, generators) {
        g->setOutputDirectory(outputDirectory);
        g->setLicenseComment(licenseComment);
        if (g->setup(extractor, args))
            g->generate();
    }
    qDeleteAll(generators);

    ReportHandler::flush();
    std::cout << "Done, " << ReportHandler::warningCount();
    std::cout << " warnings (" << ReportHandler::suppressedCount() << " known issues)";
    std::cout << std::endl;
}
