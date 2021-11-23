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

#ifndef TYPESYSTEM_H
#define TYPESYSTEM_H

#include <QtCore/QHash>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QMap>
#include <QtCore/QDebug>
#include "include.h"

//Used to identify the conversion rule to avoid break API
#define TARGET_CONVERSION_RULE_FLAG "0"
#define NATIVE_CONVERSION_RULE_FLAG "1"

class Indentor;

class AbstractMetaType;
class QTextStream;

class EnumTypeEntry;
class FlagsTypeEntry;

typedef QMap<int, QString> ArgumentMap;

class TemplateInstance;

namespace TypeSystem
{
enum Language {
    NoLanguage          = 0x0000,
    TargetLangCode      = 0x0001,
    NativeCode          = 0x0002,
    ShellCode           = 0x0004,
    ShellDeclaration    = 0x0008,
    PackageInitializer  = 0x0010,
    DestructorFunction  = 0x0020,
    Constructors        = 0x0040,
    Interface           = 0x0080,

    // masks
    All                 = TargetLangCode
    | NativeCode
    | ShellCode
    | ShellDeclaration
    | PackageInitializer
    | Constructors
    | Interface
    | DestructorFunction,

    TargetLangAndNativeCode   = TargetLangCode | NativeCode
};

enum Ownership {
    InvalidOwnership,
    DefaultOwnership,
    TargetLangOwnership,
    CppOwnership
};
};

struct ReferenceCount
{
    ReferenceCount()  {}
    enum Action { // 0x01 - 0xff
        Invalid     = 0x00,
        Add         = 0x01,
        AddAll      = 0x02,
        Remove      = 0x04,
        Set         = 0x08,
        Ignore      = 0x10,

        ActionsMask = 0xff,

        Padding     = 0xffffffff
    };

    Action action;
    QString varName;
};

struct ArgumentOwner
{
    enum Action {
        Invalid     = 0x00,
        Add         = 0x01,
        Remove      = 0x02
    };
    enum {
        InvalidIndex = -2,
        ThisIndex = -1,
        ReturnIndex = 0,
        FirstArgumentIndex = 1
    };
    ArgumentOwner() : action(ArgumentOwner::Invalid), index(ArgumentOwner::InvalidIndex) {}

    Action action;
    int index;
};

class CodeSnipFragment
{
private:
    QString m_code;
    TemplateInstance *m_instance;

public:
    CodeSnipFragment(const QString &code)
        : m_code(code),
          m_instance(0) {}

    CodeSnipFragment(TemplateInstance *instance)
        : m_instance(instance) {}

    QString code() const;
};

class CodeSnipAbstract
{
public:
    QString code() const;

    void addCode(const QString &code)
    {
        codeList.append(CodeSnipFragment(code));
    }

    void addTemplateInstance(TemplateInstance *ti)
    {
        codeList.append(CodeSnipFragment(ti));
    }

    QList<CodeSnipFragment> codeList;
};

class CustomFunction : public CodeSnipAbstract
{
public:
    CustomFunction(const QString &n = QString()) : name(n) { }

    QString name;
    QString paramName;
};

class TemplateEntry : public CodeSnipAbstract
{
public:
    TemplateEntry(const QString &name, double vr)
            : m_name(name), m_version(vr)
    {
    };

    QString name() const
    {
        return m_name;
    };

    double version() const
    {
        return m_version;
    }

private:
    QString m_name;
    double m_version;
};

typedef QHash<QString, TemplateEntry *> TemplateEntryHash;

class TemplateInstance
{
public:
    TemplateInstance(const QString &name, double vr)
            : m_name(name), m_version(vr) {}

    void addReplaceRule(const QString &name, const QString &value)
    {
        replaceRules[name] = value;
    }

    QString expandCode() const;

    QString name() const
    {
        return m_name;
    }

    double version() const
    {
        return m_version;
    }

private:
    const QString m_name;
    double m_version;
    QHash<QString, QString> replaceRules;
};


class CodeSnip : public CodeSnipAbstract
{
public:
    enum Position {
        Beginning,
        End,
        AfterThis,
        // QtScript
        Declaration,
        PrototypeInitialization,
        ConstructorInitialization,
        Constructor,
        Any
    };

    CodeSnip(double vr) : language(TypeSystem::TargetLangCode), version(vr) { }
    CodeSnip(double vr, TypeSystem::Language lang) : language(lang), version(vr) { }

    TypeSystem::Language language;
    Position position;
    ArgumentMap argumentMap;
    double version;
};
typedef QList<CodeSnip> CodeSnipList;

struct ArgumentModification
{
    ArgumentModification(int idx, double vr)
            : removedDefaultExpression(false), removed(false),
              noNullPointers(false), index(idx), version(vr) {}

    // Should the default expression be removed?
    uint removedDefaultExpression : 1;
    uint removed : 1;
    uint noNullPointers : 1;
    uint resetAfterUse : 1;

    // The index of this argument
    int index;

    // Reference count flags for this argument
    QList<ReferenceCount> referenceCounts;

    // The text given for the new type of the argument
    QString modified_type;

    QString replace_value;

    // The code to be used to construct a return value when noNullPointers is true and
    // the returned value is null. If noNullPointers is true and this string is
    // empty, then the base class implementation will be used (or a default construction
    // if there is no implementation)
    QString nullPointerDefaultValue;

    // The text of the new default expression of the argument
    QString replacedDefaultExpression;

