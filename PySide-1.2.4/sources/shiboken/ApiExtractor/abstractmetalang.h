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

#ifndef ABSTRACTMETALANG_H
#define ABSTRACTMETALANG_H

#include "typesystem.h"

#include <QtCore/QSet>
#include <QtCore/QStringList>
#include <QtCore/QTextStream>
#include <QSharedPointer>


class AbstractMeta;
class AbstractMetaClass;
class AbstractMetaField;
class AbstractMetaFunction;
class AbstractMetaType;
class AbstractMetaVariable;
class AbstractMetaArgument;
class AbstractMetaEnumValue;
class AbstractMetaEnum;
class QPropertySpec;

class Documentation
{
public:
    enum Format {
        Native,
        Target
    };

    Documentation()
            : m_format(Documentation::Native)  {}

    Documentation(const QString& value, Format fmt = Documentation::Native)
            : m_data(value), m_format(fmt) {}

    QString value() const
    {
        return m_data;
    }

    void setValue(const QString& value, Format fmt = Documentation::Native)
    {
        m_data = value; m_format = fmt;
    }

    Documentation::Format format() const
    {
        return m_format;
    }

private:
    QString m_data;
    Format m_format;

};

typedef QList<AbstractMetaField *> AbstractMetaFieldList;
typedef QList<AbstractMetaArgument *> AbstractMetaArgumentList;
typedef QList<AbstractMetaFunction *> AbstractMetaFunctionList;
class AbstractMetaClassList : public  QList<AbstractMetaClass *>
{
public:
    AbstractMetaClass *findClass(const QString &name) const;
    AbstractMetaClass *findClass(const TypeEntry* typeEntry) const;
    AbstractMetaEnumValue *findEnumValue(const QString &string) const;
    AbstractMetaEnum *findEnum(const EnumTypeEntry *entry) const;

};

class AbstractMetaAttributes
{
public:
    AbstractMetaAttributes() : m_attributes(0), m_originalAttributes(0) {};

    enum Attribute {
        None                        = 0x00000000,

        Private                     = 0x00000001,
        Protected                   = 0x00000002,
        Public                      = 0x00000004,
        Friendly                    = 0x00000008,
        Visibility                  = 0x0000000f,

        Native                      = 0x00000010,
        Abstract                    = 0x00000020,
        Static                      = 0x00000040,

        FinalInTargetLang           = 0x00000080,
        FinalInCpp                  = 0x00000100,
        ForceShellImplementation    = 0x00000200,

        GetterFunction              = 0x00000400,
        SetterFunction              = 0x00000800,

        FinalOverload               = 0x00001000,
        InterfaceFunction           = 0x00002000,

        PropertyReader              = 0x00004000,
        PropertyWriter              = 0x00008000,
        PropertyResetter            = 0x00010000,

        Fake                        = 0x00020000,

        Invokable                   = 0x00040000,

        Final                       = FinalInTargetLang | FinalInCpp
    };

    uint attributes() const
    {
        return m_attributes;
    }

    void setAttributes(uint attributes)
    {
        m_attributes = attributes;
    }

    uint originalAttributes() const
    {
        return m_originalAttributes;
    }

    void setOriginalAttributes(uint attributes)
    {
        m_originalAttributes = attributes;
    }

    uint visibility() const
    {
        return m_attributes & Visibility;
    }

    void setVisibility(uint visi)
    {
        m_attributes = (m_attributes & ~Visibility) | visi;
    }

    void operator+=(Attribute attribute)
    {
        m_attributes |= attribute;
    }

    void operator-=(Attribute attribute)
    {
        m_attributes &= ~attribute;
    }

    bool isNative() const
    {
        return m_attributes & Native;
    }

    bool isFinal() const
    {
        return (m_attributes & Final) == Final;
    }

    bool isFinalInTargetLang() const
    {
        return m_attributes & FinalInTargetLang;
    }

    bool isFinalInCpp() const
    {
        return m_attributes & FinalInCpp;
    }

    bool isAbstract() const
    {
        return m_attributes & Abstract;
    }

    bool isStatic() const
    {
        return m_attributes & Static;
    }

    bool isForcedShellImplementation() const
    {
        return m_attributes & ForceShellImplementation;
    }

    bool isInterfaceFunction() const
    {
        return m_attributes & InterfaceFunction;
    }

    bool isFinalOverload() const
    {
        return m_attributes & FinalOverload;
    }

    bool isInvokable() const
    {
        return m_attributes & Invokable;
    }

    bool isPropertyReader() const
    {
        return m_attributes & PropertyReader;
    }

    bool isPropertyWriter() const
    {
        return m_attributes & PropertyWriter;
    }

    bool isPropertyResetter() const
    {
        return m_attributes & PropertyResetter;
    }

    bool isPrivate() const
    {
        return m_attributes & Private;
    }

    bool isProtected() const
    {
        return m_attributes & Protected;
    }

    bool isPublic() const
    {
        return m_attributes & Public;
    }

    bool isFriendly() const
    {
        return m_attributes & Friendly;
    }

    bool wasPrivate() const
    {
        return m_originalAttributes & Private;
    }

    bool wasProtected() const
    {
        return m_originalAttributes & Protected;
    }

    bool wasPublic() const
    {
        return m_originalAttributes & Public;
    }

    bool wasFriendly() const
    {
        return m_originalAttributes & Friendly;
    }

    void setDocumentation(const Documentation& doc)
    {
        m_doc = doc;
    }

    Documentation documentation() const
    {
        return m_doc;
    }

private:
    uint m_attributes;
    uint m_originalAttributes;
    Documentation m_doc;
};

typedef QList<AbstractMetaType*> AbstractMetaTypeList;
class AbstractMetaType
{
public:

    enum TypeUsagePattern {
        InvalidPattern,
        PrimitivePattern,
        FlagsPattern,
        EnumPattern,
        ValuePattern,
        StringPattern,
        CharPattern,
        ObjectPattern,
        QObjectPattern,
        ValuePointerPattern,
        NativePointerPattern,
        ContainerPattern,
        VariantPattern,
        VarargsPattern,
        JObjectWrapperPattern,
        ArrayPattern,
        ThreadPattern
    };

    AbstractMetaType();
    ~AbstractMetaType();

