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

#include "typedatabase.h"
#include "typesystem.h"
#include "typesystem_p.h"

#include <QFile>
#include <QXmlInputSource>
#include "reporthandler.h"
// #include <tr1/tuple>
#include <algorithm>

// package -> api-version
typedef QMap<QString, QByteArray> ApiVersionMap;

Q_GLOBAL_STATIC(ApiVersionMap, apiVersions)

TypeDatabase::TypeDatabase() : m_suppressWarnings(true), m_apiVersion(0)
{
    addType(new VoidTypeEntry());
    addType(new VarargsTypeEntry());
}

TypeDatabase* TypeDatabase::instance(bool newInstance)
{
    static TypeDatabase* db = 0;
    if (!db || newInstance) {
        if (db)
            delete db;
        db = new TypeDatabase;
    }
    return db;
}

QString TypeDatabase::normalizedSignature(const char* signature)
{
    QString normalized = QMetaObject::normalizedSignature(signature);

    if (!instance() || !QString(signature).contains("unsigned"))
        return normalized;

    QStringList types;
    types << "char" << "short" << "int" << "long";
    foreach (const QString& type, types) {
        if (instance()->findType(QString("u%1").arg(type)))
            continue;
        normalized.replace(QRegExp(QString("\\bu%1\\b").arg(type)), QString("unsigned %1").arg(type));
    }

    return normalized;
}

QStringList TypeDatabase::requiredTargetImports() const
{
    return m_requiredTargetImports;
}

void TypeDatabase::addRequiredTargetImport(const QString& moduleName)
{
    if (!m_requiredTargetImports.contains(moduleName))
        m_requiredTargetImports << moduleName;
}

void TypeDatabase::addTypesystemPath(const QString& typesystem_paths)
{
    #if defined(Q_OS_WIN32)
    char* path_splitter = const_cast<char*>(";");
    #else
    char* path_splitter = const_cast<char*>(":");
    #endif
    m_typesystemPaths += typesystem_paths.split(path_splitter);
}

IncludeList TypeDatabase::extraIncludes(const QString& className) const
{
    ComplexTypeEntry* typeEntry = findComplexType(className);
    if (typeEntry)
        return typeEntry->extraIncludes();
    else
        return IncludeList();
}

ContainerTypeEntry* TypeDatabase::findContainerType(const QString &name) const
{
    QString template_name = name;

    int pos = name.indexOf('<');
    if (pos > 0)
        template_name = name.left(pos);

    TypeEntry* type_entry = findType(template_name);
    if (type_entry && type_entry->isContainer())
        return static_cast<ContainerTypeEntry*>(type_entry);
    return 0;
}

FunctionTypeEntry* TypeDatabase::findFunctionType(const QString& name) const
{
    TypeEntry* entry = findType(name);
    if (entry && entry->type() == TypeEntry::FunctionType)
        return static_cast<FunctionTypeEntry*>(entry);
    return 0;
}


PrimitiveTypeEntry* TypeDatabase::findTargetLangPrimitiveType(const QString& targetLangName) const
{
    foreach (QList<TypeEntry*> entries, m_entries.values()) {
        foreach (TypeEntry* e, entries) {
            if (e && e->isPrimitive()) {
                PrimitiveTypeEntry *pe = static_cast<PrimitiveTypeEntry*>(e);
                if (pe->targetLangName() == targetLangName && pe->preferredConversion())
                    return pe;
            }
        }
    }

    return 0;
}

TypeEntry* TypeDatabase::findType(const QString& name) const
{
    QList<TypeEntry *> entries = findTypes(name);
    foreach (TypeEntry *entry, entries) {
        if (entry &&
            (!entry->isPrimitive() || static_cast<PrimitiveTypeEntry *>(entry)->preferredTargetLangType())) {
            return entry;
        }
    }
    return 0;
}

SingleTypeEntryHash TypeDatabase::entries() const
{
    TypeEntryHash entries = allEntries();

    SingleTypeEntryHash returned;
    QList<QString> keys = entries.keys();

    foreach (QString key, keys)
        returned[key] = findType(key);

    return returned;
}