    // The new definition of ownership for a specific argument
    QHash<TypeSystem::Language, TypeSystem::Ownership> ownerships;

    // Different conversion rules
    CodeSnipList conversion_rules;

    //QObject parent(owner) of this argument
    ArgumentOwner owner;

    //Api version
    double version;

    //New name
    QString renamed_to;
};

struct Modification
{
    enum Modifiers {
        Private =               0x0001,
        Protected =             0x0002,
        Public =                0x0003,
        Friendly =              0x0004,
        AccessModifierMask =    0x000f,

        Final =                 0x0010,
        NonFinal =              0x0020,
        FinalMask =             Final | NonFinal,

        Readable =              0x0100,
        Writable =              0x0200,

        CodeInjection =         0x1000,
        Rename =                0x2000,
        Deprecated =            0x4000,
        ReplaceExpression =     0x8000,
        VirtualSlot =          0x10000 | NonFinal
    };

    Modification() : modifiers(0), removal(TypeSystem::NoLanguage) { }

    bool isAccessModifier() const
    {
        return modifiers & AccessModifierMask;
    }
    Modifiers accessModifier() const
    {
        return Modifiers(modifiers & AccessModifierMask);
    }
    bool isPrivate() const
    {
        return accessModifier() == Private;
    }
    bool isProtected() const
    {
        return accessModifier() == Protected;
    }
    bool isPublic() const
    {
        return accessModifier() == Public;
    }
    bool isFriendly() const
    {
        return accessModifier() == Friendly;
    }
    bool isFinal() const
    {
        return modifiers & Final;
    }
    bool isNonFinal() const
    {
        return modifiers & NonFinal;
    }
    bool isVirtualSlot() const
    {
        return (modifiers & VirtualSlot) == VirtualSlot;
    }
    QString accessModifierString() const;

    bool isDeprecated() const
    {
        return modifiers & Deprecated;
    }

    void setRenamedTo(const QString &name)
    {
        renamedToName = name;
    }
    QString renamedTo() const
    {
        return renamedToName;
    }
    bool isRenameModifier() const
    {
        return modifiers & Rename;
    }

    bool isRemoveModifier() const
    {
        return removal != TypeSystem::NoLanguage;
    }

    uint modifiers;
    QString renamedToName;
    TypeSystem::Language removal;
};

struct FunctionModification: public Modification
{
    FunctionModification(double vr) : m_thread(false), m_allowThread(false), m_version(vr) {}

    bool isCodeInjection() const
    {
        return modifiers & CodeInjection;
    }
    void setIsThread(bool flag)
    {
        m_thread = flag;
    }
    bool isThread() const
    {
        return m_thread;
    }
    bool allowThread() const
    {
        return m_allowThread;
    }
    void setAllowThread(bool allow)
    {
        m_allowThread = allow;
    }
    double version() const
    {
        return m_version;
    }

    bool operator!=(const FunctionModification& other) const;
    bool operator==(const FunctionModification& other) const;


    QString toString() const;

    QString signature;
    QString association;
    CodeSnipList snips;

    QList<ArgumentModification> argument_mods;

private:
    FunctionModification() {}

    bool m_thread;
    bool m_allowThread;
    double m_version;


};
typedef QList<FunctionModification> FunctionModificationList;

struct FieldModification: public Modification
{
    bool isReadable() const
    {
        return modifiers & Readable;
    }
    bool isWritable() const
    {
        return modifiers & Writable;
    }

    QString name;
};

typedef QList<FieldModification> FieldModificationList;

/**
*   \internal
*   Struct used to store information about functions added by the typesystem.
*   This info will be used later to create a fake AbstractMetaFunction which
*   will be inserted into the right AbstractMetaClass.
*/
struct AddedFunction
{
    /// Function access types.
    enum Access {
        Protected = 0x1,
        Public =    0x2
    };

    /**
    *   \internal
    *   Internal struct used to store information about arguments and return type of the
    *   functions added by the type system. This information is later used to create
    *   AbstractMetaType and AbstractMetaArgument for the AbstractMetaFunctions.
    */
    struct TypeInfo {
        TypeInfo() : isConstant(false), indirections(0), isReference(false) {}
        static TypeInfo fromSignature(const QString& signature);

        QString name;
        bool isConstant;
        int indirections;
        bool isReference;
        QString defaultValue;
    };

    /// Creates a new AddedFunction with a signature and a return type.
    AddedFunction(QString signature, QString returnType, double vr);

    /// Returns the function name.
    QString name() const
    {
        return m_name;
    }

    /// Set the function access type.
    void setAccess(Access access)
    {
        m_access = access;
    }

    /// Returns the function access type.
    Access access() const
    {
        return m_access;
    }

    /// Returns the function return type.
    TypeInfo returnType() const
    {
        return m_returnType;
    }

    /// Returns a list of argument type infos.
    QList<TypeInfo> arguments() const
    {
        return m_arguments;
    }

    /// Returns true if this is a constant method.
    bool isConstant() const
    {
        return m_isConst;
    }

    /// Set this method static.
    void setStatic(bool value)
    {
        m_isStatic = value;
    }

    /// Returns true if this is a static method.
    bool isStatic() const
    {
        return m_isStatic;
    }

    double version() const
    {
        return m_version;
    }
private:
    QString m_name;
    Access m_access;
    QList<TypeInfo> m_arguments;
    TypeInfo m_returnType;
    bool m_isConst;
    bool m_isStatic;
    double m_version;
};
typedef QList<AddedFunction> AddedFunctionList;