    QString package() const
    {
        return m_typeEntry->targetLangPackage();
    }
    QString name() const
    {
        if (m_name.isNull())
            m_name = m_typeEntry->targetLangName().split("::").last();
        return m_name;
    }
    QString fullName() const
    {
        return m_typeEntry->qualifiedTargetLangName();
    }

    void setTypeUsagePattern(TypeUsagePattern pattern)
    {
        m_pattern = pattern;
    }
    TypeUsagePattern typeUsagePattern() const
    {
        return m_pattern;
    }

    // true when use pattern is container
    bool hasInstantiations() const
    {
        return !m_instantiations.isEmpty();
    }

    void addInstantiation(AbstractMetaType* inst, bool owner = false)
    {
        if (owner)
            m_children << inst;
        m_instantiations << inst;
    }

    void setInstantiations(const AbstractMetaTypeList  &insts, bool owner = false)
    {
        m_instantiations = insts;
        if (owner) {
            m_children.clear();
            m_children = insts;
        }
    }

    AbstractMetaTypeList instantiations() const
    {
        return m_instantiations;
    }

    void setInstantiationInCpp(bool incpp)
    {
        m_cppInstantiation = incpp;
    }
    bool hasInstantiationInCpp() const
    {
        return hasInstantiations() && m_cppInstantiation;
    }

    QString minimalSignature() const;

    // true when the type is a QtJambiObject subclass
    bool hasNativeId() const;

    // returns true if the typs is used as a non complex primitive, no & or *'s
    bool isPrimitive() const
    {
        return m_pattern == PrimitivePattern;
    }

    // returns true if the type is used as an enum
    bool isEnum() const
    {
        return m_pattern == EnumPattern;
    }

    // returns true if the type is used as a QObject *
    bool isQObject() const
    {
        return m_pattern == QObjectPattern;
    }

    // returns true if the type is used as an object, e.g. Xxx *
    bool isObject() const
    {
        return m_pattern == ObjectPattern;
    }

    // returns true if the type is used as an array, e.g. Xxx[42]
    bool isArray() const
    {
        return m_pattern == ArrayPattern;
    }

    // returns true if the type is used as a value type (X or const X &)
    bool isValue() const
    {
        return m_pattern == ValuePattern;
    }

    bool isValuePointer() const
    {
        return m_pattern == ValuePointerPattern;
    }

    // returns true for more complex types...
    bool isNativePointer() const
    {
        return m_pattern == NativePointerPattern;
    }

    // returns true if the type was originally a QString or const QString & or equivalent for QLatin1String
    bool isTargetLangString() const
    {
        return m_pattern == StringPattern;
    }

    // returns true if the type was originally a QChar or const QChar &
    bool isTargetLangChar() const
    {
        return m_pattern == CharPattern;
    }

    // return true if the type was originally a QVariant or const QVariant &
    bool isVariant() const
    {
        return m_pattern == VariantPattern;
    }

    // return true if the type was originally a varargs
    bool isVarargs() const
    {
        return m_pattern == VarargsPattern;
    }

    // return true if the type was originally a JObjectWrapper or const JObjectWrapper &
    bool isJObjectWrapper() const
    {
        return m_pattern == JObjectWrapperPattern;
    }

    // returns true if the type was used as a container
    bool isContainer() const
    {
        return m_pattern == ContainerPattern;
    }

    // returns true if the type was used as a flag
    bool isFlags() const
    {
        return m_pattern == FlagsPattern;
    }

    // returns true if the type was used as a thread
    bool isThread() const
    {
        return m_pattern == ThreadPattern;
    }

    bool isConstant() const
    {
        return m_constant;
    }
    void setConstant(bool constant)
    {
        m_constant = constant;
    }

    bool isReference() const
    {
        return m_reference;
    }
    void setReference(bool ref)
    {
        m_reference = ref;
    }

    /**
     *   Says if the type is to be implemented using target language
     *   equivalent of C++ enums, i.e. not plain ints.
     *   /return true if the type is to be implemented using target
     *   language enums
     */
    bool isTargetLangEnum() const
    {
        return isEnum() && !((EnumTypeEntry *) typeEntry())->forceInteger();
    }
    bool isIntegerEnum() const
    {
        return isEnum() && !isTargetLangEnum();
    }

    /**
     *   Says if the type is to be implemented using target language
     *   equivalent of Qt's QFlags, i.e. not plain ints.
     *   /return true if the type is to be implemented using target
     *   language QFlags
     */
    bool isTargetLangFlags() const
    {
        return isFlags() && !((FlagsTypeEntry *) typeEntry())->forceInteger();
    }
    bool isIntegerFlags() const
    {
        return isFlags() && !isTargetLangFlags();
    }

    int actualIndirections() const
    {
        return m_indirections + (isReference() ? 1 : 0);
    }
    int indirections() const
    {
        return m_indirections;
    }
    void setIndirections(int indirections)
    {
        m_indirections = indirections;
    }

    void setArrayElementCount(int n)
    {
        m_arrayElementCount = n;
    }
    int arrayElementCount() const
    {
        return m_arrayElementCount;
    }

    const AbstractMetaType *arrayElementType() const
    {
        return m_arrayElementType;
    }
    void setArrayElementType(const AbstractMetaType *t)
    {
        m_arrayElementType = t;
    }

    QString cppSignature() const;

    AbstractMetaType *copy() const;

    const TypeEntry *typeEntry() const
    {
        return m_typeEntry;
    }
    void setTypeEntry(const TypeEntry *type)
    {
        m_typeEntry = type;
    }

    void setOriginalTypeDescription(const QString &otd)
    {
        m_originalTypeDescription = otd;
    }
    QString originalTypeDescription() const
    {
        return m_originalTypeDescription;
    }

    void setOriginalTemplateType(const AbstractMetaType *type)
    {
        m_originalTemplateType = type;
    }
    const AbstractMetaType *originalTemplateType() const
    {
        return m_originalTemplateType;
    }

    /// Decides and sets the proper usage patter for the current meta type.
    void decideUsagePattern();

private:
    const TypeEntry *m_typeEntry;
    AbstractMetaTypeList m_instantiations;
    QString m_package;
    mutable QString m_name;
    mutable QString m_cachedCppSignature;
    QString m_originalTypeDescription;

    int m_arrayElementCount;
    const AbstractMetaType *m_arrayElementType;
    const AbstractMetaType *m_originalTemplateType;