QList<const PrimitiveTypeEntry*> TypeDatabase::primitiveTypes() const
{
    TypeEntryHash entries = allEntries();
    QList<const PrimitiveTypeEntry*> returned;
    foreach(QString key, entries.keys()) {
        foreach(const TypeEntry* typeEntry, entries[key]) {
            if (typeEntry->isPrimitive())
                returned.append((PrimitiveTypeEntry*) typeEntry);
        }
    }
    return returned;
}

QList<const ContainerTypeEntry*> TypeDatabase::containerTypes() const
{
    TypeEntryHash entries = allEntries();
    QList<const ContainerTypeEntry*> returned;
    foreach(QString key, entries.keys()) {
        foreach(const TypeEntry* typeEntry, entries[key]) {
            if (typeEntry->isContainer())
                returned.append((ContainerTypeEntry*) typeEntry);
        }
    }
    return returned;
}
void TypeDatabase::addRejection(const QString& className, const QString& functionName,
                                const QString& fieldName, const QString& enumName)
{
    TypeRejection r;
    r.class_name = className;
    r.function_name = functionName;
    r.field_name = fieldName;
    r.enum_name = enumName;

    m_rejections << r;
}

bool TypeDatabase::isClassRejected(const QString& className) const
{
    if (!m_rebuildClasses.isEmpty())
        return !m_rebuildClasses.contains(className);

    foreach (const TypeRejection& r, m_rejections)
    if (r.class_name == className && r.function_name == "*" && r.field_name == "*" && r.enum_name == "*")
        return true;

    return false;
}

bool TypeDatabase::isEnumRejected(const QString& className, const QString& enumName) const
{
    foreach (const TypeRejection& r, m_rejections) {
        if (r.enum_name == enumName
            && (r.class_name == className || r.class_name == "*")) {
            return true;
        }
    }

    return false;
}

bool TypeDatabase::isFunctionRejected(const QString& className, const QString& functionName) const
{
    foreach (const TypeRejection& r, m_rejections)
    if (r.function_name == functionName &&
        (r.class_name == className || r.class_name == "*"))
        return true;
    return false;
}


bool TypeDatabase::isFieldRejected(const QString& className, const QString& fieldName) const
{
    foreach (const TypeRejection& r, m_rejections)
    if (r.field_name == fieldName &&
        (r.class_name == className || r.class_name == "*"))
        return true;
    return false;
}

FlagsTypeEntry* TypeDatabase::findFlagsType(const QString &name) const
{
    FlagsTypeEntry* fte = (FlagsTypeEntry*) findType(name);
    if (!fte) {
        fte = (FlagsTypeEntry*) m_flagsEntries.value(name);
        if (!fte) {
            //last hope, search for flag without scope  inside of flags hash
            foreach(QString key, m_flagsEntries.keys()) {
                if (key.endsWith(name)) {
                    fte = (FlagsTypeEntry*) m_flagsEntries.value(key);
                    break;
                }
            }
        }
    }
    return fte;
}

AddedFunctionList TypeDatabase::findGlobalUserFunctions(const QString& name) const
{
    AddedFunctionList addedFunctions;
    foreach (AddedFunction func, m_globalUserFunctions) {
        if (func.name() == name)
            addedFunctions.append(func);
    }
    return addedFunctions;
}


QString TypeDatabase::globalNamespaceClassName(const TypeEntry * /*entry*/)
{
    return QLatin1String("Global");
}

FunctionModificationList TypeDatabase::functionModifications(const QString& signature) const
{
    FunctionModificationList lst;
    for (int i = 0; i < m_functionMods.count(); ++i) {
        const FunctionModification& mod = m_functionMods.at(i);
        if (mod.signature == signature)
            lst << mod;
    }

    return lst;
}