struct ExpensePolicy
{
    ExpensePolicy() : limit(-1) {}
    int limit;
    QString cost;
    bool isValid() const
    {
        return limit >= 0;
    }
};

class InterfaceTypeEntry;
class ObjectTypeEntry;

class DocModification
{
public:
    enum Mode {
        Append,
        Prepend,
        Replace,
        XPathReplace
    };

    DocModification(const QString& xpath, const QString& signature, double vr)
            : format(TypeSystem::NativeCode), m_mode(XPathReplace),
              m_xpath(xpath), m_signature(signature), m_version(vr) {}
    DocModification(Mode mode, const QString& signature, double vr)
            : m_mode(mode), m_signature(signature), m_version(vr) {}

    void setCode(const QString& code)
    {
        m_code = code;
    }
    QString code() const
    {
        return m_code;
    }
    QString xpath() const
    {
        return m_xpath;
    }
    QString signature() const
    {
        return m_signature;
    }
    Mode mode() const
    {
        return m_mode;
    }
    double version() const
    {
        return m_version;
    }

    TypeSystem::Language format;

private:
    Mode m_mode;
    QString m_code;
    QString m_xpath;
    QString m_signature;
    double m_version;
};

typedef QList<DocModification> DocModificationList;

class CustomConversion;

class TypeEntry
{
public:
    enum Type {
        PrimitiveType,
        VoidType,
        VarargsType,
        FlagsType,
        EnumType,
        EnumValue,
        TemplateArgumentType,
        ThreadType,
        BasicValueType,
        StringType,
        ContainerType,
        InterfaceType,
        ObjectType,
        NamespaceType,
        VariantType,
        JObjectWrapperType,
        CharType,
        ArrayType,
        TypeSystemType,
        CustomType,
        TargetLangType,
        FunctionType
    };

    enum CodeGeneration {
        GenerateTargetLang      = 0x0001,
        GenerateCpp             = 0x0002,
        GenerateForSubclass     = 0x0004,

        GenerateNothing         = 0,
        GenerateAll             = 0xffff,
        GenerateCode            = GenerateTargetLang | GenerateCpp
    };

    TypeEntry(const QString &name, Type t, double vr)
            : m_name(name),
              m_type(t),
              m_codeGeneration(GenerateAll),
              m_preferredConversion(true),
              m_stream(false),
              m_version(vr)
    {
    };

    virtual ~TypeEntry();

    Type type() const
    {
        return m_type;
    }
    bool isPrimitive() const
    {
        return m_type == PrimitiveType;
    }
    bool isEnum() const
    {
        return m_type == EnumType;
    }
    bool isFlags() const
    {
        return m_type == FlagsType;
    }
    bool isInterface() const
    {
        return m_type == InterfaceType;
    }
    bool isObject() const
    {
        return m_type == ObjectType;
    }
    bool isString() const
    {
        return m_type == StringType;
    }
    bool isChar() const
    {
        return m_type == CharType;
    }
    bool isNamespace() const
    {
        return m_type == NamespaceType;
    }
    bool isContainer() const
    {
        return m_type == ContainerType;
    }
    bool isVariant() const
    {
        return m_type == VariantType;
    }
    bool isJObjectWrapper() const
    {
        return m_type == JObjectWrapperType;
    }
    bool isArray() const
    {
        return m_type == ArrayType;
    }
    bool isTemplateArgument() const
    {
        return m_type == TemplateArgumentType;
    }
    bool isVoid() const
    {
        return m_type == VoidType;
    }
    bool isVarargs() const
    {
        return m_type == VarargsType;
    }
    bool isThread() const
    {
        return m_type == ThreadType;
    }
    bool isCustom() const
    {
        return m_type == CustomType;
    }
    bool isBasicValue() const
    {
        return m_type == BasicValueType;
    }
    bool isTypeSystem() const
    {
        return m_type == TypeSystemType;
    }
    bool isFunction() const
    {
        return m_type == FunctionType;
    }
    bool isEnumValue() const
    {
        return m_type == EnumValue;
    }

    virtual bool preferredConversion() const
    {
        return m_preferredConversion;
    }
    virtual void setPreferredConversion(bool b)
    {
        m_preferredConversion = b;
    }

    bool stream() const
    {
        return m_stream;
    }

    void setStream(bool b)
    {
        m_stream = b;
    }

    // The type's name in C++, fully qualified
    QString name() const
    {
        return m_name;
    }

    uint codeGeneration() const
    {
        return m_codeGeneration;
    }
    void setCodeGeneration(uint cg)
    {
        m_codeGeneration = cg;
    }

    // Returns true if code must be generated for this entry,
    // it will return false in case of types coming from typesystems
    // included for reference only.
    // NOTE: 'GenerateForSubclass' means 'generate="no"'
    //       on 'load-typesystem' tag
    inline bool generateCode() const
    {
        return m_codeGeneration != TypeEntry::GenerateForSubclass
               && m_codeGeneration != TypeEntry::GenerateNothing;
    }

    virtual QString qualifiedCppName() const
    {
        return m_name;
    }

    /**
     *   Its type's name in target language API
     *   The target language API name represents how this type is
     *   referred on low level code for the target language.
     *   Examples: for Java this would be a JNI name, for Python
     *   it should represent the CPython type name.
     *   /return string representing the target language API name
     *   for this type entry
     */
    virtual QString targetLangApiName() const
    {
        return m_name;
    }