    TypeUsagePattern m_pattern;
    uint m_constant : 1;
    uint m_reference : 1;
    uint m_cppInstantiation : 1;
    int m_indirections : 4;
    uint m_reserved : 25; // unused
    AbstractMetaTypeList m_children;

    Q_DISABLE_COPY(AbstractMetaType);
};

class AbstractMetaVariable
{
public:
    AbstractMetaVariable() : m_type(0), m_hasName(false) {}
    AbstractMetaVariable(const AbstractMetaVariable &other);

    virtual ~AbstractMetaVariable()
    {
        delete m_type;
    }

    AbstractMetaType *type() const
    {
        return m_type;
    }
    void setType(AbstractMetaType *type)
    {
        Q_ASSERT(m_type == 0);
        m_type = type;
    }
    void replaceType(AbstractMetaType *type)
    {
        if (m_type)
            delete m_type;
        m_type = type;
    }

    QString name() const
    {
        return m_name;
    }
    void setName(const QString &name, bool realName = true)
    {
        m_name = name;
        m_hasName = realName;
    }
    bool hasName() const
    {
        return m_hasName;
    }
    QString originalName() const
    {
        return m_originalName;
    }
    void setOriginalName(const QString& name)
    {
        m_originalName = name;
    }
    void setDocumentation(const Documentation& doc)
    {
        m_doc = doc;
    }
    Documentation documentation() const
    {
        return m_doc;
    }

private:
    QString m_originalName;
    QString m_name;
    AbstractMetaType *m_type;
    bool m_hasName;

    Documentation m_doc;
};



class AbstractMetaArgument : public AbstractMetaVariable
{
public:
    AbstractMetaArgument() : m_argumentIndex(0) {};

    QString defaultValueExpression() const
    {
        return m_expression;
    }
    void setDefaultValueExpression(const QString &expr)
    {
        m_expression = expr;
    }

    QString originalDefaultValueExpression() const
    {
        return m_originalExpression;
    }
    void setOriginalDefaultValueExpression(const QString &expr)
    {
        m_originalExpression = expr;
    }

    QString toString() const
    {
        return type()->name() + " " + AbstractMetaVariable::name() +
               (m_expression.isEmpty() ? "" :  " = " + m_expression);
    }

    int argumentIndex() const
    {
        return m_argumentIndex;
    }
    void setArgumentIndex(int argIndex)
    {
        m_argumentIndex = argIndex;
    }

    AbstractMetaArgument *copy() const;
private:
    QString m_expression;
    QString m_originalExpression;
    int m_argumentIndex;

    friend class AbstractMetaClass;
};


class AbstractMetaField : public AbstractMetaVariable, public AbstractMetaAttributes
{
public:
    AbstractMetaField();
    ~AbstractMetaField();

    const AbstractMetaClass *enclosingClass() const
    {
        return m_class;
    }
    void setEnclosingClass(const AbstractMetaClass *cls)
    {
        m_class = cls;
    }

    const AbstractMetaFunction *getter() const;
    const AbstractMetaFunction *setter() const;

    FieldModificationList modifications() const;

    bool isModifiedRemoved(int types = TypeSystem::All) const;

    using AbstractMetaVariable::setDocumentation;
    using AbstractMetaVariable::documentation;

    AbstractMetaField *copy() const;

private:
    mutable AbstractMetaFunction *m_getter;
    mutable AbstractMetaFunction *m_setter;
    const AbstractMetaClass *m_class;
};

class AbstractMetaFunction : public AbstractMetaAttributes
{
public:
    enum FunctionType {
        ConstructorFunction,
        DestructorFunction,
        NormalFunction,
        SignalFunction,
        EmptyFunction,
        SlotFunction,
        GlobalScopeFunction
    };

    enum CompareResult {
        EqualName                   = 0x00000001,
        EqualArguments              = 0x00000002,
        EqualAttributes             = 0x00000004,
        EqualImplementor            = 0x00000008,
        EqualReturnType             = 0x00000010,
        EqualDefaultValueOverload   = 0x00000020,
        EqualModifiedName           = 0x00000040,

        NameLessThan                = 0x00001000,

        PrettySimilar               = EqualName | EqualArguments,
        Equal                       = 0x0000001f,
        NotEqual                    = 0x00001000
    };

    AbstractMetaFunction()
            : m_typeEntry(0),
            m_functionType(NormalFunction),
            m_type(0),
            m_class(0),
            m_implementingClass(0),
            m_declaringClass(0),
            m_interfaceClass(0),
            m_propertySpec(0),
            m_constant(false),
            m_invalid(false),
            m_reverse(false),
            m_userAdded(false),
            m_explicit(false),
            m_pointerOperator(false),
            m_isCallOperator(false)
    {
    }

    ~AbstractMetaFunction();

    QString name() const
    {
        return m_name;
    }

    void setName(const QString &name)
    {
        m_name = name;
    }

    QString originalName() const
    {
        return m_originalName.isEmpty() ? name() : m_originalName;
    }

    void setOriginalName(const QString &name)
    {
        m_originalName = name;
    }

    void setReverseOperator(bool reverse)
    {
        m_reverse = reverse;
    }

    bool isReverseOperator() const
    {
        return m_reverse;
    }

    /**
     *  Returns true if this is a operator and the "self" operand is a pointer.
     *  e.g. class Foo {}; operator+(SomeEnum, Foo*);
     */
    bool isPointerOperator() const
    {
        return m_pointerOperator;
    }

    void setPointerOperator(bool value)
    {
        m_pointerOperator = value;
    }

    void setExplicit(bool isExplicit)
    {
        m_explicit = isExplicit;
    }
    /**
    *   Says if the function (a constructor) was declared as explicit in C++.
    *   \return true if the function was declared as explicit in C++
    */
    bool isExplicit() const
    {
        return m_explicit;
    }

    static bool isConversionOperator(QString funcName);
    bool isConversionOperator() const
    {
        return isConversionOperator(originalName());
    }

    static bool isOperatorOverload(QString funcName);
    bool isOperatorOverload() const
    {
        return isOperatorOverload(originalName());
    }
    bool isCastOperator() const;

    bool isArithmeticOperator() const;
    bool isBitwiseOperator() const;
    bool isComparisonOperator() const;
    bool isLogicalOperator() const;
    bool isSubscriptOperator() const;
    bool isAssignmentOperator() const;
    bool isOtherOperator() const;

