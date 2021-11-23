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

#ifndef ABSTRACTMETABUILDER_H
#define ABSTRACTMETABUILDER_H

#include "parser/codemodel.h"
#include "abstractmetalang.h"
#include "typesystem.h"
#include "typeparser.h"

#include <QSet>
#include <QFileInfo>

class TypeDatabase;

class AbstractMetaBuilder
{
public:
    enum RejectReason {
        NotInTypeSystem,
        GenerationDisabled,
        RedefinedToNotClass,
        UnmatchedArgumentType,
        UnmatchedReturnType,
        ApiIncompatible,
        NoReason
    };

    AbstractMetaBuilder();
    virtual ~AbstractMetaBuilder();

    AbstractMetaClassList classes() const
    {
        return m_metaClasses;
    }
    AbstractMetaClassList templates() const
    {
        return m_templates;
    }
    /**
    *   Sorts a list of classes topologically, if an AbstractMetaClass object
    *   is passed the list of classes will be its inner classes, otherwise
    *   the list will be the module global classes.
    *   \return a list of classes sorted topologically
    */
    AbstractMetaClassList classesTopologicalSorted(const AbstractMetaClass* cppClass = 0) const;

    FileModelItem model() const
    {
        return m_dom;
    }

    void setModel(FileModelItem item)
    {
        m_dom = item;
    }

    ScopeModelItem popScope()
    {
        return m_scopes.takeLast();
    }

    void pushScope(ScopeModelItem item)
    {
        m_scopes << item;
    }

    ScopeModelItem currentScope() const
    {
        return m_scopes.last();
    }

    void dumpLog();

    bool build(QIODevice* input);
    void setLogDirectory(const QString& logDir);

    void figureOutEnumValuesForClass(AbstractMetaClass *metaClass, QSet<AbstractMetaClass *> *classes);
    int figureOutEnumValue(const QString &name, int value, AbstractMetaEnum *meta_enum, AbstractMetaFunction *metaFunction = 0);
    void figureOutEnumValues();
    void figureOutDefaultEnumArguments();

    void addAbstractMetaClass(AbstractMetaClass *cls);
    AbstractMetaClass *traverseTypeAlias(TypeAliasModelItem item);
    AbstractMetaClass *traverseClass(ClassModelItem item);
    AbstractMetaClass* currentTraversedClass(ScopeModelItem item);
    void traverseScopeMembers(ScopeModelItem item, AbstractMetaClass* metaClass);
    void traverseClassMembers(ClassModelItem scopeItem);
    void traverseNamespaceMembers(NamespaceModelItem scopeItem);
    bool setupInheritance(AbstractMetaClass *metaClass);
    AbstractMetaClass *traverseNamespace(NamespaceModelItem item);
    AbstractMetaEnum *traverseEnum(EnumModelItem item, AbstractMetaClass *enclosing, const QSet<QString> &enumsDeclarations);
    void traverseEnums(ScopeModelItem item, AbstractMetaClass *parent, const QStringList &enumsDeclarations);
    void traverseFunctions(ScopeModelItem item, AbstractMetaClass *parent);
    void applyFunctionModifications(AbstractMetaFunction* func);
    void traverseFields(ScopeModelItem item, AbstractMetaClass *parent);
    void traverseStreamOperator(FunctionModelItem functionItem);
    void traverseOperatorFunction(FunctionModelItem item);
    AbstractMetaFunction* traverseFunction(const AddedFunction& addedFunc);
    AbstractMetaFunction* traverseFunction(const AddedFunction& addedFunc, AbstractMetaClass* metaClass);
    AbstractMetaFunction *traverseFunction(FunctionModelItem function);
    AbstractMetaField *traverseField(VariableModelItem field, const AbstractMetaClass *cls);
    void checkFunctionModifications();
    void registerHashFunction(FunctionModelItem functionItem);
    void registerToStringCapability(FunctionModelItem functionItem);

    /**
     *   A conversion operator function should not have its owner class as
     *   its return type, but unfortunately it does. This function fixes the
     *   return type of operator functions of this kind making the return type
     *   be the same as it is supposed to generate when used in C++.
     *   If the returned type is a wrapped C++ class, this method also adds the
     *   conversion operator to the collection of external conversions of the
     *   said class.
     *   \param metaFunction conversion operator function to be fixed.
     */
    void fixReturnTypeOfConversionOperator(AbstractMetaFunction* metaFunction);