    // The type's name in TargetLang
    virtual QString targetLangName() const
    {
        return m_name;
    }

    // The type to lookup when converting to TargetLang
    virtual QString lookupName() const
    {
        return targetLangName();
    }

    // The package
    virtual QString targetLangPackage() const
    {
        return QString();
    }

    virtual QString qualifiedTargetLangName() const
    {
        QString pkg = targetLangPackage();
        if (pkg.isEmpty())
            return targetLangName();
        return pkg + '.' + targetLangName();
    }

    virtual InterfaceTypeEntry *designatedInterface() const
    {
        return 0;
    }

    void setCustomConstructor(const CustomFunction &func)
    {
        m_customConstructor = func;
    }
    CustomFunction customConstructor() const
    {
        return m_customConstructor;
    }

    void setCustomDestructor(const CustomFunction &func)
    {
        m_customDestructor = func;
    }
    CustomFunction customDestructor() const
    {
        return m_customDestructor;
    }

    virtual bool isValue() const
    {
        return false;
    }
    virtual bool isComplex() const
    {
        return false;
    }

    virtual bool isNativeIdBased() const
    {
        return false;
    }

    CodeSnipList codeSnips() const;
    void setCodeSnips(const CodeSnipList &codeSnips)
    {
        m_codeSnips = codeSnips;
    }
    void addCodeSnip(const CodeSnip &codeSnip)
    {
        m_codeSnips << codeSnip;
    }

    void setDocModification(const DocModificationList& docMods)
    {
        m_docModifications << docMods;
    }
    DocModificationList docModifications() const
    {
        return m_docModifications;
    }

    IncludeList extraIncludes() const
    {
        return m_extraIncludes;
    }
    void setExtraIncludes(const IncludeList &includes)
    {
        m_extraIncludes = includes;
    }
    void addExtraInclude(const Include &include)
    {
        if (!m_includesUsed.value(include.name(), false)) {
            m_extraIncludes << include;
            m_includesUsed[include.name()] = true;
        }
    }

    Include include() const
    {
        return m_include;
    }
    void setInclude(const Include &inc)
    {
        m_include = inc;
    }

    // Replace conversionRule arg to CodeSnip in future version
    /// Set the type convertion rule
    void setConversionRule(const QString& conversionRule)
    {
        m_conversionRule = conversionRule;
    }

    /// Returns the type convertion rule
    QString conversionRule() const
    {
        //skip conversions flag
        return m_conversionRule.mid(1);
    }

    /// Returns true if there are any conversiton rule for this type, false otherwise.
    bool hasConversionRule() const
    {
        return !m_conversionRule.isEmpty();
    }

    double version() const
    {
        return m_version;
    }

    /// TODO-CONVERTER: mark as deprecated
    bool hasNativeConversionRule() const
    {
        return m_conversionRule.startsWith(NATIVE_CONVERSION_RULE_FLAG);
    }

    /// TODO-CONVERTER: mark as deprecated
    bool hasTargetConversionRule() const
    {
        return m_conversionRule.startsWith(TARGET_CONVERSION_RULE_FLAG);
    }

    bool isCppPrimitive() const;

    bool hasCustomConversion() const;
    void setCustomConversion(CustomConversion* customConversion);
    CustomConversion* customConversion() const;
private:
    QString m_name;
    Type m_type;
    uint m_codeGeneration;
    CustomFunction m_customConstructor;
    CustomFunction m_customDestructor;
    bool m_preferredConversion;
    CodeSnipList m_codeSnips;
    DocModificationList m_docModifications;
    IncludeList m_extraIncludes;
    Include m_include;
    QHash<QString, bool> m_includesUsed;
    QString m_conversionRule;
    bool m_stream;
    double m_version;
};
typedef QHash<QString, QList<TypeEntry *> > TypeEntryHash;
typedef QHash<QString, TypeEntry *> SingleTypeEntryHash;


class TypeSystemTypeEntry : public TypeEntry
{
public:
    TypeSystemTypeEntry(const QString &name, double vr)
            : TypeEntry(name, TypeSystemType, vr)
    {
    };
};

class VoidTypeEntry : public TypeEntry
{
public:
    VoidTypeEntry() : TypeEntry("void", VoidType, 0) { }
};

class VarargsTypeEntry : public TypeEntry
{
public:
    VarargsTypeEntry() : TypeEntry("...", VarargsType, 0) { }
};

class TemplateArgumentEntry : public TypeEntry
{
public:
    TemplateArgumentEntry(const QString &name, double vr)
            : TypeEntry(name, TemplateArgumentType, vr), m_ordinal(0)
    {
    }

    int ordinal() const
    {
        return m_ordinal;
    }
    void setOrdinal(int o)
    {
        m_ordinal = o;
    }

private:
    int m_ordinal;
};

class ArrayTypeEntry : public TypeEntry
{
public:
    ArrayTypeEntry(const TypeEntry *nested_type, double vr)
            : TypeEntry("Array", ArrayType, vr), m_nestedType(nested_type)
    {
        Q_ASSERT(m_nestedType);
    }

    void setNestedTypeEntry(TypeEntry *nested)
    {
        m_nestedType = nested;
    }
    const TypeEntry *nestedTypeEntry() const
    {
        return m_nestedType;
    }

