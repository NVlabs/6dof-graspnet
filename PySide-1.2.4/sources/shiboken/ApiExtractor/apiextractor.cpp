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

#include "apiextractor.h"
#include <QDir>
#include <QDebug>
#include <QTemporaryFile>
#include <iostream>

#include "reporthandler.h"
#include "typesystem.h"
#include "fileout.h"
#include "parser/rpp/pp.h"
#include "abstractmetabuilder.h"
#include "typedatabase.h"

static bool preprocess(const QString& sourceFile,
                       QFile& targetFile,
                       const QStringList& includes);

ApiExtractor::ApiExtractor() : m_builder(0)
{
    static bool qrcInitialized = false;
    if (!qrcInitialized)
        Q_INIT_RESOURCE(generator);
    // Environment TYPESYSTEMPATH
    QString envTypesystemPaths = getenv("TYPESYSTEMPATH");
    if (!envTypesystemPaths.isEmpty())
        TypeDatabase::instance()->addTypesystemPath(envTypesystemPaths);
    ReportHandler::setContext("ApiExtractor");
}

ApiExtractor::~ApiExtractor()
{
    delete m_builder;
}

void ApiExtractor::addTypesystemSearchPath (const QString& path)
{
    TypeDatabase::instance()->addTypesystemPath(path);
}

void ApiExtractor::addTypesystemSearchPath(const QStringList& paths)
{
    foreach (QString path, paths)
        addTypesystemSearchPath(path);
}

void ApiExtractor::addIncludePath(const QString& path)
{
    m_includePaths << path;
}

void ApiExtractor::addIncludePath(const QStringList& paths)
{
    m_includePaths << paths;
}

void ApiExtractor::setLogDirectory(const QString& logDir)
{
    m_logDirectory = logDir;
}

void ApiExtractor::setCppFileName(const QString& cppFileName)
{
    m_cppFileName = cppFileName;
}

void ApiExtractor::setTypeSystem(const QString& typeSystemFileName)
{
    m_typeSystemFileName = typeSystemFileName;
}

void ApiExtractor::setDebugLevel(ReportHandler::DebugLevel debugLevel)
{
    ReportHandler::setDebugLevel(debugLevel);
}

void ApiExtractor::setSuppressWarnings ( bool value )
{
    TypeDatabase::instance()->setSuppressWarnings(value);
}

void ApiExtractor::setSilent ( bool value )
{
    ReportHandler::setSilent(value);
}

void ApiExtractor::setApiVersion(double version)
{
    TypeDatabase::instance()->setApiVersion("*", QByteArray::number(version));
}

void ApiExtractor::setApiVersion(const QString& package, const QByteArray& version)
{
    TypeDatabase::instance()->setApiVersion(package, version);
}

void ApiExtractor::setDropTypeEntries(QString dropEntries)
{
    dropEntries.remove(' ');
    QStringList entries = dropEntries.split(';');
    TypeDatabase::instance()->setDropTypeEntries(entries);
}

AbstractMetaEnumList ApiExtractor::globalEnums() const
{
    Q_ASSERT(m_builder);
    return m_builder->globalEnums();
}

AbstractMetaFunctionList ApiExtractor::globalFunctions() const
{
    Q_ASSERT(m_builder);
    return m_builder->globalFunctions();
}

AbstractMetaClassList ApiExtractor::classes() const
{
    Q_ASSERT(m_builder);
    return m_builder->classes();
}

AbstractMetaClassList ApiExtractor::classesTopologicalSorted() const
{
    Q_ASSERT(m_builder);
    return m_builder->classesTopologicalSorted();
}

PrimitiveTypeEntryList ApiExtractor::primitiveTypes() const
{
    return TypeDatabase::instance()->primitiveTypes();
}

ContainerTypeEntryList ApiExtractor::containerTypes() const
{
    return TypeDatabase::instance()->containerTypes();
}

QSet<QString> ApiExtractor::qtMetaTypeDeclaredTypeNames() const
{
    Q_ASSERT(m_builder);
    return m_builder->qtMetaTypeDeclaredTypeNames();
}

static const AbstractMetaEnum* findEnumOnClasses(AbstractMetaClassList metaClasses, const EnumTypeEntry* typeEntry)
{
    const AbstractMetaEnum* result = 0;
    foreach (const AbstractMetaClass* metaClass, metaClasses) {
        foreach (const AbstractMetaEnum* metaEnum, metaClass->enums()) {
            if (metaEnum->typeEntry() == typeEntry) {
                result = metaEnum;
                break;
            }
        }
        if (result)
            break;
        result = findEnumOnClasses(metaClass->innerClasses(), typeEntry);
    }
    return result;
}