bool TypeDatabase::isSuppressedWarning(const QString& s) const
{
    if (!m_suppressWarnings)
        return false;

    foreach (const QString &_warning, m_suppressedWarnings) {
        QString warning(QString(_warning).replace("\\*", "&place_holder_for_asterisk;"));

        QStringList segs = warning.split("*", QString::SkipEmptyParts);
        if (!segs.size())
            continue;

        int i = 0;
        int pos = s.indexOf(QString(segs.at(i++)).replace("&place_holder_for_asterisk;", "*"));
        //qDebug() << "s == " << s << ", warning == " << segs;
        while (pos != -1) {
            if (i == segs.size())
                return true;
            pos = s.indexOf(QString(segs.at(i++)).replace("&place_holder_for_asterisk;", "*"), pos);
        }
    }

    return false;
}

QString TypeDatabase::modifiedTypesystemFilepath(const QString& tsFile) const
{
    if (!QFile::exists(tsFile)) {
        int idx = tsFile.lastIndexOf('/');
        QString fileName = idx >= 0 ? tsFile.right(tsFile.length() - idx - 1) : tsFile;
        foreach (const QString &path, m_typesystemPaths) {
            QString filepath(path + '/' + fileName);
            if (QFile::exists(filepath))
                return filepath;
        }
    }
    return tsFile;
}

bool TypeDatabase::parseFile(const QString &filename, bool generate)
{
    QString filepath = modifiedTypesystemFilepath(filename);
    if (m_parsedTypesystemFiles.contains(filepath))
        return m_parsedTypesystemFiles[filepath];

    QFile file(filepath);
    if (!file.exists()) {
        ReportHandler::warning("Can't find " + filename+", typesystem paths: "+m_typesystemPaths.join(", "));
        return false;
    }

    int count = m_entries.size();
    bool ok = parseFile(&file, generate);
    m_parsedTypesystemFiles[filepath] = ok;
    int newCount = m_entries.size();

    ReportHandler::debugSparse(QString::fromLatin1("Parsed: '%1', %2 new entries")
    .arg(filename)
    .arg(newCount - count));
    return ok;
}

bool TypeDatabase::parseFile(QIODevice* device, bool generate)
{
    if (m_apiVersion) // backwards compatibility with deprecated API
        setApiVersion("*", QByteArray::number(m_apiVersion));

    QXmlInputSource source(device);
    QXmlSimpleReader reader;
    Handler handler(this, generate);

    reader.setContentHandler(&handler);
    reader.setErrorHandler(&handler);

    return reader.parse(&source, false);
}

PrimitiveTypeEntry *TypeDatabase::findPrimitiveType(const QString& name) const
{
    QList<TypeEntry*> entries = findTypes(name);

    foreach (TypeEntry* entry, entries) {
        if (entry && entry->isPrimitive() && static_cast<PrimitiveTypeEntry*>(entry)->preferredTargetLangType())
            return static_cast<PrimitiveTypeEntry*>(entry);
    }

    return 0;
}

ComplexTypeEntry* TypeDatabase::findComplexType(const QString& name) const
{
    QList<TypeEntry*> entries = findTypes(name);
    foreach (TypeEntry* entry, entries) {
        if (entry && entry->isComplex())
            return static_cast<ComplexTypeEntry*>(entry);
    }
    return 0;
}

ObjectTypeEntry* TypeDatabase::findObjectType(const QString& name) const
{
    QList<TypeEntry*> entries = findTypes(name);
    foreach (TypeEntry* entry, entries) {
        if (entry && entry->isObject())
            return static_cast<ObjectTypeEntry*>(entry);
    }
    return 0;
}

NamespaceTypeEntry* TypeDatabase::findNamespaceType(const QString& name) const
{
    QList<TypeEntry*> entries = findTypes(name);
    foreach (TypeEntry* entry, entries) {
        if (entry && entry->isNamespace())
            return static_cast<NamespaceTypeEntry*>(entry);
    }
    return 0;
}

bool TypeDatabase::supportedApiVersion(double version) const
{
    return version <= m_apiVersion;
}

bool TypeDatabase::shouldDropTypeEntry(const QString& fullTypeName) const
{
    return m_dropTypeEntries.contains(fullTypeName);
}

void TypeDatabase::setDropTypeEntries(QStringList dropTypeEntries)
{
    m_dropTypeEntries = dropTypeEntries;
    m_dropTypeEntries.sort();
}