    QString targetLangName() const
    {
        return m_nestedType->targetLangName() + "[]";
    }
    QString targetLangApiName() const
    {
        if (m_nestedType->isPrimitive())
            return m_nestedType->targetLangApiName() + "Array";
        else
            return "jobjectArray";
    }

private:
    const TypeEntry *m_nestedType;
};


class PrimitiveTypeEntry : public TypeEntry
{
public:
    PrimitiveTypeEntry(const QString &name, double vr)
            : TypeEntry(name, PrimitiveType, vr),
              m_preferredConversion(true),
              m_preferredTargetLangType(true),
              m_aliasedTypeEntry(0)
    {
    }

    QString targetLangName() const
    {
        return m_targetLangName;
    }
    void setTargetLangName(const QString &targetLangName)
    {
        m_targetLangName  = targetLangName;
    }

    QString targetLangApiName() const
    {
        return m_targetLangApiName;
    }
    void setTargetLangApiName(const QString &targetLangApiName)
    {
        m_targetLangApiName = targetLangApiName;
    }

    QString defaultConstructor() const
    {
        return m_defaultConstructor;
    }
    void setDefaultConstructor(const QString& defaultConstructor)
    {
        m_defaultConstructor = defaultConstructor;
    }
    bool hasDefaultConstructor() const
    {
        return !m_defaultConstructor.isEmpty();
    }

    /**
     *   The PrimitiveTypeEntry pointed by this type entry if it
     *   represents an alias (i.e. a typedef).
     *   /return the type pointed by the alias, or a null pointer
     *   if the current object is not an alias
     */
    PrimitiveTypeEntry* aliasedTypeEntry() const { return m_aliasedTypeEntry; }

    /**
     *   Defines type aliased by this entry.
     *   /param aliasedTypeEntry type aliased by this entry
     */
    void setAliasedTypeEntry(PrimitiveTypeEntry* aliasedTypeEntry)
    {
        m_aliasedTypeEntry = aliasedTypeEntry;
    }

    /**
     *   Finds the most basic primitive type that the typedef represents,
     *   i.e. a type that is not an alias.
     *   /return the most basic non-aliased primitive type represented
     *   by this typedef
     */
    PrimitiveTypeEntry* basicAliasedTypeEntry() const;

    virtual bool preferredConversion() const
    {
        return m_preferredConversion;
    }
    virtual void setPreferredConversion(bool b)
    {
        m_preferredConversion = b;
    }

    virtual bool preferredTargetLangType() const
    {
        return m_preferredTargetLangType;
    }
    virtual void setPreferredTargetLangType(bool b)
    {
        m_preferredTargetLangType = b;
    }

    void setTargetLangPackage(const QString& package);
    QString targetLangPackage() const;
private:
    QString m_targetLangName;
    QString m_targetLangApiName;
    QString m_defaultConstructor;
    uint m_preferredConversion : 1;
    uint m_preferredTargetLangType : 1;
    PrimitiveTypeEntry* m_aliasedTypeEntry;
};

typedef QList<const PrimitiveTypeEntry*> PrimitiveTypeEntryList;

struct EnumValueRedirection
{
    EnumValueRedirection(const QString &rej, const QString &us)
            : rejected(rej),
            used(us)
    {
    }
    QString rejected;
    QString used;
};

class EnumTypeEntry : public TypeEntry
{
public:
    EnumTypeEntry(const QString &nspace, const QString &enumName, double vr)
            : TypeEntry(nspace.isEmpty() ? enumName : nspace + QLatin1String("::") + enumName,
                        EnumType, vr),
            m_flags(0),
            m_extensible(false)
    {
        m_qualifier = nspace;
        m_targetLangName = enumName;
    }

    QString targetLangPackage() const
    {
        return m_packageName;
    }
    void setTargetLangPackage(const QString &package)
    {
        m_packageName = package;
    }

    QString targetLangName() const
    {
        return m_targetLangName;
    }
    QString targetLangQualifier() const;
    QString qualifiedTargetLangName() const
    {
        QString qualifiedName;
        QString pkg = targetLangPackage();
        QString qualifier = targetLangQualifier();

        if (!pkg.isEmpty())
            qualifiedName += pkg + '.';
        if (!qualifier.isEmpty())
            qualifiedName += qualifier + '.';
        qualifiedName += targetLangName();

        return qualifiedName;
    }

    QString targetLangApiName() const;

    QString qualifier() const
    {
        return m_qualifier;
    }
    void setQualifier(const QString &q)
    {
        m_qualifier = q;
    }

    virtual bool preferredConversion() const
    {
        return false;
    }

    bool isBoundsChecked() const
    {
        return m_lowerBound.isEmpty() && m_upperBound.isEmpty();
    }

    QString upperBound() const
    {
        return m_upperBound;
    }
    void setUpperBound(const QString &bound)
    {
        m_upperBound = bound;
    }

    QString lowerBound() const
    {
        return m_lowerBound;
    }
    void setLowerBound(const QString &bound)
    {
        m_lowerBound = bound;
    }

    void setFlags(FlagsTypeEntry *flags)
    {
        m_flags = flags;
    }
    FlagsTypeEntry *flags() const
    {
        return m_flags;
    }

    bool isExtensible() const
    {
        return m_extensible;
    }
    void setExtensible(bool is)
    {
        m_extensible = is;
    }

    bool isEnumValueRejected(const QString &name)
    {
        return m_rejectedEnums.contains(name);
    }
    void addEnumValueRejection(const QString &name)
    {
        m_rejectedEnums << name;
    }
    QStringList enumValueRejections() const
    {
        return m_rejectedEnums;
    }