    /**
     * Informs the arity of the operator or -1 if the function is not
     * an operator overload.
     * /return the arity of the operator or -1
     */
    int arityOfOperator() const;
    bool isUnaryOperator() const { return arityOfOperator() == 1; }
    bool isBinaryOperator() const { return arityOfOperator() == 2; }
    bool isInplaceOperator() const;

    // TODO: ths function *should* know if it is virtual
    // instead of asking to your implementing class.
    bool isVirtual() const;
    bool isCopyConstructor() const;
    bool isThread() const;
    bool allowThread() const;
    QString modifiedName() const;

    QString minimalSignature() const;
    QStringList possibleIntrospectionCompatibleSignatures() const;

    QString marshalledName() const;

    // true if one or more of the arguments are of QtJambiObject subclasses
    bool argumentsHaveNativeId() const
    {
        foreach (const AbstractMetaArgument *arg, m_arguments) {
            if (arg->type()->hasNativeId())
                return true;
        }

        return false;
    }

    bool isModifiedRemoved(int types = TypeSystem::All) const;

    AbstractMetaType *type() const
    {
        return m_type;
    }
    void setType(AbstractMetaType *type)
    {
        Q_ASSERT(m_type == 0);
        m_type = type;
    }

    void replaceType(AbstractMetaType *type)
    {
        if (m_type)
            delete m_type;
        m_type = type;
    }

    // The class that has this function as a member.
    const AbstractMetaClass *ownerClass() const
    {
        return m_class;
    }
    void setOwnerClass(const AbstractMetaClass *cls)
    {
        m_class = cls;
    }

    // The first class in a hierarchy that declares the function
    const AbstractMetaClass *declaringClass() const
    {
        return m_declaringClass;
    }
    void setDeclaringClass(const AbstractMetaClass *cls)
    {
        m_declaringClass = cls;
    }

    // The class that actually implements this function
    const AbstractMetaClass *implementingClass() const
    {
        return m_implementingClass;
    }
    void setImplementingClass(const AbstractMetaClass *cls)
    {
        m_implementingClass = cls;
    }

    bool needsCallThrough() const;

    AbstractMetaArgumentList arguments() const
    {
        return m_arguments;
    }
    void setArguments(const AbstractMetaArgumentList &arguments)
    {
        m_arguments = arguments;
    }
    void addArgument(AbstractMetaArgument *argument)
    {
        m_arguments << argument;
    }
    int actualMinimumArgumentCount() const;

    void setInvalid(bool on)
    {
        m_invalid = on;
    }
    bool isInvalid() const
    {
        return m_invalid;
    }
    bool isDeprecated() const;
    bool isDestructor() const
    {
        return functionType() == DestructorFunction;
    }
    bool isConstructor() const
    {
        return functionType() == ConstructorFunction;
    }
    bool isNormal() const
    {
        return functionType() == NormalFunction || isSlot() || isInGlobalScope();
    }
    bool isInGlobalScope() const
    {
        return functionType() == GlobalScopeFunction;
    }
    bool isSignal() const
    {
        return functionType() == SignalFunction;
    }
    bool isSlot() const
    {
        return functionType() == SlotFunction;
    }
    bool isEmptyFunction() const
    {
        return functionType() == EmptyFunction;
    }
    FunctionType functionType() const
    {
        return m_functionType;
    }
    void setFunctionType(FunctionType type)
    {
        m_functionType = type;
    }

    QStringList introspectionCompatibleSignatures(const QStringList &resolvedArguments = QStringList()) const;
    QString signature() const;
    QString targetLangSignature(bool minimal = false) const;
    bool shouldReturnThisObject() const
    {
        return QLatin1String("this") == argumentReplaced(0);
    }
    bool shouldIgnoreReturnValue() const
    {
        return QLatin1String("void") == argumentReplaced(0);
    }

    bool isConstant() const
    {
        return m_constant;
    }
    void setConstant(bool constant)
    {
        m_constant = constant;
    }

    /// Returns true if the AbstractMetaFunction was added by the user via the type system description.
    bool isUserAdded() const
    {
        return m_userAdded;
    }
    void setUserAdded(bool userAdded)
    {
        m_userAdded = userAdded;
    }

    QString toString() const
    {
        return m_name;
    }

    uint compareTo(const AbstractMetaFunction *other) const;

    bool operator <(const AbstractMetaFunction &a) const;

    AbstractMetaFunction *copy() const;

    QString replacedDefaultExpression(const AbstractMetaClass *cls, int idx) const;
    bool removedDefaultExpression(const AbstractMetaClass *cls, int idx) const;
    QString conversionRule(TypeSystem::Language language, int idx) const;
    QList<ReferenceCount> referenceCounts(const AbstractMetaClass *cls, int idx = -2) const;
    ArgumentOwner argumentOwner(const AbstractMetaClass *cls, int idx) const;

    bool nullPointersDisabled(const AbstractMetaClass *cls = 0, int argument_idx = 0) const;
    QString nullPointerDefaultValue(const AbstractMetaClass *cls = 0, int argument_idx = 0) const;

    bool resetObjectAfterUse(int argument_idx) const;

    // Returns whether garbage collection is disabled for the argument in any context
    bool disabledGarbageCollection(const AbstractMetaClass *cls, int key) const;

    // Returns the ownership rules for the given argument in the given context
    TypeSystem::Ownership ownership(const AbstractMetaClass *cls, TypeSystem::Language language, int idx) const;

    bool isVirtualSlot() const;

    QString typeReplaced(int argument_index) const;
    bool isRemovedFromAllLanguages(const AbstractMetaClass *) const;
    bool isRemovedFrom(const AbstractMetaClass *, TypeSystem::Language language) const;
    bool argumentRemoved(int) const;

    QString argumentReplaced(int key) const;
    bool needsSuppressUncheckedWarning() const;

    bool hasModifications(const AbstractMetaClass *implementor) const;
    /**
    *   Verifies if any modification to the function is an inject code.
    *   \return true if there is inject code modifications to the function.
    */
    bool hasInjectedCode() const;
    /**
    *   Returns a list of code snips for this function.
    *   The code snips can be filtered by position and language.
    *   \return list of code snips
    */
    CodeSnipList injectedCodeSnips(CodeSnip::Position position = CodeSnip::Any,
                                   TypeSystem::Language language = TypeSystem::All) const;

