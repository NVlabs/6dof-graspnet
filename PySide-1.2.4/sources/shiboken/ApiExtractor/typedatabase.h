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

#ifndef TYPEDATABASE_H
#define TYPEDATABASE_H

#include <QStringList>
#include "typesystem.h"
#include "apiextractormacros.h"

void setTypeRevision(TypeEntry* typeEntry, int revision);
int getTypeRevision(const TypeEntry* typeEntry);
int getTypeIndex(const TypeEntry* typeEntry);
int getMaxTypeIndex();

class ContainerTypeEntry;
class PrimitiveTypeEntry;
class TypeDatabase
{
    TypeDatabase();
    TypeDatabase(const TypeDatabase&);
    TypeDatabase& operator=(const TypeDatabase&);
public:

    /**
    * Return the type system instance.
    * \param newInstance This parameter is useful just for unit testing, because singletons causes
    *                    too many side effects on unit testing.
    */
    static TypeDatabase* instance(bool newInstance = false);

    static QString normalizedSignature(const char* signature);

    QStringList requiredTargetImports() const;

    void addRequiredTargetImport(const QString& moduleName);

    QStringList typesystemPaths() const
    {
        return m_typesystemPaths;
    }

    void addTypesystemPath(const QString& typesystem_paths);

    IncludeList extraIncludes(const QString& className) const;

    PrimitiveTypeEntry* findPrimitiveType(const QString& name) const;
    ComplexTypeEntry* findComplexType(const QString& name) const;
    ObjectTypeEntry* findObjectType(const QString& name) const;
    NamespaceTypeEntry* findNamespaceType(const QString& name) const;
    ContainerTypeEntry* findContainerType(const QString& name) const;
    FunctionTypeEntry* findFunctionType(const QString& name) const;

    TypeEntry* findType(const QString& name) const;

    QList<TypeEntry *> findTypes(const QString &name) const
    {
        return m_entries.value(name);
    }

    TypeEntryHash allEntries() const
    {
        return m_entries;
    }

    SingleTypeEntryHash entries() const;

    PrimitiveTypeEntry* findTargetLangPrimitiveType(const QString& targetLangName) const;

    QList<const PrimitiveTypeEntry*> primitiveTypes() const;

    QList<const ContainerTypeEntry*> containerTypes() const;

    void addRejection(const QString& className, const QString& functionName,
                        const QString& fieldName, const QString& enumName);
    bool isClassRejected(const QString& className) const;
    bool isFunctionRejected(const QString& className, const QString& functionName) const;
    bool isFieldRejected(const QString& className, const QString& fieldName) const;
    bool isEnumRejected(const QString& className, const QString& enumName) const;

    void addType(TypeEntry* e)
    {
        m_entries[e->qualifiedCppName()].append(e);
    }

    SingleTypeEntryHash flagsEntries() const
    {
        return m_flagsEntries;
    }
    FlagsTypeEntry* findFlagsType(const QString& name) const;
    void addFlagsType(FlagsTypeEntry* fte)
    {
        m_flagsEntries[fte->originalName()] = fte;
    }

    TemplateEntry* findTemplate(const QString& name) const
    {
        return m_templates[name];
    }

    void addTemplate(TemplateEntry* t)
    {
        m_templates[t->name()] = t;
    }

    AddedFunctionList globalUserFunctions() const
    {
        return m_globalUserFunctions;
    }

    void addGlobalUserFunctions(const AddedFunctionList& functions)
    {
        m_globalUserFunctions << functions;
    }

    AddedFunctionList findGlobalUserFunctions(const QString& name) const;

    void addGlobalUserFunctionModifications(const FunctionModificationList& functionModifications)
    {
        m_functionMods << functionModifications;
    }

    void addGlobalUserFunctionModification(const FunctionModification& functionModification)
    {
        m_functionMods << functionModification;
    }

    FunctionModificationList functionModifications(const QString& signature) const;

    void setSuppressWarnings(bool on)
    {
        m_suppressWarnings = on;
    }

    void addSuppressedWarning(const QString& s)
    {
        m_suppressedWarnings.append(s);
    }

    bool isSuppressedWarning(const QString& s) const;

    void setRebuildClasses(const QStringList &cls)
    {
        m_rebuildClasses = cls;
    }

    static QString globalNamespaceClassName(const TypeEntry *te);
    QString filename() const
    {
        return "typesystem.txt";
    }

    QString modifiedTypesystemFilepath(const QString& tsFile) const;
    bool parseFile(const QString &filename, bool generate = true);
    bool parseFile(QIODevice* device, bool generate = true);

    APIEXTRACTOR_DEPRECATED(double apiVersion() const)
    {
        return m_apiVersion;
    }

    APIEXTRACTOR_DEPRECATED(void setApiVersion(double version))
    {
        m_apiVersion = version;
    }
    void setApiVersion(const QString& package, const QByteArray& version);

    APIEXTRACTOR_DEPRECATED(bool supportedApiVersion(double version) const);
    bool checkApiVersion(const QString& package, const QByteArray& version) const;

    const QStringList& dropTypeEntries() const
    {
        return m_dropTypeEntries;
    }

    bool hasDroppedTypeEntries() const
    {
        return !m_dropTypeEntries.isEmpty();
    }

    bool shouldDropTypeEntry(const QString& fullTypeName) const;

    void setDropTypeEntries(QStringList dropTypeEntries);

private:
    bool m_suppressWarnings;
    TypeEntryHash m_entries;
    SingleTypeEntryHash m_flagsEntries;
    TemplateEntryHash m_templates;
    QStringList m_suppressedWarnings;

    AddedFunctionList m_globalUserFunctions;
    FunctionModificationList m_functionMods;

    QStringList m_requiredTargetImports;

    QStringList m_typesystemPaths;
    QHash<QString, bool> m_parsedTypesystemFiles;

    QList<TypeRejection> m_rejections;
    QStringList m_rebuildClasses;

    double m_apiVersion;
    QStringList m_dropTypeEntries;
};

#endif