    void addEnumValueRedirection(const QString &rejected, const QString &usedValue);
    QString enumValueRedirection(const QString &value) const;

    bool forceInteger() const
    {
        return m_forceInteger;
    }
    void setForceInteger(bool force)
    {
        m_forceInteger = force;
    }

    bool isAnonymous() const
    {
        return m_anonymous;
    }
    void setAnonymous(bool anonymous)
    {
        m_anonymous = anonymous;
    }

private:
    QString m_packageName;
    QString m_qualifier;
    QString m_targetLangName;

    QString m_lowerBound;
    QString m_upperBound;

    QStringList m_rejectedEnums;
    QList<EnumValueRedirection> m_enumRedirections;

    FlagsTypeEntry *m_flags;

    bool m_extensible;
    bool m_forceInteger;
    bool m_anonymous;
};

class EnumValueTypeEntry : public TypeEntry
{
public:
    EnumValueTypeEntry(const QString& name, const QString& value, const EnumTypeEntry* enclosingEnum, double vr)
        : TypeEntry(name, TypeEntry::EnumValue, vr), m_value(value), m_enclosingEnum(enclosingEnum)
    {
    }

    QString value() const { return m_value; }
    const EnumTypeEntry* enclosingEnum() const { return m_enclosingEnum; }
private:
    QString m_value;
    const EnumTypeEntry* m_enclosingEnum;
};

class FlagsTypeEntry : public TypeEntry
{
public:
    FlagsTypeEntry(const QString &name, double vr) : TypeEntry(name, FlagsType, vr), m_enum(0)
    {
    }

    QString qualifiedTargetLangName() const;
    QString targetLangName() const
    {
        return m_targetLangName;
    }
    QString targetLangApiName() const;
    virtual bool preferredConversion() const
    {
        return false;
    }

    QString originalName() const
    {
        return m_originalName;
    }
    void setOriginalName(const QString &s)
    {
        m_originalName = s;
    }

    QString flagsName() const
    {
        return m_targetLangName;
    }
    void setFlagsName(const QString &name)
    {
        m_targetLangName = name;
    }

    bool forceInteger() const
    {
        return m_enum->forceInteger();
    }

    EnumTypeEntry *originator() const
    {
        return m_enum;
    }
    void setOriginator(EnumTypeEntry *e)
    {
        m_enum = e;
    }

    QString targetLangPackage() const
    {
        return m_enum->targetLangPackage();
    }

private:
    QString m_originalName;
    QString m_targetLangName;
    EnumTypeEntry *m_enum;
};


class ComplexTypeEntry : public TypeEntry
{
public:
    enum TypeFlag {
        ForceAbstract      = 0x1,
        DeleteInMainThread = 0x2,
        Deprecated         = 0x4
    };
    typedef QFlags<TypeFlag> TypeFlags;

    enum CopyableFlag {
        CopyableSet,
        NonCopyableSet,
        Unknown
    };

    ComplexTypeEntry(const QString &name, Type t, double vr)
            : TypeEntry(QString(name).replace(".*::", ""), t, vr),
            m_qualifiedCppName(name),
            m_qobject(false),
            m_polymorphicBase(false),
            m_genericClass(false),
            m_typeFlags(0),
            m_copyableFlag(Unknown),
            m_hashFunction(""),
            m_baseContainerType(0)
    {
    }

    bool isComplex() const
    {
        return true;
    }

    ComplexTypeEntry *copy() const
    {
        ComplexTypeEntry *centry = new ComplexTypeEntry(name(), type(), version());
        centry->setInclude(include());
        centry->setExtraIncludes(extraIncludes());
        centry->setAddedFunctions(addedFunctions());
        centry->setFunctionModifications(functionModifications());
        centry->setFieldModifications(fieldModifications());
        centry->setQObject(isQObject());
        centry->setDefaultSuperclass(defaultSuperclass());
        centry->setCodeSnips(codeSnips());
        centry->setTargetLangPackage(targetLangPackage());
        centry->setBaseContainerType(baseContainerType());
        centry->setDefaultConstructor(defaultConstructor());

        return centry;
    }

    void setLookupName(const QString &name)
    {
        m_lookupName = name;
    }

    virtual QString lookupName() const
    {
        return m_lookupName.isEmpty() ? targetLangName() : m_lookupName;
    }

    QString targetLangApiName() const;

    void setTypeFlags(TypeFlags flags)
    {
        m_typeFlags = flags;
    }

    TypeFlags typeFlags() const
    {
        return m_typeFlags;
    }

    FunctionModificationList functionModifications() const
    {
        return m_functionMods;
    }
    void setFunctionModifications(const FunctionModificationList &functionModifications)
    {
        m_functionMods = functionModifications;
    }
    void addFunctionModification(const FunctionModification &functionModification)
    {
        m_functionMods << functionModification;
    }
    FunctionModificationList functionModifications(const QString &signature) const;

    AddedFunctionList addedFunctions() const
    {
        return m_addedFunctions;
    }
    void setAddedFunctions(const AddedFunctionList &addedFunctions)
    {
        m_addedFunctions = addedFunctions;
    }
    void addNewFunction(const AddedFunction &addedFunction)
    {
        m_addedFunctions << addedFunction;
    }

    FieldModification fieldModification(const QString &name) const;
    void setFieldModifications(const FieldModificationList &mods)
    {
        m_fieldMods = mods;
    }
    FieldModificationList fieldModifications() const
    {
        return m_fieldMods;
    }