    /**
    *   Verifies if any modification to the function alters/removes its
    *   arguments types or default values.
    *   \return true if there is some modification to function signature
    */
    bool hasSignatureModifications() const;
    FunctionModificationList modifications(const AbstractMetaClass* implementor = 0) const;

    /**
     * Return the argument name if there is a modification the renamed value will be returned
     */
    QString argumentName(int index, bool create = true, const AbstractMetaClass *cl = 0) const;

    // If this function stems from an interface, this returns the
    // interface that declares it.
    const AbstractMetaClass *interfaceClass() const
    {
        return m_interfaceClass;
    }

    void setInterfaceClass(const AbstractMetaClass *cl)
    {
        m_interfaceClass = cl;
    }

    void setPropertySpec(QPropertySpec *spec)
    {
        m_propertySpec = spec;
    }

    QPropertySpec *propertySpec() const
    {
        return m_propertySpec;
    }

    FunctionTypeEntry* typeEntry() const
    {
        return m_typeEntry;
    }

    void setTypeEntry(FunctionTypeEntry* typeEntry)
    {
        m_typeEntry = typeEntry;
    }

    bool isCallOperator() const;
private:
    QString m_name;
    QString m_originalName;
    mutable QString m_cachedMinimalSignature;
    mutable QString m_cachedSignature;
    mutable QString m_cachedModifiedName;

    FunctionTypeEntry* m_typeEntry;
    FunctionType m_functionType;
    AbstractMetaType *m_type;
    const AbstractMetaClass *m_class;
    const AbstractMetaClass *m_implementingClass;
    const AbstractMetaClass *m_declaringClass;
    const AbstractMetaClass *m_interfaceClass;
    QPropertySpec *m_propertySpec;
    AbstractMetaArgumentList m_arguments;
    uint m_constant                 : 1;
    uint m_invalid                  : 1;
    uint m_reverse                  : 1;
    uint m_userAdded                : 1;
    uint m_explicit                 : 1;
    uint m_pointerOperator          : 1;
    uint m_isCallOperator           : 1;
};


class AbstractMetaEnumValue
{
public:
    AbstractMetaEnumValue()
            : m_valueSet(false), m_value(0)
    {
    }

    int value() const
    {
        return m_value;
    }

    void setValue(int value)
    {
        m_valueSet = true;
        m_value = value;
    }

    QString stringValue() const
    {
        return m_stringValue;
    }

    void setStringValue(const QString &v)
    {
        m_stringValue = v;
    }

    QString name() const
    {
        return m_name;
    }

    void setName(const QString &name)
    {
        m_name = name;
    }

    bool isValueSet() const
    {
        return m_valueSet;
    }

    void setDocumentation(const Documentation& doc)
    {
        m_doc = doc;
    }

    Documentation documentation() const
    {
        return m_doc;
    }

private:
    QString m_name;
    QString m_stringValue;

    bool m_valueSet;
    int m_value;

    Documentation m_doc;
};


class AbstractMetaEnumValueList : public QList<AbstractMetaEnumValue *>
{
public:
    AbstractMetaEnumValue *find(const QString &name) const;
};

class AbstractMetaEnum : public AbstractMetaAttributes
{
public:
    AbstractMetaEnum() : m_typeEntry(0), m_class(0), m_hasQenumsDeclaration(false) {}
    ~AbstractMetaEnum()
    {
        qDeleteAll(m_enumValues);
    }

    AbstractMetaEnumValueList values() const
    {
        return m_enumValues;
    }

    void addEnumValue(AbstractMetaEnumValue *enumValue)
    {
        m_enumValues << enumValue;
    }

    QString name() const
    {
        return m_typeEntry->targetLangName();
    }

    QString qualifier() const
    {
        return m_typeEntry->targetLangQualifier();
    }

    QString package() const
    {
        return m_typeEntry->targetLangPackage();
    }

    QString fullName() const
    {
        return package() + "." + qualifier()  + "." + name();
    }

    // Has the enum been declared inside a Q_ENUMS() macro in its enclosing class?
    void setHasQEnumsDeclaration(bool on)
    {
        m_hasQenumsDeclaration = on;
    }

    bool hasQEnumsDeclaration() const
    {
        return m_hasQenumsDeclaration;
    }

    EnumTypeEntry *typeEntry() const
    {
        return m_typeEntry;
    }

    void setTypeEntry(EnumTypeEntry *entry)
    {
        m_typeEntry = entry;
    }

    AbstractMetaClass *enclosingClass() const
    {
        return m_class;
    }

    void setEnclosingClass(AbstractMetaClass *c)
    {
        m_class = c;
    }

    bool isAnonymous() const
    {
        return m_typeEntry->isAnonymous();
    }

private:
    AbstractMetaEnumValueList m_enumValues;
    EnumTypeEntry *m_typeEntry;
    AbstractMetaClass *m_class;

    uint m_hasQenumsDeclaration : 1;
    uint m_reserved : 31;
};

typedef QList<AbstractMetaEnum *> AbstractMetaEnumList;

class AbstractMetaClass : public AbstractMetaAttributes
{
public:
    enum FunctionQueryOption {
        Constructors                 = 0x0000001, // Only constructors
        //Destructors                  = 0x0000002, // Only destructors. Not included in class.
        VirtualFunctions             = 0x0000004, // Only virtual functions (virtual in both TargetLang and C++)
        FinalInTargetLangFunctions   = 0x0000008, // Only functions that are non-virtual in TargetLang
        FinalInCppFunctions          = 0x0000010, // Only functions that are non-virtual in C++
        ClassImplements              = 0x0000020, // Only functions implemented by the current class
        Inconsistent                 = 0x0000040, // Only inconsistent functions (inconsistent virtualness in TargetLang/C++)
        StaticFunctions              = 0x0000080, // Only static functions
        Signals                      = 0x0000100, // Only signals
        NormalFunctions              = 0x0000200, // Only functions that aren't signals
        Visible                      = 0x0000400, // Only public and protected functions
        ForcedShellFunctions         = 0x0000800, // Only functions that are overridden to be implemented in the shell class
        WasPublic                    = 0x0001000, // Only functions that were originally public
        WasProtected                 = 0x0002000, // Only functions that were originally protected
        NonStaticFunctions           = 0x0004000, // No static functions
        Empty                        = 0x0008000, // Empty overrides of abstract functions
        Invisible                    = 0x0010000, // Only private functions
        VirtualInCppFunctions        = 0x0020000, // Only functions that are virtual in C++
        NonEmptyFunctions            = 0x0040000, // Only functions with target language API implementations
        VirtualInTargetLangFunctions = 0x0080000, // Only functions which are virtual in TargetLang
        AbstractFunctions            = 0x0100000, // Only abstract functions
        WasVisible                   = 0x0200000, // Only functions that were public or protected in the original code
        NotRemovedFromTargetLang     = 0x0400000, // Only functions that have not been removed from TargetLang
        NotRemovedFromShell          = 0x0800000, // Only functions that have not been removed from the shell class
        VirtualSlots                 = 0x1000000, // Only functions that are set as virtual slots in the type system
        OperatorOverloads            = 0x2000000  // Only functions that are operator overloads
    };