    void parseQ_Property(AbstractMetaClass *metaClass, const QStringList &declarations);
    void setupEquals(AbstractMetaClass *metaClass);
    void setupComparable(AbstractMetaClass *metaClass);
    void setupClonable(AbstractMetaClass *cls);
    void setupExternalConversion(AbstractMetaClass* cls);
    void setupFunctionDefaults(AbstractMetaFunction *metaFunction, AbstractMetaClass *metaClass);

    QString fixDefaultValue(ArgumentModelItem item, AbstractMetaType *type,
                                  AbstractMetaFunction *fnc, AbstractMetaClass *,
                                  int argumentIndex);
    AbstractMetaType* translateType(double vr, const AddedFunction::TypeInfo& typeInfo);
    AbstractMetaType *translateType(const TypeInfo &type, bool *ok, bool resolveType = true, bool resolveScope = true);

    int findOutValueFromString(const QString& stringValue, bool& ok);

    void decideUsagePattern(AbstractMetaType *type);

    const AbstractMetaFunctionList globalFunctions() const
    {
        return m_globalFunctions;
    }

    const AbstractMetaEnumList globalEnums() const
    {
        return m_globalEnums;
    }

    AbstractMetaClass *findTemplateClass(const QString& name, const AbstractMetaClass *context,
                                         TypeParser::Info *info = 0, ComplexTypeEntry **baseContainerType = 0) const;
    AbstractMetaClassList getBaseClasses(const AbstractMetaClass* metaClass) const;
    bool ancestorHasPrivateCopyConstructor(const AbstractMetaClass* metaClass) const;

    bool inheritTemplate(AbstractMetaClass *subclass,
                         const AbstractMetaClass *templateClass,
                         const TypeParser::Info &info);
    AbstractMetaType *inheritTemplateType(const QList<AbstractMetaType *> &templateTypes, const AbstractMetaType *metaType, bool *ok = 0);

    bool isQObject(const QString &qualifiedName);
    bool isEnum(const QStringList &qualifiedName);

    void fixQObjectForScope(TypeDatabase* types, NamespaceModelItem item);

    // QtScript
    QSet<QString> qtMetaTypeDeclaredTypeNames() const
    {
        return m_qmetatypeDeclaredTypenames;
    }

    /**
    *   AbstractMetaBuilder should know what's the global header being used,
    *   so any class declared under this header wont have the include file
    *   filled.
    */
    void setGlobalHeader(const QString& globalHeader);

protected:
    AbstractMetaClass *argumentToClass(ArgumentModelItem);

    virtual AbstractMetaClass *createMetaClass()
    {
        return new AbstractMetaClass();
    }

    virtual AbstractMetaEnum *createMetaEnum()
    {
        return new AbstractMetaEnum();
    }

    virtual AbstractMetaEnumValue *createMetaEnumValue()
    {
        return new AbstractMetaEnumValue();
    }

    virtual AbstractMetaField *createMetaField()
    {
        return new AbstractMetaField();
    }

    virtual AbstractMetaFunction *createMetaFunction()
    {
        return new AbstractMetaFunction();
    }

    virtual AbstractMetaArgument *createMetaArgument()
    {
        return new AbstractMetaArgument();
    }

    virtual AbstractMetaType *createMetaType()
    {
        return new AbstractMetaType();
    }

    FileModelItem m_dom;

private:
    void sortLists();
    AbstractMetaArgumentList reverseList(const AbstractMetaArgumentList& list);
    void setInclude(TypeEntry* te, const QString& fileName) const;
    void fixArgumentNames(AbstractMetaFunction* func);
    void fillAddedFunctions(AbstractMetaClass* metaClass);

    AbstractMetaClassList m_metaClasses;
    AbstractMetaClassList m_templates;
    AbstractMetaFunctionList m_globalFunctions;
    AbstractMetaEnumList m_globalEnums;

    QSet<const TypeEntry *> m_usedTypes;

    QMap<QString, RejectReason> m_rejectedClasses;
    QMap<QString, RejectReason> m_rejectedEnums;
    QMap<QString, RejectReason> m_rejectedFunctions;
    QMap<QString, RejectReason> m_rejectedFields;

    QList<AbstractMetaEnum *> m_enums;

    QList<QPair<AbstractMetaArgument *, AbstractMetaFunction *> > m_enumDefaultArguments;

    QHash<QString, AbstractMetaEnumValue *> m_enumValues;

    AbstractMetaClass *m_currentClass;
    QList<ScopeModelItem> m_scopes;
    QString m_namespacePrefix;

    QSet<AbstractMetaClass *> m_setupInheritanceDone;

    // QtScript
    QSet<QString> m_qmetatypeDeclaredTypenames;

    QString m_logDirectory;
    QFileInfo m_globalHeader;
};

#endif // ABSTRACTMETBUILDER_H