    QString targetLangPackage() const
    {
        return m_package;
    }
    void setTargetLangPackage(const QString &package)
    {
        m_package = package;
    }

    bool isQObject() const
    {
        return m_qobject;
    }
    void setQObject(bool qobject)
    {
        m_qobject = qobject;
    }

    QString defaultSuperclass() const
    {
        return m_defaultSuperclass;
    }
    void setDefaultSuperclass(const QString &sc)
    {
        m_defaultSuperclass = sc;
    }

    virtual QString qualifiedCppName() const
    {
        return m_qualifiedCppName;
    }


    void setIsPolymorphicBase(bool on)
    {
        m_polymorphicBase = on;
    }
    bool isPolymorphicBase() const
    {
        return m_polymorphicBase;
    }

    void setPolymorphicIdValue(const QString &value)
    {
        m_polymorphicIdValue = value;
    }
    QString polymorphicIdValue() const
    {
        return m_polymorphicIdValue;
    }

    void setHeldType(const QString &value)
    {
        m_heldTypeValue = value;
    }
    QString heldTypeValue() const
    {
        return m_heldTypeValue;
    }


    void setExpensePolicy(const ExpensePolicy &policy)
    {
        m_expensePolicy = policy;
    }
    const ExpensePolicy &expensePolicy() const
    {
        return m_expensePolicy;
    }

    QString targetType() const
    {
        return m_targetType;
    }
    void setTargetType(const QString &code)
    {
        m_targetType = code;
    }

    QString targetLangName() const
    {
        return m_targetLangName.isEmpty()
               ? TypeEntry::targetLangName()
               : m_targetLangName;
    }
    void setTargetLangName(const QString &name)
    {
        m_targetLangName = name;
    }

    bool isGenericClass() const
    {
        return m_genericClass;
    }
    void setGenericClass(bool isGeneric)
    {
        m_genericClass = isGeneric;
    }

    CopyableFlag copyable() const
    {
        return m_copyableFlag;
    }
    void setCopyable(CopyableFlag flag)
    {
        m_copyableFlag = flag;
    }

    QString hashFunction() const
    {
        return m_hashFunction;
    }
    void setHashFunction(QString hashFunction)
    {
        m_hashFunction = hashFunction;
    }

    void setBaseContainerType(const ComplexTypeEntry *baseContainer)
    {
        m_baseContainerType = baseContainer;
    }

    const ComplexTypeEntry* baseContainerType() const
    {
        return m_baseContainerType;
    }

    QString defaultConstructor() const;
    void setDefaultConstructor(const QString& defaultConstructor);
    bool hasDefaultConstructor() const;

private:
    AddedFunctionList m_addedFunctions;
    FunctionModificationList m_functionMods;
    FieldModificationList m_fieldMods;
    QString m_package;
    QString m_defaultSuperclass;
    QString m_qualifiedCppName;
    QString m_targetLangName;

    uint m_qobject : 1;
    uint m_polymorphicBase : 1;
    uint m_genericClass : 1;

    QString m_polymorphicIdValue;
    QString m_heldTypeValue;
    QString m_lookupName;
    QString m_targetType;
    ExpensePolicy m_expensePolicy;
    TypeFlags m_typeFlags;
    CopyableFlag m_copyableFlag;
    QString m_hashFunction;

    const ComplexTypeEntry* m_baseContainerType;
};

class ContainerTypeEntry : public ComplexTypeEntry
{
public:
    enum Type {
        NoContainer,
        ListContainer,
        StringListContainer,
        LinkedListContainer,
        VectorContainer,
        StackContainer,
        QueueContainer,
        SetContainer,
        MapContainer,
        MultiMapContainer,
        HashContainer,
        MultiHashContainer,
        PairContainer,
    };

    ContainerTypeEntry(const QString &name, Type type, double vr)
        : ComplexTypeEntry(name, ContainerType, vr), m_type(type)
    {
        setCodeGeneration(GenerateForSubclass);
    }

    Type type() const
    {
        return m_type;
    }

    QString typeName() const;
    QString targetLangName() const;
    QString targetLangPackage() const;
    QString qualifiedCppName() const;

    static Type containerTypeFromString(QString typeName)
    {
        static QHash<QString, Type> m_stringToContainerType;
        if (m_stringToContainerType.isEmpty()) {
            m_stringToContainerType["list"] = ListContainer;
            m_stringToContainerType["string-list"] = StringListContainer;
            m_stringToContainerType["linked-list"] = LinkedListContainer;
            m_stringToContainerType["vector"] = VectorContainer;
            m_stringToContainerType["stack"] = StackContainer;
            m_stringToContainerType["queue"] = QueueContainer;
            m_stringToContainerType["set"] = SetContainer;
            m_stringToContainerType["map"] = MapContainer;
            m_stringToContainerType["multi-map"] = MultiMapContainer;
            m_stringToContainerType["hash"] = HashContainer;
            m_stringToContainerType["multi-hash"] = MultiHashContainer;
            m_stringToContainerType["pair"] = PairContainer;
        }
        return m_stringToContainerType.value(typeName, NoContainer);
    }

private:
    Type m_type;
};

typedef QList<const ContainerTypeEntry*> ContainerTypeEntryList;

class NamespaceTypeEntry : public ComplexTypeEntry
{
public:
    NamespaceTypeEntry(const QString &name, double vr) : ComplexTypeEntry(name, NamespaceType, vr) { }
};