    enum OperatorQueryOption {
        ArithmeticOp   = 0x01, // Arithmetic: +, -, *, /, %, +=, -=, *=, /=, %=, ++, --, unary+, unary-
        BitwiseOp      = 0x02, // Bitwise: <<, <<=, >>, >>=, ~, &, &=, |, |=, ^, ^=
        ComparisonOp   = 0x04, // Comparison: <, <=, >, >=, !=, ==
        LogicalOp      = 0x08, // Logical: !, &&, ||
        ConversionOp   = 0x10, // Conversion: operator [const] TYPE()
        SubscriptionOp = 0x20, // Subscription: []
        AssignmentOp   = 0x40, // Assignment: =
        OtherOp        = 0x80, // The remaining operators: call(), etc
        AllOperators   = ArithmeticOp | BitwiseOp | ComparisonOp
                        | LogicalOp | ConversionOp | SubscriptionOp
                        | AssignmentOp | OtherOp
    };

    AbstractMetaClass()
            : m_namespace(false),
              m_qobject(false),
              m_hasVirtuals(false),
              m_isPolymorphic(false),
              m_hasNonpublic(false),
              m_hasVirtualSlots(false),
              m_hasNonPrivateConstructor(false),
              m_functionsFixed(false),
              m_hasPrivateDestructor(false),
              m_hasProtectedDestructor(false),
              m_hasVirtualDestructor(false),
              m_forceShellClass(false),
              m_hasHashFunction(false),
              m_hasEqualsOperator(false),
              m_hasCloneOperator(false),
              m_isTypeAlias(false),
              m_hasToStringCapability(false),
              m_enclosingClass(0),
              m_baseClass(0),
              m_templateBaseClass(0),
              m_extractedInterface(0),
              m_primaryInterfaceImplementor(0),
              m_typeEntry(0),
              m_stream(false)
    {
    }

    virtual ~AbstractMetaClass();

    AbstractMetaClass *extractInterface();
    void fixFunctions();

    AbstractMetaFunctionList functions() const
    {
        return m_functions;
    }

    void setFunctions(const AbstractMetaFunctionList &functions);
    void addFunction(AbstractMetaFunction *function);
    bool hasFunction(const AbstractMetaFunction *f) const;
    bool hasFunction(const QString &str) const;
    const AbstractMetaFunction* findFunction(const QString& functionName) const;
    bool hasSignal(const AbstractMetaFunction *f) const;

    bool hasConstructors() const;
    bool hasCopyConstructor() const;
    bool hasPrivateCopyConstructor() const;

    void addDefaultConstructor();
    void addDefaultCopyConstructor(bool isPrivate = false);

    bool hasNonPrivateConstructor() const
    {
        return m_hasNonPrivateConstructor;
    }

    void setHasNonPrivateConstructor(bool value)
    {
        m_hasNonPrivateConstructor = value;
    }

    bool hasPrivateDestructor() const
    {
        return m_hasPrivateDestructor;
    }

    void setHasPrivateDestructor(bool value)
    {
        m_hasPrivateDestructor = value;
    }

    bool hasProtectedDestructor() const
    {
        return m_hasProtectedDestructor;
    }

    void setHasProtectedDestructor(bool value)
    {
        m_hasProtectedDestructor = value;
    }

    bool hasVirtualDestructor() const
    {
        return m_hasVirtualDestructor;
    }

    void setHasVirtualDestructor(bool value)
    {
        m_hasVirtualDestructor = value;
    }

    AbstractMetaFunctionList queryFunctionsByName(const QString &name) const;
    AbstractMetaFunctionList queryFunctions(uint query) const;
    inline AbstractMetaFunctionList allVirtualFunctions() const;
    inline AbstractMetaFunctionList allFinalFunctions() const;
    AbstractMetaFunctionList functionsInTargetLang() const;
    AbstractMetaFunctionList functionsInShellClass() const;
    inline AbstractMetaFunctionList cppInconsistentFunctions() const;
    inline AbstractMetaFunctionList cppSignalFunctions() const;
    AbstractMetaFunctionList publicOverrideFunctions() const;
    AbstractMetaFunctionList virtualOverrideFunctions() const;
    AbstractMetaFunctionList virtualFunctions() const;
    AbstractMetaFunctionList nonVirtualShellFunctions() const;
    AbstractMetaFunctionList implicitConversions() const;

    /**
     *   Retrieves all class' operator overloads that meet
     *   query criteria defined with the OperatorQueryOption
     *   enum.
     *   /param query composition of OperatorQueryOption enum values
     *   /return list of operator overload methods that meet the
     *   query criteria
     */
    AbstractMetaFunctionList operatorOverloads(uint query = AllOperators) const;

    bool hasOperatorOverload() const;
    bool hasArithmeticOperatorOverload() const;
    bool hasBitwiseOperatorOverload() const;
    bool hasComparisonOperatorOverload() const;
    bool hasLogicalOperatorOverload() const;
    bool hasSubscriptOperatorOverload() const;
    bool hasAssignmentOperatorOverload() const;
    bool hasConversionOperatorOverload() const;

    AbstractMetaFieldList fields() const
    {
        return m_fields;
    }

    void setFields(const AbstractMetaFieldList &fields)
    {
        m_fields = fields;
    }

    void addField(AbstractMetaField *field)
    {
        m_fields << field;
    }