const AbstractMetaEnum* ApiExtractor::findAbstractMetaEnum(const EnumTypeEntry* typeEntry) const
{
    if (!typeEntry)
        return 0;
    foreach (AbstractMetaEnum* metaEnum, m_builder->globalEnums()) {
        if (metaEnum->typeEntry() == typeEntry)
            return metaEnum;
    }
    return findEnumOnClasses(m_builder->classes(), typeEntry);
}

const AbstractMetaEnum* ApiExtractor::findAbstractMetaEnum(const TypeEntry* typeEntry) const
{
    if (!typeEntry)
        return 0;
    if (typeEntry->isFlags())
        return findAbstractMetaEnum(reinterpret_cast<const FlagsTypeEntry*>(typeEntry));
    if (typeEntry->isEnum())
        return findAbstractMetaEnum(reinterpret_cast<const EnumTypeEntry*>(typeEntry));
    return 0;
}

const AbstractMetaEnum* ApiExtractor::findAbstractMetaEnum(const FlagsTypeEntry* typeEntry) const
{
    if (!typeEntry)
        return 0;
    return findAbstractMetaEnum(typeEntry->originator());
}

const AbstractMetaEnum* ApiExtractor::findAbstractMetaEnum(const AbstractMetaType* metaType) const
{
    if (!metaType)
        return 0;
    return findAbstractMetaEnum(metaType->typeEntry());
}

int ApiExtractor::classCount() const
{
    Q_ASSERT(m_builder);
    return m_builder->classes().count();
}

bool ApiExtractor::run()
{
    if (m_builder)
        return false;

    if (m_typeSystemFileName.isEmpty()) {
        std::cerr << "You must specify a Type System file." << std::endl;
        return false;
    } else if (!TypeDatabase::instance()->parseFile(m_typeSystemFileName)) {
        std::cerr << "Cannot parse file: " << qPrintable(m_typeSystemFileName);
        return false;
    }

    QTemporaryFile ppFile;
#ifndef NDEBUG
    ppFile.setAutoRemove(false);
#endif
    // run rpp pre-processor
    if (!preprocess(m_cppFileName, ppFile, m_includePaths)) {
        std::cerr << "Preprocessor failed on file: " << qPrintable(m_cppFileName);
        return false;
    }
    ppFile.seek(0);
    m_builder = new AbstractMetaBuilder;
    m_builder->setLogDirectory(m_logDirectory);
    m_builder->setGlobalHeader(m_cppFileName);
    m_builder->build(&ppFile);

    return true;
}

static bool preprocess(const QString& sourceFile,
                       QFile& targetFile,
                       const QStringList& includes)
{
    rpp::pp_environment env;
    rpp::pp preprocess(env);

    rpp::pp_null_output_iterator null_out;

    const char *ppconfig = ":/trolltech/generator/pp-qt-configuration";

    QFile file(ppconfig);
    if (!file.open(QFile::ReadOnly)) {
        std::cerr << "Preprocessor configuration file not found " << ppconfig << std::endl;
        return false;
    }

    QByteArray ba = file.readAll();
    file.close();
    preprocess.operator()(ba.constData(), ba.constData() + ba.size(), null_out);

    preprocess.push_include_path(".");
    foreach (QString include, includes)
        preprocess.push_include_path(QDir::convertSeparators(include).toStdString());
    preprocess.push_include_path("/usr/include");

    QString currentDir = QDir::current().absolutePath();
    QFileInfo sourceInfo(sourceFile);
    if (!sourceInfo.exists()) {
        std::cerr << "File not found " << qPrintable(sourceFile) << std::endl;
        return false;
    }
    QDir::setCurrent(sourceInfo.absolutePath());

    std::string result;
    result.reserve(20 * 1024);  // 20K

    result += "# 1 \"builtins\"\n";
    result += "# 1 \"";
    result += sourceFile.toStdString();
    result += "\"\n";

    preprocess.file(sourceInfo.fileName().toStdString(),
                    rpp::pp_output_iterator<std::string> (result));

    QDir::setCurrent(currentDir);

    if (!targetFile.open(QIODevice::ReadWrite | QIODevice::Text)) {
        std::cerr << "Failed to write preprocessed file: " << qPrintable(targetFile.fileName()) << std::endl;
        return false;
    }

    targetFile.write(result.c_str(), result.length());
    return true;
}