// Using std::pair to save some memory
// the pair means (revision, typeIndex)
// This global variable exists only because we can't break the ABI
typedef QHash<const TypeEntry*, std::pair<int, int> > TypeRevisionMap;
Q_GLOBAL_STATIC(TypeRevisionMap, typeEntryFields);
static bool computeTypeIndexes = true;
static int maxTypeIndex;

int getTypeRevision(const TypeEntry* typeEntry)
{
    return typeEntryFields()->value(typeEntry).first;
}

void setTypeRevision(TypeEntry* typeEntry, int revision)
{
    (*typeEntryFields())[typeEntry].first = revision;
    computeTypeIndexes = true;
}

static bool compareTypeEntriesByName(const TypeEntry* t1, const TypeEntry* t2)
{
    return t1->qualifiedCppName() < t2->qualifiedCppName();
}

static void _computeTypeIndexes()
{
    TypeDatabase* tdb = TypeDatabase::instance();
    typedef QMap<int, QList<TypeEntry*> > GroupedTypeEntries;
    GroupedTypeEntries groupedEntries;

    // Group type entries by revision numbers
    TypeEntryHash allEntries = tdb->allEntries();
    foreach (QList<TypeEntry*> entryList, allEntries) {
        foreach (TypeEntry* entry, entryList) {
            if (entry->isPrimitive()
                || entry->isContainer()
                || entry->isFunction()
                || !entry->generateCode()
                || entry->isEnumValue()
                || entry->isVarargs()
                || entry->isTypeSystem()
                || entry->isVoid()
                || entry->isCustom())
                continue;
            groupedEntries[getTypeRevision(entry)] << entry;
        }
    }

    maxTypeIndex = 0;
    GroupedTypeEntries::iterator it = groupedEntries.begin();
    for (; it != groupedEntries.end(); ++it) {
        // Remove duplicates
        QList<TypeEntry*>::iterator newEnd = std::unique(it.value().begin(), it.value().end());
        it.value().erase(newEnd, it.value().end());
        // Sort the type entries by name
        qSort(it.value().begin(), newEnd, compareTypeEntriesByName);

        foreach (TypeEntry* entry, it.value()) {
            (*typeEntryFields())[entry].second = maxTypeIndex++;
        }
    }
    computeTypeIndexes = false;
}

int getTypeIndex(const TypeEntry* typeEntry)
{
    if (computeTypeIndexes)
        _computeTypeIndexes();
    return typeEntryFields()->value(typeEntry).second;
}

int getMaxTypeIndex()
{
    if (computeTypeIndexes)
        _computeTypeIndexes();
    return maxTypeIndex;
}

void TypeDatabase::setApiVersion(const QString& package, const QByteArray& version)
{
    (*apiVersions())[package.trimmed()] = version.trimmed();
}

/**
 * Returns -1, 0 or 1 if v1 is less, equal or greater than v2
 */
static int versionCheck(const QByteArray& v1, const QByteArray& v2)
{
    if (v1.isEmpty() || v2.isEmpty())
        return 0;

    QList<QByteArray> v1Components = v1.split('.');
    QList<QByteArray> v2Components = v2.split('.');
    int numComponents = qMax(v1Components.count(), v2Components.count());
    while (v1Components.count() < numComponents)
        v1Components.append("0");
    while (v2Components.count() < numComponents)
        v2Components.append("0");

    for (int i = 0, max = v1Components.count(); i < max; ++i) {
        int v1Comp = v1Components[i].toInt();
        int v2Comp = v2Components[i].toInt();
        if (v1Comp > v2Comp)
            return 1;
        else if (v1Comp < v2Comp)
            return -1;
    }
    return 0;
}

bool TypeDatabase::checkApiVersion(const QString& package, const QByteArray& version) const
{
    ApiVersionMap* vMap = apiVersions();
    ApiVersionMap::const_iterator it = vMap->begin();
    for (; it != vMap->end(); ++it) {
        QRegExp regex(it.key(), Qt::CaseSensitive, QRegExp::Wildcard);
        if (regex.exactMatch(package))
            return versionCheck(it.value(), version) >= 0;
    }
    return false;
}