    AbstractMetaEnumList enums() const
    {
        return m_enums;
    }
    void setEnums(const AbstractMetaEnumList &enums)
    {
        m_enums = enums;
    }

    void addEnum(AbstractMetaEnum *e)
    {
        m_enums << e;
    }

    AbstractMetaEnum *findEnum(const QString &enumName);
    AbstractMetaEnum *findEnumForValue(const QString &enumName);
    AbstractMetaEnumValue *findEnumValue(const QString &enumName, AbstractMetaEnum *meta_enum);

    AbstractMetaClassList interfaces() const
    {
        return m_interfaces;
    }
    void addInterface(AbstractMetaClass *interface);
    void setInterfaces(const AbstractMetaClassList &interface);

    QString fullName() const
    {
        return package() + "." + name();
    }

    /**
     *   Retrieves the class name without any namespace/scope information.
     *   /return the class name without scope information
     */
    QString name() const;

    QString baseClassName() const
    {
        return m_baseClass ? m_baseClass->name() : QString();
    }

    AbstractMetaClass *baseClass() const
    {
        return m_baseClass;
    }

    void setBaseClass(AbstractMetaClass *base_class);

    const AbstractMetaClass *enclosingClass() const
    {
        return m_enclosingClass;
    }

    void setEnclosingClass(AbstractMetaClass *cl)
    {
        m_enclosingClass = cl;
    }

    const AbstractMetaClassList& innerClasses() const
    {
        return m_innerClasses;
    }

    void addInnerClass(AbstractMetaClass* cl)
    {
        m_innerClasses << cl;
    }

    void setInnerClasses(AbstractMetaClassList innerClasses)
    {
        m_innerClasses = innerClasses;
    }

    QString package() const
    {
        return m_typeEntry->targetLangPackage();
    }

    bool isInterface() const
    {
        return m_typeEntry->isInterface();
    }

    bool isNamespace() const
    {
        return m_typeEntry->isNamespace();
    }

    bool isQObject() const
    {
        return m_typeEntry->isQObject();
    }

    bool isQtNamespace() const
    {
        return isNamespace() && name() == "Qt";
    }

    QString qualifiedCppName() const
    {
        return m_typeEntry->qualifiedCppName();
    }

    bool hasInconsistentFunctions() const;
    bool hasSignals() const;
    bool inheritsFrom(const AbstractMetaClass *other) const;

    void setForceShellClass(bool on)
    {
        m_forceShellClass = on;
    }

    bool generateShellClass() const;

    bool hasVirtualSlots() const
    {
        return m_hasVirtualSlots;
    }

    /**
    *   Says if a class has any virtual functions of its own.
    *   \return true if the class implements any virtual methods
    */
    bool hasVirtualFunctions() const
    {
        return !isFinal() && m_hasVirtuals;
    }
    /**
    *   Says if the class that declares or inherits a virtual function.
    *   \return true if the class implements or inherits any virtual methods
    */
    bool isPolymorphic() const
    {
        return m_isPolymorphic;
    }

    /**
     * Tells if this class has one or more functions that are protected.
     * \return true if the class has protected functions.
     */
    bool hasProtectedFunctions() const;

    /**
     * Tells if this class has one or more fields (member variables) that are protected.
     * \return true if the class has protected fields.
     */
    bool hasProtectedFields() const;

    /**
     * Tells if this class has one or more members (functions or fields) that are protected.
     * \return true if the class has protected members.
     */
    bool hasProtectedMembers() const;


    QList<TypeEntry *> templateArguments() const
    {
        return m_templateArgs;
    }

    void setTemplateArguments(const QList<TypeEntry *> &args)
    {
        m_templateArgs = args;
    }

    bool hasFieldAccessors() const;

    // only valid during metabuilder's run
    QStringList baseClassNames() const
    {
        return m_baseClassNames;
    }

    void setBaseClassNames(const QStringList &names)
    {
        m_baseClassNames = names;
    }

    AbstractMetaClass *primaryInterfaceImplementor() const
    {
        return m_primaryInterfaceImplementor;
    }

    void setPrimaryInterfaceImplementor(AbstractMetaClass *cl)
    {
        m_primaryInterfaceImplementor = cl;
    }

    const ComplexTypeEntry *typeEntry() const
    {
        return m_typeEntry;
    }

    ComplexTypeEntry *typeEntry()
    {
        return m_typeEntry;
    }

    void setTypeEntry(ComplexTypeEntry *type)
    {
        m_typeEntry = type;
    }

    void setHasHashFunction(bool on)
    {
        m_hasHashFunction = on;
    }

    bool hasHashFunction() const
    {
        return m_hasHashFunction;
    }
    virtual bool hasDefaultToStringFunction() const;

    void setHasEqualsOperator(bool on)
    {
        m_hasEqualsOperator = on;
    }

    bool hasEqualsOperator() const
    {
        return m_hasEqualsOperator;
    }

    void setHasCloneOperator(bool on)
    {
        m_hasCloneOperator = on;
    }

    bool hasCloneOperator() const
    {
        return m_hasCloneOperator;
    }

    void addPropertySpec(QPropertySpec *spec)
    {
        m_propertySpecs << spec;
    }

    QList<QPropertySpec *> propertySpecs() const
    {
        return m_propertySpecs;
    }

    QPropertySpec *propertySpecForRead(const QString &name) const;
    QPropertySpec *propertySpecForWrite(const QString &name) const;
    QPropertySpec *propertySpecForReset(const QString &name) const;

    QList<ReferenceCount> referenceCounts() const;

    void setEqualsFunctions(const AbstractMetaFunctionList &lst)
    {
        m_equalsFunctions = lst;
    }

    AbstractMetaFunctionList equalsFunctions() const
    {
        return m_equalsFunctions;
    }

    void setNotEqualsFunctions(const AbstractMetaFunctionList &lst)
    {
        m_nequalsFunctions = lst;
    }

    AbstractMetaFunctionList notEqualsFunctions() const
    {
        return m_nequalsFunctions;
    }

    void setLessThanFunctions(const AbstractMetaFunctionList &lst)
    {
        m_lessThanFunctions = lst;
    }

    AbstractMetaFunctionList lessThanFunctions() const
    {
        return m_lessThanFunctions;
    }

    void setGreaterThanFunctions(const AbstractMetaFunctionList &lst)
    {
        m_greaterThanFunctions = lst;
    }