class ValueTypeEntry : public ComplexTypeEntry
{
public:
    ValueTypeEntry(const QString &name, double vr) : ComplexTypeEntry(name, BasicValueType, vr) { }

    bool isValue() const
    {
        return true;
    }

    virtual bool isNativeIdBased() const
    {
        return true;
    }

protected:
    ValueTypeEntry(const QString &name, Type t, double vr) : ComplexTypeEntry(name, t, vr) { }
};


class StringTypeEntry : public ValueTypeEntry
{
public:
    StringTypeEntry(const QString &name, double vr)
            : ValueTypeEntry(name, StringType, vr)
    {
        setCodeGeneration(GenerateNothing);
    }

    QString targetLangApiName() const;
    QString targetLangName() const;
    QString targetLangPackage() const;

    virtual bool isNativeIdBased() const
    {
        return false;
    }
};

class CharTypeEntry : public ValueTypeEntry
{
public:
    CharTypeEntry(const QString &name, double vr) : ValueTypeEntry(name, CharType, vr)
    {
        setCodeGeneration(GenerateNothing);
    }

    QString targetLangApiName() const;
    QString targetLangName() const;
    QString targetLangPackage() const
    {
        return QString();
    }

    virtual bool isNativeIdBased() const
    {
        return false;
    }
};

class VariantTypeEntry: public ValueTypeEntry
{
public:
    VariantTypeEntry(const QString &name, double vr) : ValueTypeEntry(name, VariantType, vr) { }

    QString targetLangApiName() const;
    QString targetLangName() const;
    QString targetLangPackage() const;

    virtual bool isNativeIdBased() const
    {
        return false;
    }
};


class InterfaceTypeEntry : public ComplexTypeEntry
{
public:
    InterfaceTypeEntry(const QString &name, double vr)
            : ComplexTypeEntry(name, InterfaceType, vr) {}

    static QString interfaceName(const QString &name)
    {
        return name + "Interface";
    }

    ObjectTypeEntry *origin() const
    {
        return m_origin;
    }
    void setOrigin(ObjectTypeEntry *origin)
    {
        m_origin = origin;
    }

    virtual bool isNativeIdBased() const
    {
        return true;
    }
    virtual QString qualifiedCppName() const
    {
        return ComplexTypeEntry::qualifiedCppName().left(ComplexTypeEntry::qualifiedCppName().length() - interfaceName("").length());
    }

private:
    ObjectTypeEntry *m_origin;
};


class FunctionTypeEntry : public TypeEntry
{
public:
    FunctionTypeEntry(const QString& name, const QString& signature, double vr)
            : TypeEntry(name, FunctionType, vr)
    {
        addSignature(signature);
    }
    void addSignature(const QString& signature)
    {
        m_signatures << signature;
    }

    QStringList signatures() const
    {
        return m_signatures;
    }

    bool hasSignature(const QString& signature) const
    {
        return m_signatures.contains(signature);
    }
private:
    QStringList m_signatures;
};

class ObjectTypeEntry : public ComplexTypeEntry
{
public:
    ObjectTypeEntry(const QString &name, double vr)
            : ComplexTypeEntry(name, ObjectType, vr), m_interface(0) {}

    InterfaceTypeEntry *designatedInterface() const
    {
        return m_interface;
    }
    void setDesignatedInterface(InterfaceTypeEntry *entry)
    {
        m_interface = entry;
    }

    virtual bool isNativeIdBased() const
    {
        return true;
    }

private:
    InterfaceTypeEntry *m_interface;
};

struct TypeRejection
{
    QString class_name;
    QString function_name;
    QString field_name;
    QString enum_name;
};

QString fixCppTypeName(const QString &name);

class CustomConversion
{
public:
    CustomConversion(TypeEntry* ownerType);
    ~CustomConversion();

    const TypeEntry* ownerType() const;
    QString nativeToTargetConversion() const;
    void setNativeToTargetConversion(const QString& nativeToTargetConversion);

    class TargetToNativeConversion
    {
    public:
        TargetToNativeConversion(const QString& sourceTypeName,
                                 const QString& sourceTypeCheck,
                                 const QString& conversion = QString());
        ~TargetToNativeConversion();

        const TypeEntry* sourceType() const;
        void setSourceType(const TypeEntry* sourceType);
        bool isCustomType() const;
        QString sourceTypeName() const;
        QString sourceTypeCheck() const;
        QString conversion() const;
        void setConversion(const QString& conversion);
    private:
        struct TargetToNativeConversionPrivate;
        TargetToNativeConversionPrivate* m_d;
    };

    /**
     *  Returns true if the target to C++ custom conversions should
     *  replace the original existing ones, and false if the custom
     *  conversions should be added to the original.
     */
    bool replaceOriginalTargetToNativeConversions() const;
    void setReplaceOriginalTargetToNativeConversions(bool replaceOriginalTargetToNativeConversions);

    typedef QList<TargetToNativeConversion*> TargetToNativeConversions;
    bool hasTargetToNativeConversions() const;
    TargetToNativeConversions& targetToNativeConversions();
    const TargetToNativeConversions& targetToNativeConversions() const;
    void addTargetToNativeConversion(const QString& sourceTypeName,
                                     const QString& sourceTypeCheck,
                                     const QString& conversion = QString());
private:
    struct CustomConversionPrivate;
    CustomConversionPrivate* m_d;
};

#endif // TYPESYSTEM_H