    AbstractMetaFunctionList greaterThanFunctions() const
    {
        return m_greaterThanFunctions;
    }

    void setLessThanEqFunctions(const AbstractMetaFunctionList &lst)
    {
        m_lessThanEqFunctions = lst;
    }

    AbstractMetaFunctionList lessThanEqFunctions() const
    {
        return m_lessThanEqFunctions;
    }

    void setGreaterThanEqFunctions(const AbstractMetaFunctionList &lst)
    {
        m_greaterThanEqFunctions = lst;
    }

    AbstractMetaFunctionList greaterThanEqFunctions() const
    {
        return m_greaterThanEqFunctions;
    }

    /// Returns a list of conversion operators for this class. The conversion operators are defined in other classes of the same module.
    AbstractMetaFunctionList externalConversionOperators() const
    {
        return m_externalConversionOperators;
    }
    /// Adds a converter operator for this class.
    void addExternalConversionOperator(AbstractMetaFunction* conversionOp)
    {
        if (!m_externalConversionOperators.contains(conversionOp))
            m_externalConversionOperators.append(conversionOp);
    }
    /// Returns true if this class has any converter operators defined elsewhere.
    bool hasExternalConversionOperators() const
    {
        return !m_externalConversionOperators.isEmpty();
    }

    void sortFunctions();

    const AbstractMetaClass *templateBaseClass() const
    {
        return m_templateBaseClass;
    }

    void setTemplateBaseClass(const AbstractMetaClass *cls)
    {
        m_templateBaseClass = cls;
    }

    bool hasTemplateBaseClassInstantiations() const;
    AbstractMetaTypeList templateBaseClassInstantiations() const;
    void setTemplateBaseClassInstantiations(AbstractMetaTypeList& instantiations);

    void setTypeAlias(bool typeAlias)
    {
        m_isTypeAlias = typeAlias;
    }

    bool isTypeAlias() const
    {
        return m_isTypeAlias;
    }

    void setStream(bool stream)
    {
        m_stream = stream;
    }

    bool isStream() const
    {
        return m_stream;
    }

    void setToStringCapability(bool value)
    {
        m_hasToStringCapability = value;
    }

    bool hasToStringCapability() const
    {
        return m_hasToStringCapability;
    }
private:
    uint m_namespace : 1;
    uint m_qobject : 1;
    uint m_hasVirtuals : 1;
    uint m_isPolymorphic : 1;
    uint m_hasNonpublic : 1;
    uint m_hasVirtualSlots : 1;
    uint m_hasNonPrivateConstructor : 1;
    uint m_functionsFixed : 1;
    uint m_hasPrivateDestructor : 1;
    uint m_hasProtectedDestructor : 1;
    uint m_hasVirtualDestructor : 1;
    uint m_forceShellClass : 1;
    uint m_hasHashFunction : 1;
    uint m_hasEqualsOperator : 1;
    uint m_hasCloneOperator : 1;
    uint m_isTypeAlias : 1;
    uint m_hasToStringCapability : 1;
    uint m_reserved : 17;

    const AbstractMetaClass *m_enclosingClass;
    AbstractMetaClass *m_baseClass;
    const AbstractMetaClass *m_templateBaseClass;
    AbstractMetaFunctionList m_functions;
    AbstractMetaFieldList m_fields;
    AbstractMetaEnumList m_enums;
    AbstractMetaClassList m_interfaces;
    AbstractMetaClassList m_orphanInterfaces;
    AbstractMetaClass *m_extractedInterface;
    AbstractMetaClass *m_primaryInterfaceImplementor;
    QList<QPropertySpec *> m_propertySpecs;
    AbstractMetaFunctionList m_equalsFunctions;
    AbstractMetaFunctionList m_nequalsFunctions;
    AbstractMetaClassList m_innerClasses;

    AbstractMetaFunctionList m_lessThanFunctions;
    AbstractMetaFunctionList m_greaterThanFunctions;
    AbstractMetaFunctionList m_lessThanEqFunctions;
    AbstractMetaFunctionList m_greaterThanEqFunctions;

    AbstractMetaFunctionList m_externalConversionOperators;

    QStringList m_baseClassNames;
    QList<TypeEntry *> m_templateArgs;
    ComplexTypeEntry *m_typeEntry;
//     FunctionModelItem m_qDebugStreamFunction;

    bool m_stream;
    static int m_count;
};

class QPropertySpec
{
public:
    QPropertySpec(const TypeEntry *type)
            : m_type(type),
            m_index(-1)
    {}

    const TypeEntry *type() const
    {
        return m_type;
    }

    QString name() const
    {
        return m_name;
    }

    void setName(const QString &name)
    {
        m_name = name;
    }

    QString read() const
    {
        return m_read;
    }

    void setRead(const QString &read)
    {
        m_read = read;
    }

    QString write() const
    {
        return m_write;
    }

    void setWrite(const QString &write)
    {
        m_write = write;
    }

    QString designable() const
    {
        return m_designable;
    }

    void setDesignable(const QString &designable)
    {
        m_designable = designable;
    }

    QString reset() const
    {
        return m_reset;
    }

    void setReset(const QString &reset)
    {
        m_reset = reset;
    }

    int index() const
    {
        return m_index;
    }

    void setIndex(int index)
    {
        m_index = index;
    }

private:
    QString m_name;
    QString m_read;
    QString m_write;
    QString m_designable;
    QString m_reset;
    const TypeEntry *m_type;
    int m_index;
};

inline AbstractMetaFunctionList AbstractMetaClass::allVirtualFunctions() const
{
    return queryFunctions(VirtualFunctions | NotRemovedFromTargetLang);
}

inline AbstractMetaFunctionList AbstractMetaClass::allFinalFunctions() const
{
    return queryFunctions(FinalInTargetLangFunctions
                          | FinalInCppFunctions
                          | NotRemovedFromTargetLang);
}

inline AbstractMetaFunctionList AbstractMetaClass::cppInconsistentFunctions() const
{
    return queryFunctions(Inconsistent
                          | NormalFunctions
                          | Visible
                          | NotRemovedFromTargetLang);
}

inline AbstractMetaFunctionList AbstractMetaClass::cppSignalFunctions() const
{
    return queryFunctions(Signals
                          | Visible
                          | NotRemovedFromTargetLang);
}

#endif // ABSTRACTMETALANG_H
