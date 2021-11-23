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

#include "abstractmetalang.h"
#include "reporthandler.h"
#include "typedatabase.h"

/*******************************************************************************
 * AbstractMetaVariable
 */

AbstractMetaVariable::AbstractMetaVariable(const AbstractMetaVariable &other)
{
    m_originalName = other.m_originalName;
    m_name = other.m_name;
    m_type = other.m_type->copy();
    m_hasName = other.m_hasName;
    m_doc = other.m_doc;
}

/*******************************************************************************
 * AbstractMetaType
 */

AbstractMetaType::AbstractMetaType()
    :m_typeEntry(0),
    m_arrayElementCount(0),
    m_arrayElementType(0),
    m_originalTemplateType(0),
    m_pattern(InvalidPattern),
    m_constant(false),
    m_reference(false),
    m_cppInstantiation(true),
    m_indirections(0),
    m_reserved(0)
{
}

AbstractMetaType::~AbstractMetaType()
{
    qDeleteAll(m_children);
    m_instantiations.clear();
}

AbstractMetaType *AbstractMetaType::copy() const
{
    AbstractMetaType *cpy = new AbstractMetaType;

    cpy->setTypeUsagePattern(typeUsagePattern());
    cpy->setConstant(isConstant());
    cpy->setReference(isReference());
    cpy->setIndirections(indirections());
    cpy->setInstantiations(instantiations());
    cpy->setArrayElementCount(arrayElementCount());
    cpy->setOriginalTypeDescription(originalTypeDescription());
    cpy->setOriginalTemplateType(originalTemplateType() ? originalTemplateType()->copy() : 0);

    cpy->setArrayElementType(arrayElementType() ? arrayElementType()->copy() : 0);

    cpy->setTypeEntry(typeEntry());

    return cpy;
}

QString AbstractMetaType::cppSignature() const
{
    if (m_cachedCppSignature.isEmpty()) {
        if (isConstant())
            m_cachedCppSignature += "const ";

        m_cachedCppSignature += typeEntry()->qualifiedCppName();

        if (hasInstantiationInCpp()) {
            AbstractMetaTypeList types = instantiations();
            m_cachedCppSignature += "<";
            for (int i = 0; i < types.count(); ++i) {
                if (i > 0)
                    m_cachedCppSignature += ", ";
                m_cachedCppSignature += types[i]->cppSignature();
            }
            m_cachedCppSignature += " >";
        }

        if (actualIndirections()) {
            m_cachedCppSignature += ' ';
            if (indirections())
                m_cachedCppSignature += QString(indirections(), '*');
            if (isReference())
                m_cachedCppSignature += '&';
        }
    }
    return m_cachedCppSignature;
}

void AbstractMetaType::decideUsagePattern()
{
    const TypeEntry* type = typeEntry();

    if (type->isPrimitive() && (!actualIndirections()
        || (isConstant() && isReference() && !indirections()))) {
        setTypeUsagePattern(AbstractMetaType::PrimitivePattern);

    } else if (type->isVoid()) {
        setTypeUsagePattern(AbstractMetaType::NativePointerPattern);

    } else if (type->isVarargs()) {
        setTypeUsagePattern(AbstractMetaType::VarargsPattern);

    } else if (type->isString()
               && indirections() == 0
               && (isConstant() == isReference()
                   || isConstant())) {
        setTypeUsagePattern(AbstractMetaType::StringPattern);

    } else if (type->isChar()
               && !indirections()
               && isConstant() == isReference()) {
        setTypeUsagePattern(AbstractMetaType::CharPattern);

    } else if (type->isJObjectWrapper()
               && !indirections()
               && isConstant() == isReference()) {
        setTypeUsagePattern(AbstractMetaType::JObjectWrapperPattern);

    } else if (type->isVariant()
               && !indirections()
               && isConstant() == isReference()) {
        setTypeUsagePattern(AbstractMetaType::VariantPattern);

    } else if (type->isEnum() && !actualIndirections()) {
        setTypeUsagePattern(AbstractMetaType::EnumPattern);

    } else if (type->isObject() && indirections() == 0) {
        if (isReference()) {
            if (((ComplexTypeEntry*) type)->isQObject())
                setTypeUsagePattern(AbstractMetaType::QObjectPattern);
            else
                setTypeUsagePattern(AbstractMetaType::ObjectPattern);
        } else {
            setTypeUsagePattern(AbstractMetaType::ValuePattern);
        }

    } else if (type->isObject()
               && indirections() == 1) {
        if (((ComplexTypeEntry*) type)->isQObject())
            setTypeUsagePattern(AbstractMetaType::QObjectPattern);
        else
            setTypeUsagePattern(AbstractMetaType::ObjectPattern);

        // const-references to pointers can be passed as pointers
        if (isReference() && isConstant()) {
            setReference(false);
            setConstant(false);
        }

    } else if (type->isContainer() && !indirections()) {
        setTypeUsagePattern(AbstractMetaType::ContainerPattern);

    } else if (type->isTemplateArgument()) {

    } else if (type->isFlags()
               && !indirections()
               && (isConstant() == isReference())) {
        setTypeUsagePattern(AbstractMetaType::FlagsPattern);

    } else if (type->isArray()) {
        setTypeUsagePattern(AbstractMetaType::ArrayPattern);

    } else if (type->isThread()) {
        Q_ASSERT(indirections() == 1);
        setTypeUsagePattern(AbstractMetaType::ThreadPattern);
    } else if (type->isValue()) {
        if (indirections() == 1) {
            setTypeUsagePattern(AbstractMetaType::ValuePointerPattern);
        } else {
            setTypeUsagePattern(AbstractMetaType::ValuePattern);
        }
    } else {
        setTypeUsagePattern(AbstractMetaType::NativePointerPattern);
        ReportHandler::debugFull(QString("native pointer pattern for '%1'")
                                 .arg(cppSignature()));
    }
}

/*******************************************************************************
 * AbstractMetaArgument
 */
AbstractMetaArgument *AbstractMetaArgument::copy() const
{
    return new AbstractMetaArgument(*this);
}

/*******************************************************************************
 * AbstractMetaFunction
 */
AbstractMetaFunction::~AbstractMetaFunction()
{
    qDeleteAll(m_arguments);
    delete m_type;
}

/*******************************************************************************
 * Indicates that this function has a modification that removes it
 */
bool AbstractMetaFunction::isModifiedRemoved(int types) const
{
    FunctionModificationList mods = modifications(implementingClass());
    foreach (FunctionModification mod, mods) {
        if (!mod.isRemoveModifier())
            continue;

        if ((mod.removal & types) == types)
            return true;
    }

    return false;
}

bool AbstractMetaFunction::needsCallThrough() const
{
    if (ownerClass()->isInterface())
        return false;
    if (referenceCounts(implementingClass()).size() > 0)
        return true;
    if (argumentsHaveNativeId() || !isStatic())
        return true;

    foreach (const AbstractMetaArgument *arg, arguments()) {
        if (arg->type()->isArray() || arg->type()->isTargetLangEnum() || arg->type()->isTargetLangFlags())
            return true;
    }

    if (type() && (type()->isArray() || type()->isTargetLangEnum() || type()->isTargetLangFlags()))
        return true;

    for (int i = -1; i <= arguments().size(); ++i) {
        TypeSystem::Ownership owner = this->ownership(implementingClass(), TypeSystem::TargetLangCode, i);
        if (owner != TypeSystem::InvalidOwnership)
            return true;
    }

    return false;
}

bool AbstractMetaFunction::needsSuppressUncheckedWarning() const
{
    for (int i = -1; i <= arguments().size(); ++i) {
        QList<ReferenceCount> referenceCounts = this->referenceCounts(implementingClass(), i);
        foreach (ReferenceCount referenceCount, referenceCounts) {
            if (referenceCount.action != ReferenceCount::Set)
                return true;
        }
    }
    return false;
}

QString AbstractMetaFunction::marshalledName() const
{
    QString returned = "__qt_" + name();
    AbstractMetaArgumentList arguments = this->arguments();
    foreach (const AbstractMetaArgument *arg, arguments) {
        returned += "_";
        if (arg->type()->isNativePointer())
            returned += "nativepointer";
        else if (arg->type()->isIntegerEnum() || arg->type()->isIntegerFlags())
            returned += "int";
        else
            returned += arg->type()->name().replace("[]", "_3").replace(".", "_");
    }
    return returned;
}

bool AbstractMetaFunction::operator<(const AbstractMetaFunction &other) const
{
    uint result = compareTo(&other);
    return result & NameLessThan;
}


/*!
    Returns a mask of CompareResult describing how this function is
    compares to another function
*/
uint AbstractMetaFunction::compareTo(const AbstractMetaFunction *other) const
{
    uint result = 0;

    // Enclosing class...
    if (ownerClass() == other->ownerClass())
        result |= EqualImplementor;

    // Attributes
    if (attributes() == other->attributes())
        result |= EqualAttributes;

    // Compare types
    AbstractMetaType *t = type();
    AbstractMetaType *ot = other->type();
    if ((!t && !ot) || ((t && ot && t->name() == ot->name())))
        result |= EqualReturnType;

    // Compare names
    int cmp = originalName().compare(other->originalName());

    if (cmp < 0)
        result |= NameLessThan;
    else if (!cmp)
        result |= EqualName;

    // compare name after modification...
    cmp = modifiedName().compare(other->modifiedName());
    if (!cmp)
        result |= EqualModifiedName;

    // Compare arguments...
    AbstractMetaArgumentList minArguments;
    AbstractMetaArgumentList maxArguments;
    if (arguments().size() < other->arguments().size()) {
        minArguments = arguments();
        maxArguments = other->arguments();
    } else {
        minArguments = other->arguments();
        maxArguments = arguments();
    }

    int minCount = minArguments.size();
    int maxCount = maxArguments.size();
    bool same = true;
    for (int i = 0; i < maxCount; ++i) {
        if (i < minCount) {
            const AbstractMetaArgument *min_arg = minArguments.at(i);
            const AbstractMetaArgument *max_arg = maxArguments.at(i);
            if (min_arg->type()->name() != max_arg->type()->name()
                && (min_arg->defaultValueExpression().isEmpty() || max_arg->defaultValueExpression().isEmpty())) {
                same = false;
                break;
            }
        } else {
            if (maxArguments.at(i)->defaultValueExpression().isEmpty()) {
                same = false;
                break;
            }
        }
    }

    if (same)
        result |= minCount == maxCount ? EqualArguments : EqualDefaultValueOverload;

    return result;
}

AbstractMetaFunction *AbstractMetaFunction::copy() const
{
    AbstractMetaFunction *cpy = new AbstractMetaFunction;
    cpy->setName(name());
    cpy->setOriginalName(originalName());
    cpy->setOwnerClass(ownerClass());
    cpy->setImplementingClass(implementingClass());
    cpy->setInterfaceClass(interfaceClass());
    cpy->setFunctionType(functionType());
    cpy->setAttributes(attributes());
    cpy->setDeclaringClass(declaringClass());
    if (type())
        cpy->setType(type()->copy());
    cpy->setConstant(isConstant());
    cpy->setOriginalAttributes(originalAttributes());

    foreach (AbstractMetaArgument *arg, arguments())
    cpy->addArgument(arg->copy());

    Q_ASSERT((!type() && !cpy->type())
             || (type()->instantiations() == cpy->type()->instantiations()));

    return cpy;
}

QStringList AbstractMetaFunction::introspectionCompatibleSignatures(const QStringList &resolvedArguments) const
{
    AbstractMetaArgumentList arguments = this->arguments();
    if (arguments.size() == resolvedArguments.size()) {
        return (QStringList() << TypeDatabase::normalizedSignature((name() + "(" + resolvedArguments.join(",") + ")").toUtf8().constData()));
    } else {
        QStringList returned;

        AbstractMetaArgument *argument = arguments.at(resolvedArguments.size());
        QStringList minimalTypeSignature = argument->type()->minimalSignature().split("::");
        for (int i = 0; i < minimalTypeSignature.size(); ++i) {
            returned += introspectionCompatibleSignatures(QStringList(resolvedArguments)
                                                          << QStringList(minimalTypeSignature.mid(minimalTypeSignature.size() - i - 1)).join("::"));
        }

        return returned;
    }
}

QString AbstractMetaFunction::signature() const
{
    if (m_cachedSignature.isEmpty()) {
        m_cachedSignature = m_originalName;

        m_cachedSignature += '(';

        for (int i = 0; i < m_arguments.count(); ++i) {
            if (i > 0)
                m_cachedSignature += ", ";
            AbstractMetaArgument *a = m_arguments.at(i);
            m_cachedSignature += a->type()->cppSignature();

            // We need to have the argument names in the qdoc files
            m_cachedSignature += ' ';
            m_cachedSignature += a->name();
        }
        m_cachedSignature += ")";

        if (isConstant())
            m_cachedSignature += " const";
    }
    return m_cachedSignature;
}

int AbstractMetaFunction::actualMinimumArgumentCount() const
{
    AbstractMetaArgumentList arguments = this->arguments();

    int count = 0;
    for (int i = 0; i < arguments.size(); ++i && ++count) {
        if (argumentRemoved(i + 1))
            --count;
        else if (!arguments.at(i)->defaultValueExpression().isEmpty())
            break;
    }

    return count;
}

// Returns reference counts for argument at idx, or all arguments if idx == -2
QList<ReferenceCount> AbstractMetaFunction::referenceCounts(const AbstractMetaClass *cls, int idx) const
{
    QList<ReferenceCount> returned;

    FunctionModificationList mods = this->modifications(cls);
    foreach (FunctionModification mod, mods) {
        QList<ArgumentModification> argumentMods = mod.argument_mods;
        foreach (ArgumentModification argumentMod, argumentMods) {
            if (argumentMod.index != idx && idx != -2)
                continue;
            returned += argumentMod.referenceCounts;
        }
    }

    return returned;
}


ArgumentOwner AbstractMetaFunction::argumentOwner(const AbstractMetaClass *cls, int idx) const
{
    FunctionModificationList mods = this->modifications(cls);
    foreach (FunctionModification mod, mods) {
        QList<ArgumentModification> argumentMods = mod.argument_mods;
        foreach (ArgumentModification argumentMod, argumentMods) {
            if (argumentMod.index != idx)
                continue;
            return argumentMod.owner;
        }
    }
    return ArgumentOwner();
}


QString AbstractMetaFunction::replacedDefaultExpression(const AbstractMetaClass *cls, int key) const
{
    FunctionModificationList modifications = this->modifications(cls);
    foreach (FunctionModification modification, modifications) {
        QList<ArgumentModification> argumentModifications = modification.argument_mods;
        foreach (ArgumentModification argumentModification, argumentModifications) {
            if (argumentModification.index == key
                && !argumentModification.replacedDefaultExpression.isEmpty()) {
                return argumentModification.replacedDefaultExpression;
            }
        }
    }

    return QString();
}

bool AbstractMetaFunction::removedDefaultExpression(const AbstractMetaClass *cls, int key) const
{
    FunctionModificationList modifications = this->modifications(cls);
    foreach (FunctionModification modification, modifications) {
        QList<ArgumentModification> argumentModifications = modification.argument_mods;
        foreach (ArgumentModification argumentModification, argumentModifications) {
            if (argumentModification.index == key
                && argumentModification.removedDefaultExpression) {
                return true;
            }
        }
    }

    return false;
}

bool AbstractMetaFunction::resetObjectAfterUse(int argumentIdx) const
{
    const AbstractMetaClass *cls = declaringClass();
    FunctionModificationList modifications = this->modifications(cls);
    foreach (FunctionModification modification, modifications) {
        QList<ArgumentModification> argumentModifications = modification.argument_mods;
        foreach (ArgumentModification argumentModification, argumentModifications) {
            if (argumentModification.index == argumentIdx && argumentModification.resetAfterUse)
                return true;
        }
    }

    return false;
}

QString AbstractMetaFunction::nullPointerDefaultValue(const AbstractMetaClass *mainClass, int argumentIdx) const
{
    Q_ASSERT(nullPointersDisabled(mainClass, argumentIdx));

    const AbstractMetaClass *cls = mainClass;
    if (!cls)
        cls = implementingClass();

    do {
        FunctionModificationList modifications = this->modifications(cls);
        foreach (FunctionModification modification, modifications) {
            QList<ArgumentModification> argumentModifications = modification.argument_mods;
            foreach (ArgumentModification argumentModification, argumentModifications) {
                if (argumentModification.index == argumentIdx
                    && argumentModification.noNullPointers) {
                    return argumentModification.nullPointerDefaultValue;
                }
            }
        }
        cls = cls->baseClass();
    } while (cls && !mainClass); // Once when mainClass, or once for all base classes of implementing class

    return QString();

}

bool AbstractMetaFunction::nullPointersDisabled(const AbstractMetaClass *mainClass, int argumentIdx) const
{
    const AbstractMetaClass *cls = mainClass;
    if (!cls)
        cls = implementingClass();

    do {
        FunctionModificationList modifications = this->modifications(cls);
        foreach (FunctionModification modification, modifications) {
            QList<ArgumentModification> argumentModifications = modification.argument_mods;
            foreach (ArgumentModification argumentModification, argumentModifications) {
                if (argumentModification.index == argumentIdx
                    && argumentModification.noNullPointers) {
                    return true;
                }
            }
        }

        cls = cls->baseClass();
    } while (cls && !mainClass); // Once when mainClass, or once for all base classes of implementing class

    return false;
}

QString AbstractMetaFunction::conversionRule(TypeSystem::Language language, int key) const
{
    FunctionModificationList modifications = this->modifications(declaringClass());
    foreach (FunctionModification modification, modifications) {
        QList<ArgumentModification> argumentModifications = modification.argument_mods;
        foreach (ArgumentModification argumentModification, argumentModifications) {
            if (argumentModification.index != key)
                continue;

            foreach (CodeSnip snip, argumentModification.conversion_rules) {
                if (snip.language == language && !snip.code().isEmpty())
                    return snip.code();
            }
        }
    }

    return QString();
}

QString AbstractMetaFunction::argumentReplaced(int key) const
{
    FunctionModificationList modifications = this->modifications(declaringClass());
    foreach (FunctionModification modification, modifications) {
        QList<ArgumentModification>& argumentModifications = modification.argument_mods;
        foreach (ArgumentModification argumentModification, argumentModifications) {
            if (argumentModification.index == key && !argumentModification.replace_value.isEmpty())
                return argumentModification.replace_value;
        }
    }

    return "";
}

// FIXME If we remove a arg. in the method at the base class, it will not reflect here.
bool AbstractMetaFunction::argumentRemoved(int key) const
{
    FunctionModificationList modifications = this->modifications(declaringClass());
    foreach (FunctionModification modification, modifications) {
        QList<ArgumentModification> argumentModifications = modification.argument_mods;
        foreach (ArgumentModification argumentModification, argumentModifications) {
            if (argumentModification.index == key) {
                if (argumentModification.removed)
                    return true;
            }
        }
    }

    return false;
}

bool AbstractMetaFunction::isVirtualSlot() const
{
    FunctionModificationList modifications = this->modifications(declaringClass());
    foreach (FunctionModification modification, modifications) {
        if (modification.isVirtualSlot())
            return true;
    }

    return false;
}

bool AbstractMetaFunction::disabledGarbageCollection(const AbstractMetaClass *cls, int key) const
{
    FunctionModificationList modifications = this->modifications(cls);
    foreach (FunctionModification modification, modifications) {
        QList<ArgumentModification> argumentModifications = modification.argument_mods;
        foreach (ArgumentModification argumentModification, argumentModifications) {
            if (argumentModification.index != key)
                continue;

            foreach (TypeSystem::Ownership ownership, argumentModification.ownerships.values()) {
                if (ownership == TypeSystem::CppOwnership)
                    return true;
            }

        }
    }

    return false;
}

bool AbstractMetaFunction::isDeprecated() const
{
    FunctionModificationList modifications = this->modifications(declaringClass());
    foreach (FunctionModification modification, modifications) {
        if (modification.isDeprecated())
            return true;
    }
    return false;
}

bool AbstractMetaFunction::isThread() const
{
    FunctionModificationList modifications = this->modifications(declaringClass());
    foreach (FunctionModification modification, modifications) {
        if (modification.isThread())
            return true;
    }
    return false;
}

bool AbstractMetaFunction::allowThread() const
{
    FunctionModificationList modifications = this->modifications(declaringClass());
    foreach (FunctionModification modification, modifications) {
        if (modification.allowThread())
            return true;
    }
    return false;
}


TypeSystem::Ownership AbstractMetaFunction::ownership(const AbstractMetaClass *cls, TypeSystem::Language language, int key) const
{
    FunctionModificationList modifications = this->modifications(cls);
    foreach (FunctionModification modification, modifications) {
        QList<ArgumentModification> argumentModifications = modification.argument_mods;
        foreach (ArgumentModification argumentModification, argumentModifications) {
            if (argumentModification.index == key)
                return argumentModification.ownerships.value(language, TypeSystem::InvalidOwnership);
        }
    }

    return TypeSystem::InvalidOwnership;
}

bool AbstractMetaFunction::isRemovedFromAllLanguages(const AbstractMetaClass *cls) const
{
    return isRemovedFrom(cls, TypeSystem::All);
}

bool AbstractMetaFunction::isRemovedFrom(const AbstractMetaClass *cls, TypeSystem::Language language) const
{
    FunctionModificationList modifications = this->modifications(cls);
    foreach (FunctionModification modification, modifications) {
        if ((modification.removal & language) == language)
            return true;
    }

    return false;

}

QString AbstractMetaFunction::typeReplaced(int key) const
{
    FunctionModificationList modifications = this->modifications(declaringClass());
    foreach (FunctionModification modification, modifications) {
        QList<ArgumentModification> argumentModifications = modification.argument_mods;
        foreach (ArgumentModification argumentModification, argumentModifications) {
            if (argumentModification.index == key
                && !argumentModification.modified_type.isEmpty()) {
                return argumentModification.modified_type;
            }
        }
    }

    return QString();
}

QString AbstractMetaFunction::minimalSignature() const
{
    if (!m_cachedMinimalSignature.isEmpty())
        return m_cachedMinimalSignature;

    QString minimalSignature = originalName() + "(";
    AbstractMetaArgumentList arguments = this->arguments();

    for (int i = 0; i < arguments.count(); ++i) {
        AbstractMetaType *t = arguments.at(i)->type();

        if (i > 0)
            minimalSignature += ",";

        minimalSignature += t->minimalSignature();
    }
    minimalSignature += ")";
    if (isConstant())
        minimalSignature += "const";

    minimalSignature = TypeDatabase::normalizedSignature(minimalSignature.toLocal8Bit().constData());
    m_cachedMinimalSignature = minimalSignature;

    return minimalSignature;
}

FunctionModificationList AbstractMetaFunction::modifications(const AbstractMetaClass* implementor) const
{
    if (!implementor)
        implementor = ownerClass();

    if (!implementor)
        return TypeDatabase::instance()->functionModifications(minimalSignature());

    FunctionModificationList mods;
    while (implementor) {
        mods += implementor->typeEntry()->functionModifications(minimalSignature());
        if ((implementor == implementor->baseClass()) ||
            (implementor == implementingClass() && (mods.size() > 0)))
                break;
        foreach (const AbstractMetaClass* interface, implementor->interfaces()) {
            mods += this->modifications(interface);
        }
        implementor = implementor->baseClass();
    }
    return mods;
}

bool AbstractMetaFunction::hasModifications(const AbstractMetaClass *implementor) const
{
    return !modifications(implementor).isEmpty();
}

QString AbstractMetaFunction::argumentName(int index, bool create, const AbstractMetaClass *implementor) const
{
    return m_arguments[--index]->name();
}

bool AbstractMetaFunction::isCallOperator() const
{
    return m_name == "operator()";
}

bool AbstractMetaFunction::hasInjectedCode() const
{
    foreach (const FunctionModification mod, modifications(ownerClass())) {
        if (mod.isCodeInjection())
            return true;
    }
    return false;
}

CodeSnipList AbstractMetaFunction::injectedCodeSnips(CodeSnip::Position position, TypeSystem::Language language) const
{
    CodeSnipList result;
    foreach (const FunctionModification mod, modifications(ownerClass())) {
        if (mod.isCodeInjection()) {
            QList<CodeSnip>::const_iterator it = mod.snips.constBegin();
            for (;it != mod.snips.constEnd(); ++it) {
                if ((it->language & language) && (it->position == position || position == CodeSnip::Any))
                    result << *it;
            }
        }
    }
    return result;
}

bool AbstractMetaFunction::hasSignatureModifications() const
{
    foreach (const FunctionModification mod, modifications()) {
        if (mod.isRenameModifier())
            return true;
        foreach (const ArgumentModification argmod, mod.argument_mods) {
            // since zero represents the return type and we're
            // interested only in checking the function arguments,
            // it will be ignored.
            if (argmod.index > 0)
                return true;
        }
    }
    return false;
}

bool AbstractMetaFunction::isConversionOperator(QString funcName)
{
    static QRegExp opRegEx("^operator(?:\\s+(?:const|volatile))?\\s+(\\w+\\s*)&?$");
    return opRegEx.indexIn(funcName) > -1;
}

bool AbstractMetaFunction::isOperatorOverload(QString funcName)
{
    if (isConversionOperator(funcName))
        return true;

    static QRegExp opRegEx("^operator([+\\-\\*/%=&\\|\\^\\<>!][=]?"
                    "|\\+\\+|\\-\\-|&&|\\|\\||<<[=]?|>>[=]?|~"
                    "|\\[\\]|\\s+delete\\[?\\]?"
                    "|\\(\\)"
                    "|\\s+new\\[?\\]?)$");
    return opRegEx.indexIn(funcName) > -1;
}

bool AbstractMetaFunction::isCastOperator() const
{
    return originalName().startsWith("operator ");
}

bool AbstractMetaFunction::isArithmeticOperator() const
{
    if (!isOperatorOverload())
        return false;

    QString name = originalName();

    // It's a dereference operator!
    if (name == "operator*" && m_arguments.isEmpty())
        return false;

    return name == "operator+" || name == "operator+="
            || name == "operator-" || name == "operator-="
            || name == "operator*" || name == "operator*="
            || name == "operator/" || name == "operator/="
            || name == "operator%" || name == "operator%="
            || name == "operator++" || name == "operator--";
}

bool AbstractMetaFunction::isBitwiseOperator() const
{
    if (!isOperatorOverload())
        return false;

    QString name = originalName();
    return name == "operator<<" || name == "operator<<="
            || name == "operator>>" || name == "operator>>="
            || name == "operator&" || name == "operator&="
            || name == "operator|" || name == "operator|="
            || name == "operator^" || name == "operator^="
            || name == "operator~";
}

bool AbstractMetaFunction::isComparisonOperator() const
{
    if (!isOperatorOverload())
        return false;

    QString name = originalName();
    return name == "operator<" || name == "operator<="
            || name == "operator>" || name == "operator>="
            || name == "operator==" || name == "operator!=";
}

bool AbstractMetaFunction::isLogicalOperator() const
{
    if (!isOperatorOverload())
        return false;

    QString name = originalName();
    return name == "operator!"
            || name == "operator&&"
            || name == "operator||";
}

bool AbstractMetaFunction::isSubscriptOperator() const
{
    if (!isOperatorOverload())
        return false;

    return originalName() == "operator[]";
}

bool AbstractMetaFunction::isAssignmentOperator() const
{
    if (!isOperatorOverload())
        return false;

    return originalName() == "operator=";
}

bool AbstractMetaFunction::isOtherOperator() const
{
    if (!isOperatorOverload())
        return false;

    return !isArithmeticOperator()
            && !isBitwiseOperator()
            && !isComparisonOperator()
            && !isLogicalOperator()
            && !isConversionOperator()
            && !isSubscriptOperator()
            && !isAssignmentOperator();
}

int AbstractMetaFunction::arityOfOperator() const
{
    if (!isOperatorOverload() || isCallOperator())
        return -1;

    int arity = m_arguments.size();

    // Operator overloads that are class members
    // implicitly includes the instance and have
    // one parameter less than their arity,
    // so we increment it.
    if (ownerClass() && arity < 2)
        arity++;

    return arity;
}

bool AbstractMetaFunction::isInplaceOperator() const
{
    if (!isOperatorOverload())
        return false;

    QString name = originalName();
    return name == "operator+=" || name == "operator&="
           || name == "operator-=" || name == "operator|="
           || name == "operator*=" || name == "operator^="
           || name == "operator/=" || name == "operator<<="
           || name == "operator%=" || name == "operator>>=";
}

bool AbstractMetaFunction::isVirtual() const
{
    return !isFinal() && !isSignal() && !isStatic() && !isFinalInCpp() && !isConstructor();
}

bool AbstractMetaFunction::isCopyConstructor() const
{
    if (!ownerClass() || !isConstructor() || arguments().count() != 1)
        return false;

    const AbstractMetaType* type = arguments().first()->type();
    return type->typeEntry() == ownerClass()->typeEntry() &&
           type->isConstant() && type->isReference();
}

QString AbstractMetaFunction::modifiedName() const
{
    if (m_cachedModifiedName.isEmpty()) {
        FunctionModificationList mods = modifications(implementingClass());
        foreach (FunctionModification mod, mods) {
            if (mod.isRenameModifier()) {
                m_cachedModifiedName = mod.renamedToName;
                break;
            }
        }
        if (m_cachedModifiedName.isEmpty())
            m_cachedModifiedName = name();
    }
    return m_cachedModifiedName;
}

QString AbstractMetaFunction::targetLangSignature(bool minimal) const
{
    QString s;

    // Attributes...
    if (!minimal) {
        // Return type
        if (type())
            s += type()->name() + " ";
        else
            s += "void ";
    }

    s += modifiedName();
    s += "(";

    int j = 0;
    for (int i = 0; i < m_arguments.size(); ++i) {
        if (argumentRemoved(i + 1))
            continue;
        if (j) {
            s += ",";
            if (!minimal)
                s += QLatin1Char(' ');
        }
        s += m_arguments.at(i)->type()->name();

        if (!minimal) {
            s += " ";
            s += m_arguments.at(i)->name();
        }
        ++j;
    }

    s += ")";

    return s;
}


bool function_sorter(AbstractMetaFunction *a, AbstractMetaFunction *b)
{
    return a->signature() < b->signature();
}

/*******************************************************************************
 * AbstractMetaClass
 */
AbstractMetaClass::~AbstractMetaClass()
{
    qDeleteAll(m_functions);
    qDeleteAll(m_fields);
    qDeleteAll(m_enums);
    qDeleteAll(m_orphanInterfaces);
    if (hasTemplateBaseClassInstantiations()) {
        foreach (AbstractMetaType* inst, templateBaseClassInstantiations())
            delete inst;
    }
}

/*******************************************************************************
 * Returns true if this class is a subclass of the given class
 */
bool AbstractMetaClass::inheritsFrom(const AbstractMetaClass *cls) const
{
    Q_ASSERT(cls);

    const AbstractMetaClass *clazz = this;
    while (clazz) {
        if (clazz == cls)
            return true;

        clazz = clazz->baseClass();
    }

    return false;
}

/*******************************************************************************
 * Constructs an interface based on the functions and enums in this
 * class and returns it...
 */
AbstractMetaClass *AbstractMetaClass::extractInterface()
{
    Q_ASSERT(typeEntry()->designatedInterface());

    if (!m_extractedInterface) {
        AbstractMetaClass *iface = new AbstractMetaClass;
        iface->setAttributes(attributes());
        iface->setBaseClass(0);
        iface->setPrimaryInterfaceImplementor(this);

        iface->setTypeEntry(typeEntry()->designatedInterface());

        foreach (AbstractMetaFunction *function, functions()) {
            if (!function->isConstructor())
                iface->addFunction(function->copy());
        }

//         iface->setEnums(enums());
//         setEnums(AbstractMetaEnumList());

        foreach (const AbstractMetaField *field, fields()) {
            if (field->isPublic()) {
                AbstractMetaField *new_field = field->copy();
                new_field->setEnclosingClass(iface);
                iface->addField(new_field);
            }
        }

        m_extractedInterface = iface;
        addInterface(iface);
        m_orphanInterfaces << iface;
    }

    return m_extractedInterface;
}

/*******************************************************************************
 * Returns a list of all the functions with a given name
 */
AbstractMetaFunctionList AbstractMetaClass::queryFunctionsByName(const QString &name) const
{
    AbstractMetaFunctionList returned;
    AbstractMetaFunctionList functions = this->functions();
    foreach (AbstractMetaFunction *function, functions) {
        if (function->name() == name)
            returned.append(function);
    }

    return returned;
}

/*******************************************************************************
 * Returns all reference count modifications for any function in the class
 */
QList<ReferenceCount> AbstractMetaClass::referenceCounts() const
{
    QList<ReferenceCount> returned;

    AbstractMetaFunctionList functions = this->functions();
    foreach (AbstractMetaFunction *function, functions)
        returned += function->referenceCounts(this);

    return returned;
}

/*******************************************************************************
 * Returns a list of all the functions retrieved during parsing which should
 * be added to the API.
 */
AbstractMetaFunctionList AbstractMetaClass::functionsInTargetLang() const
{
    int default_flags = NormalFunctions | Visible | NotRemovedFromTargetLang;

    // Interfaces don't implement functions
    default_flags |= isInterface() ? 0 : ClassImplements;

    // Only public functions in final classes
    // default_flags |= isFinal() ? WasPublic : 0;
    int public_flags = isFinal() ? WasPublic : 0;

    // Constructors
    AbstractMetaFunctionList returned = queryFunctions(Constructors | default_flags | public_flags);

    // Final functions
    returned += queryFunctions(FinalInTargetLangFunctions | NonStaticFunctions | default_flags | public_flags);

    // Virtual functions
    returned += queryFunctions(VirtualInTargetLangFunctions | NonStaticFunctions | default_flags | public_flags);

    // Static functions
    returned += queryFunctions(StaticFunctions | default_flags | public_flags);

    // Empty, private functions, since they aren't caught by the other ones
    returned += queryFunctions(Empty | Invisible);

    return returned;
}

AbstractMetaFunctionList AbstractMetaClass::virtualFunctions() const
{
    AbstractMetaFunctionList list = functionsInShellClass();

    AbstractMetaFunctionList returned;
    foreach (AbstractMetaFunction *f, list) {
        if (!f->isFinalInCpp() || f->isVirtualSlot())
            returned += f;
    }

    return returned;
}

AbstractMetaFunctionList AbstractMetaClass::nonVirtualShellFunctions() const
{
    AbstractMetaFunctionList list = functionsInShellClass();
    AbstractMetaFunctionList returned;
    foreach (AbstractMetaFunction *f, list) {
        if (f->isFinalInCpp() && !f->isVirtualSlot())
            returned += f;
    }

    return returned;
}

AbstractMetaFunctionList AbstractMetaClass::implicitConversions() const
{
    if (!hasCloneOperator() && !hasExternalConversionOperators())
        return AbstractMetaFunctionList();

    AbstractMetaFunctionList returned;
    AbstractMetaFunctionList list = queryFunctions(Constructors);
    list.append(externalConversionOperators());

    foreach (AbstractMetaFunction *f, list) {
        if ((f->actualMinimumArgumentCount() == 1 || f->arguments().size() == 1 || f->isConversionOperator())
            && !f->isExplicit()
            && !f->isCopyConstructor()
            && !f->isModifiedRemoved()
            && (f->originalAttributes() & Public)) {
            returned += f;
        }
    }
    return returned;
}

AbstractMetaFunctionList AbstractMetaClass::operatorOverloads(uint query) const
{
    AbstractMetaFunctionList list = queryFunctions(OperatorOverloads | Visible);
    AbstractMetaFunctionList returned;
    foreach (AbstractMetaFunction *f, list) {
        if (((query & ArithmeticOp) && f->isArithmeticOperator())
            || ((query & BitwiseOp) && f->isBitwiseOperator())
            || ((query & ComparisonOp) && f->isComparisonOperator())
            || ((query & LogicalOp) && f->isLogicalOperator())
            || ((query & SubscriptionOp) && f->isSubscriptOperator())
            || ((query & AssignmentOp) && f->isAssignmentOperator())
            || ((query & ConversionOp) && f->isConversionOperator())
            || ((query & OtherOp) && f->isOtherOperator()))
            returned += f;
    }

    return returned;
}

bool AbstractMetaClass::hasOperatorOverload() const
{
    foreach (const AbstractMetaFunction *f, m_functions) {
        if (f->ownerClass() == f->implementingClass() && f->isOperatorOverload() && !f->isPrivate())
            return true;
    }
    return false;
}

bool AbstractMetaClass::hasArithmeticOperatorOverload() const
{
    foreach (const AbstractMetaFunction *f, m_functions) {
        if (f->ownerClass() == f->implementingClass() && f->isArithmeticOperator() && !f->isPrivate())
            return true;
    }
    return false;
}

bool AbstractMetaClass::hasBitwiseOperatorOverload() const
{
    foreach (const AbstractMetaFunction *f, m_functions) {
        if (f->ownerClass() == f->implementingClass() && f->isBitwiseOperator() && !f->isPrivate())
            return true;
    }
    return false;
}

bool AbstractMetaClass::hasComparisonOperatorOverload() const
{
    foreach (const AbstractMetaFunction *f, m_functions) {
        if (f->ownerClass() == f->implementingClass() && f->isComparisonOperator() && !f->isPrivate())
            return true;
    }
    return false;
}

bool AbstractMetaClass::hasLogicalOperatorOverload() const
{
    foreach (const AbstractMetaFunction *f, m_functions) {
        if (f->ownerClass() == f->implementingClass() && f->isLogicalOperator() && !f->isPrivate())
            return true;
    }
    return false;
}

bool AbstractMetaClass::hasSubscriptOperatorOverload() const
{
    foreach (const AbstractMetaFunction *f, m_functions) {
        if (f->ownerClass() == f->implementingClass() && f->isSubscriptOperator() && !f->isPrivate())
            return true;
    }
    return false;
}

bool AbstractMetaClass::hasAssignmentOperatorOverload() const
{
    foreach (const AbstractMetaFunction *f, m_functions) {
        if (f->ownerClass() == f->implementingClass() && f->isAssignmentOperator() && !f->isPrivate())
            return true;
    }
    return false;
}

bool AbstractMetaClass::hasConversionOperatorOverload() const
{
    foreach (const AbstractMetaFunction *f, m_functions) {
        if (f->ownerClass() == f->implementingClass() && f->isConversionOperator() && !f->isPrivate())
            return true;
    }
    return false;
}

/*******************************************************************************
 * Returns a list of all functions that should be declared and implemented in
 * the shell class which is generated as a wrapper on top of the actual C++ class
 */
AbstractMetaFunctionList AbstractMetaClass::functionsInShellClass() const
{
    // Only functions and only protected and public functions
    int default_flags = NormalFunctions | Visible | WasVisible | NotRemovedFromShell;

    // All virtual functions
    AbstractMetaFunctionList returned = queryFunctions(VirtualFunctions | default_flags);

    // All functions explicitly set to be implemented by the shell class
    // (mainly superclass functions that are hidden by other declarations)
    returned += queryFunctions(ForcedShellFunctions | default_flags);

    // All functions explicitly set to be virtual slots
    returned += queryFunctions(VirtualSlots | default_flags);

    return returned;
}

/*******************************************************************************
 * Returns a list of all functions that require a public override function to
 * be generated in the shell class. This includes all functions that were originally
 * protected in the superclass.
 */
AbstractMetaFunctionList AbstractMetaClass::publicOverrideFunctions() const
{
    return queryFunctions(NormalFunctions | WasProtected | FinalInCppFunctions | NotRemovedFromTargetLang)
           + queryFunctions(Signals | WasProtected | FinalInCppFunctions | NotRemovedFromTargetLang);
}

AbstractMetaFunctionList AbstractMetaClass::virtualOverrideFunctions() const
{
    return queryFunctions(NormalFunctions | NonEmptyFunctions | Visible | VirtualInCppFunctions | NotRemovedFromShell) +
           queryFunctions(Signals | NonEmptyFunctions | Visible | VirtualInCppFunctions | NotRemovedFromShell);
}

void AbstractMetaClass::sortFunctions()
{
    qSort(m_functions.begin(), m_functions.end(), function_sorter);
}

void AbstractMetaClass::setFunctions(const AbstractMetaFunctionList &functions)
{
    m_functions = functions;

    // Functions must be sorted by name before next loop
    sortFunctions();

    QString currentName;
    bool hasVirtuals = false;
    AbstractMetaFunctionList finalFunctions;
    foreach (AbstractMetaFunction *f, m_functions) {
        f->setOwnerClass(this);

        m_hasVirtualSlots = m_hasVirtualSlots || f->isVirtualSlot();
        m_hasVirtuals = m_hasVirtuals || f->isVirtualSlot() || hasVirtualDestructor();
        m_isPolymorphic = m_isPolymorphic || m_hasVirtuals;
        m_hasNonpublic = m_hasNonpublic || !f->isPublic();

        // If we have non-virtual overloads of a virtual function, we have to implement
        // all the overloads in the shell class to override the hiding rule
        if (currentName == f->name()) {
            hasVirtuals = hasVirtuals || !f->isFinal();
            if (f->isFinal())
                finalFunctions += f;
        } else {
            if (hasVirtuals && finalFunctions.size() > 0) {
                foreach (AbstractMetaFunction *final_function, finalFunctions) {
                    *final_function += AbstractMetaAttributes::ForceShellImplementation;

                    QString warn = QString("hiding of function '%1' in class '%2'")
                                   .arg(final_function->name()).arg(name());
                    ReportHandler::warning(warn);
                }
            }

            hasVirtuals = !f->isFinal();
            finalFunctions.clear();
            if (f->isFinal())
                finalFunctions += f;
            currentName = f->name();
        }
    }
}

bool AbstractMetaClass::hasFieldAccessors() const
{
    foreach (const AbstractMetaField *field, fields()) {
        if (field->getter() || field->setter())
            return true;
    }

    return false;
}

bool AbstractMetaClass::hasDefaultToStringFunction() const
{
    foreach (AbstractMetaFunction *f, queryFunctionsByName("toString")) {
        if (!f->actualMinimumArgumentCount())
            return true;
    }
    return false;
}

void AbstractMetaClass::addFunction(AbstractMetaFunction *function)
{
    Q_ASSERT(!function->signature().startsWith("("));
    function->setOwnerClass(this);

    if (!function->isDestructor())
        m_functions << function;
    else
        Q_ASSERT(false); //memory leak

    m_hasVirtualSlots |= function->isVirtualSlot();
    m_hasVirtuals |= !function->isFinal() || function->isVirtualSlot() || hasVirtualDestructor();
    m_isPolymorphic |= m_hasVirtuals;
    m_hasNonpublic |= !function->isPublic();
}

bool AbstractMetaClass::hasSignal(const AbstractMetaFunction *other) const
{
    if (!other->isSignal())
        return false;

    foreach (const AbstractMetaFunction *f, functions()) {
        if (f->isSignal() && f->compareTo(other) & AbstractMetaFunction::EqualName)
            return other->modifiedName() == f->modifiedName();
    }

    return false;
}


QString AbstractMetaClass::name() const
{
    return QString(m_typeEntry->targetLangName()).split("::").last();
}

void AbstractMetaClass::setBaseClass(AbstractMetaClass *baseClass)
{
    m_baseClass = baseClass;
    if (baseClass)
        m_isPolymorphic |= baseClass->isPolymorphic();
}

bool AbstractMetaClass::hasFunction(const QString &str) const
{
    return findFunction(str);
}

const AbstractMetaFunction* AbstractMetaClass::findFunction(const QString& functionName) const
{
    foreach (const AbstractMetaFunction *f, functions()) {
        if (f->name() == functionName)
            return f;
    }
    return 0;
}

bool AbstractMetaClass::hasProtectedFunctions() const
{
    foreach (AbstractMetaFunction *func, m_functions) {
        if (func->isProtected())
            return true;
    }
    return false;
}

bool AbstractMetaClass::hasProtectedFields() const
{
    foreach (const AbstractMetaField *field, fields()) {
        if (field->isProtected())
            return true;
    }
    return false;
}

bool AbstractMetaClass::hasProtectedMembers() const
{
    return hasProtectedFields() || hasProtectedFunctions();
}

bool AbstractMetaClass::generateShellClass() const
{
    return m_forceShellClass ||
           (!isFinal()
            && (hasVirtualFunctions()
                || hasProtectedFunctions()
                || hasFieldAccessors()));
}

QPropertySpec *AbstractMetaClass::propertySpecForRead(const QString &name) const
{
    for (int i = 0; i < m_propertySpecs.size(); ++i)
        if (name == m_propertySpecs.at(i)->read())
            return m_propertySpecs.at(i);
    return 0;
}

QPropertySpec *AbstractMetaClass::propertySpecForWrite(const QString &name) const
{
    for (int i = 0; i < m_propertySpecs.size(); ++i)
        if (name == m_propertySpecs.at(i)->write())
            return m_propertySpecs.at(i);
    return 0;
}

QPropertySpec *AbstractMetaClass::propertySpecForReset(const QString &name) const
{
    for (int i = 0; i < m_propertySpecs.size(); ++i) {
        if (name == m_propertySpecs.at(i)->reset())
            return m_propertySpecs.at(i);
    }
    return 0;
}

typedef QHash<const AbstractMetaClass*, AbstractMetaTypeList> AbstractMetaClassBaseTemplateInstantiationsMap;
Q_GLOBAL_STATIC(AbstractMetaClassBaseTemplateInstantiationsMap, metaClassBaseTemplateInstantiations);

bool AbstractMetaClass::hasTemplateBaseClassInstantiations() const
{
    if (!templateBaseClass())
        return false;
    return metaClassBaseTemplateInstantiations()->contains(this);
}

AbstractMetaTypeList AbstractMetaClass::templateBaseClassInstantiations() const
{
    if (!templateBaseClass())
        return AbstractMetaTypeList();
    return metaClassBaseTemplateInstantiations()->value(this);
}

void AbstractMetaClass::setTemplateBaseClassInstantiations(AbstractMetaTypeList& instantiations)
{
    if (!templateBaseClass())
        return;
    metaClassBaseTemplateInstantiations()->insert(this, instantiations);
}

static bool functions_contains(const AbstractMetaFunctionList &l, const AbstractMetaFunction *func)
{
    foreach (const AbstractMetaFunction *f, l) {
        if ((f->compareTo(func) & AbstractMetaFunction::PrettySimilar) == AbstractMetaFunction::PrettySimilar)
            return true;
    }
    return false;
}

AbstractMetaField::AbstractMetaField() : m_getter(0), m_setter(0), m_class(0)
{
}

AbstractMetaField::~AbstractMetaField()
{
    delete m_setter;
    delete m_getter;
}

AbstractMetaField *AbstractMetaField::copy() const
{
    AbstractMetaField *returned = new AbstractMetaField;
    returned->setEnclosingClass(0);
    returned->setAttributes(attributes());
    returned->setName(name());
    returned->setType(type()->copy());
    returned->setOriginalAttributes(originalAttributes());

    return returned;
}

/*******************************************************************************
 * Indicates that this field has a modification that removes it
 */
bool AbstractMetaField::isModifiedRemoved(int types) const
{
    FieldModificationList mods = modifications();
    foreach (FieldModification mod, mods) {
        if (!mod.isRemoveModifier())
            continue;

        if ((mod.removal & types) == types)
            return true;
    }

    return false;
}

static QString upCaseFirst(const QString &str)
{
    Q_ASSERT(!str.isEmpty());
    QString s = str;
    s[0] = s.at(0).toUpper();
    return s;
}

static AbstractMetaFunction *createXetter(const AbstractMetaField *g, const QString &name, uint type)
{
    AbstractMetaFunction *f = new AbstractMetaFunction;

    f->setName(name);
    f->setOriginalName(name);
    f->setOwnerClass(g->enclosingClass());
    f->setImplementingClass(g->enclosingClass());
    f->setDeclaringClass(g->enclosingClass());

    uint attr = AbstractMetaAttributes::Native
                | AbstractMetaAttributes::Final
                | type;
    if (g->isStatic())
        attr |= AbstractMetaAttributes::Static;
    if (g->isPublic())
        attr |= AbstractMetaAttributes::Public;
    else if (g->isProtected())
        attr |= AbstractMetaAttributes::Protected;
    else
        attr |= AbstractMetaAttributes::Private;
    f->setAttributes(attr);
    f->setOriginalAttributes(attr);

    FieldModificationList mods = g->modifications();
    foreach (FieldModification mod, mods) {
        if (mod.isRenameModifier())
            f->setName(mod.renamedTo());
        if (mod.isAccessModifier()) {
            if (mod.isPrivate())
                f->setVisibility(AbstractMetaAttributes::Private);
            else if (mod.isProtected())
                f->setVisibility(AbstractMetaAttributes::Protected);
            else if (mod.isPublic())
                f->setVisibility(AbstractMetaAttributes::Public);
            else if (mod.isFriendly())
                f->setVisibility(AbstractMetaAttributes::Friendly);
        }
    }
    return f;
}

FieldModificationList AbstractMetaField::modifications() const
{
    FieldModificationList mods = enclosingClass()->typeEntry()->fieldModifications();
    FieldModificationList returned;

    foreach (FieldModification mod, mods) {
        if (mod.name == name())
            returned += mod;
    }

    return returned;
}

const AbstractMetaFunction *AbstractMetaField::setter() const
{
    if (!m_setter) {
        m_setter = createXetter(this,
                                "set" + upCaseFirst(name()),
                                AbstractMetaAttributes::SetterFunction);
        AbstractMetaArgumentList arguments;
        AbstractMetaArgument *argument = new AbstractMetaArgument;
        argument->setType(type()->copy());
        argument->setName(name());
        arguments.append(argument);
        m_setter->setArguments(arguments);
    }
    return m_setter;
}

const AbstractMetaFunction *AbstractMetaField::getter() const
{
    if (!m_getter) {
        m_getter = createXetter(this,
                                name(),
                                AbstractMetaAttributes::GetterFunction);
        m_getter->setType(type());
    }

    return m_getter;
}


bool AbstractMetaClass::hasConstructors() const
{
    return queryFunctions(Constructors).size();
}

bool AbstractMetaClass::hasCopyConstructor() const
{
    foreach (const AbstractMetaFunction* ctor, queryFunctions(Constructors)) {
        if (ctor->isCopyConstructor())
            return true;
    }
    return false;
}

bool AbstractMetaClass::hasPrivateCopyConstructor() const
{
    foreach (const AbstractMetaFunction* ctor, queryFunctions(Constructors)) {
        if (ctor->isCopyConstructor() && ctor->isPrivate())
            return true;
    }
    return false;
}

void AbstractMetaClass::addDefaultConstructor()
{
    AbstractMetaFunction *f = new AbstractMetaFunction;
    f->setOriginalName(name());
    f->setName(name());
    f->setOwnerClass(this);
    f->setFunctionType(AbstractMetaFunction::ConstructorFunction);
    f->setArguments(AbstractMetaArgumentList());
    f->setDeclaringClass(this);

    uint attr = AbstractMetaAttributes::Native;
    attr |= AbstractMetaAttributes::Public;
    attr |= AbstractMetaAttributes::Final;
    f->setAttributes(attr);
    f->setImplementingClass(this);
    f->setOriginalAttributes(f->attributes());

    addFunction(f);
    this->setHasNonPrivateConstructor(true);
}

void AbstractMetaClass::addDefaultCopyConstructor(bool isPrivate)
{
    AbstractMetaFunction* f = new AbstractMetaFunction;
    f->setOriginalName(name());
    f->setName(name());
    f->setOwnerClass(this);
    f->setFunctionType(AbstractMetaFunction::ConstructorFunction);
    f->setDeclaringClass(this);

    AbstractMetaType* argType = new AbstractMetaType;
    argType->setTypeEntry(typeEntry());
    argType->setReference(true);
    argType->setConstant(true);
    argType->setTypeUsagePattern(AbstractMetaType::ValuePattern);

    AbstractMetaArgument* arg = new AbstractMetaArgument;
    arg->setType(argType);
    arg->setName(name());
    f->addArgument(arg);

    uint attr = AbstractMetaAttributes::Native;
    attr |= AbstractMetaAttributes::Final;
    if (isPrivate)
        attr |= AbstractMetaAttributes::Private;
    else
        attr |= AbstractMetaAttributes::Public;
    f->setAttributes(attr);
    f->setImplementingClass(this);
    f->setOriginalAttributes(f->attributes());

    addFunction(f);
}

bool AbstractMetaClass::hasFunction(const AbstractMetaFunction *f) const
{
    return functions_contains(m_functions, f);
}

/* Goes through the list of functions and returns a list of all
   functions matching all of the criteria in \a query.
 */

AbstractMetaFunctionList AbstractMetaClass::queryFunctions(uint query) const
{
    AbstractMetaFunctionList functions;

    foreach (AbstractMetaFunction *f, m_functions) {

        if ((query & VirtualSlots) && !f->isVirtualSlot())
            continue;

        if ((query & NotRemovedFromTargetLang) && f->isRemovedFrom(f->implementingClass(), TypeSystem::TargetLangCode))
            continue;

        if ((query & NotRemovedFromTargetLang) && !f->isFinal() && f->isRemovedFrom(f->declaringClass(), TypeSystem::TargetLangCode))
            continue;

        if ((query & NotRemovedFromShell) && f->isRemovedFrom(f->implementingClass(), TypeSystem::ShellCode))
            continue;

        if ((query & NotRemovedFromShell) && !f->isFinal() && f->isRemovedFrom(f->declaringClass(), TypeSystem::ShellCode))
            continue;

        if ((query & Visible) && f->isPrivate())
            continue;

        if ((query & VirtualInTargetLangFunctions) && f->isFinalInTargetLang())
            continue;

        if ((query & Invisible) && !f->isPrivate())
            continue;

        if ((query & Empty) && !f->isEmptyFunction())
            continue;

        if ((query & WasPublic) && !f->wasPublic())
            continue;

        if ((query & WasVisible) && f->wasPrivate())
            continue;

        if ((query & WasProtected) && !f->wasProtected())
            continue;

        if ((query & ClassImplements) && f->ownerClass() != f->implementingClass())
            continue;

        if ((query & Inconsistent) && (f->isFinalInTargetLang() || !f->isFinalInCpp() || f->isStatic()))
            continue;

        if ((query & FinalInTargetLangFunctions) && !f->isFinalInTargetLang())
            continue;

        if ((query & FinalInCppFunctions) && !f->isFinalInCpp())
            continue;

        if ((query & VirtualInCppFunctions) && f->isFinalInCpp())
            continue;

        if ((query & Signals) && (!f->isSignal()))
            continue;

        if ((query & ForcedShellFunctions) &&
            (!f->isForcedShellImplementation() || !f->isFinal())) {
            continue;
        }

        if ((query & Constructors) && (!f->isConstructor() || f->ownerClass() != f->implementingClass()))
            continue;

        if (!(query & Constructors) && f->isConstructor())
            continue;

        // Destructors are never included in the functions of a class currently
        /*
           if ((query & Destructors) && (!f->isDestructor()
                                       || f->ownerClass() != f->implementingClass())
            || f->isDestructor() && (query & Destructors) == 0) {
            continue;
        }*/

        if ((query & VirtualFunctions) && (f->isFinal() || f->isSignal() || f->isStatic()))
            continue;

        if ((query & StaticFunctions) && (!f->isStatic() || f->isSignal()))
            continue;

        if ((query & NonStaticFunctions) && (f->isStatic()))
            continue;

        if ((query & NonEmptyFunctions) && (f->isEmptyFunction()))
            continue;

        if ((query & NormalFunctions) && (f->isSignal()))
            continue;

        if ((query & AbstractFunctions) && !f->isAbstract())
            continue;

        if ((query & OperatorOverloads) && !f->isOperatorOverload())
            continue;

        functions << f;
    }

    return functions;
}


bool AbstractMetaClass::hasInconsistentFunctions() const
{
    return cppInconsistentFunctions().size() > 0;
}

bool AbstractMetaClass::hasSignals() const
{
    return cppSignalFunctions().size() > 0;
}


/**
 * Adds the specified interface to this class by adding all the
 * functions in the interface to this class.
 */
void AbstractMetaClass::addInterface(AbstractMetaClass *interface)
{
    Q_ASSERT(!m_interfaces.contains(interface));
    m_interfaces << interface;

    m_isPolymorphic |= interface->isPolymorphic();

    if (m_extractedInterface && m_extractedInterface != interface)
        m_extractedInterface->addInterface(interface);

#if 0
    foreach (AbstractMetaFunction *function, interface->functions())
    if (!hasFunction(function) && !function->isConstructor()) {
        AbstractMetaFunction *cpy = function->copy();
        cpy->setImplementingClass(this);

        // Setup that this function is an interface class.
        cpy->setInterfaceClass(interface);
        *cpy += AbstractMetaAttributes::InterfaceFunction;

        // Copy the modifications in interface into the implementing classes.
        FunctionModificationList mods = function->modifications(interface);
        foreach (const FunctionModification &mod, mods)
            m_typeEntry->addFunctionModification(mod);

        // It should be mostly safe to assume that when we implement an interface
        // we don't "pass on" pure virtual functions to our sublcasses...
//             *cpy -= AbstractMetaAttributes::Abstract;

        addFunction(cpy);
    }
#endif

}


void AbstractMetaClass::setInterfaces(const AbstractMetaClassList &interfaces)
{
    m_interfaces = interfaces;
    foreach (const AbstractMetaClass* interface, interfaces) {
        if (interface)
            m_isPolymorphic |= interface->isPolymorphic();
    }
}


AbstractMetaEnum *AbstractMetaClass::findEnum(const QString &enumName)
{
    foreach (AbstractMetaEnum *e, m_enums) {
        if (e->name() == enumName)
            return e;
    }

    if (typeEntry()->designatedInterface())
        return extractInterface()->findEnum(enumName);

    return 0;
}




/*!  Recursivly searches for the enum value named \a enumValueName in
  this class and its superclasses and interfaces. Values belonging to
  \a meta_enum are excluded from the search.
*/
AbstractMetaEnumValue *AbstractMetaClass::findEnumValue(const QString &enumValueName, AbstractMetaEnum *meta_enum)
{
    foreach (AbstractMetaEnum *e, m_enums) {
        if (e != meta_enum)
            continue;
        foreach (AbstractMetaEnumValue *v, e->values()) {
            if (v->name() == enumValueName)
                return v;
        }
    }

    if (typeEntry()->designatedInterface())
        return extractInterface()->findEnumValue(enumValueName, meta_enum);

    if (baseClass())
        return baseClass()->findEnumValue(enumValueName, meta_enum);

    return 0;
}


/*!
 * Searches through all of this class' enums for a value matching the
 * name \a enumValueName. The name is excluding the class/namespace
 * prefix. The function recursivly searches interfaces and baseclasses
 * of this class.
 */
AbstractMetaEnum *AbstractMetaClass::findEnumForValue(const QString &enumValueName)
{
    foreach (AbstractMetaEnum *e, m_enums) {
        foreach (AbstractMetaEnumValue *v, e->values()) {
            if (v->name() == enumValueName)
                return e;
        }
    }

    if (typeEntry()->designatedInterface())
        return extractInterface()->findEnumForValue(enumValueName);

    if (baseClass())
        return baseClass()->findEnumForValue(enumValueName);

    return 0;
}


static void addExtraIncludeForType(AbstractMetaClass *metaClass, const AbstractMetaType *type)
{
    if (!type)
        return;

    Q_ASSERT(metaClass);
    const TypeEntry *entry = (type ? type->typeEntry() : 0);
    if (entry && entry->isComplex()) {
        const ComplexTypeEntry *centry = static_cast<const ComplexTypeEntry *>(entry);
        ComplexTypeEntry *class_entry = metaClass->typeEntry();
        if (class_entry && centry->include().isValid())
            class_entry->addExtraInclude(centry->include());
    }

    if (type->hasInstantiations()) {
        AbstractMetaTypeList instantiations = type->instantiations();
        foreach (const AbstractMetaType *instantiation, instantiations)
            addExtraIncludeForType(metaClass, instantiation);
    }
}

static void addExtraIncludesForFunction(AbstractMetaClass *metaClass, const AbstractMetaFunction *meta_function)
{
    Q_ASSERT(metaClass);
    Q_ASSERT(meta_function);
    addExtraIncludeForType(metaClass, meta_function->type());

    AbstractMetaArgumentList arguments = meta_function->arguments();
    foreach (AbstractMetaArgument *argument, arguments)
    addExtraIncludeForType(metaClass, argument->type());
}

void AbstractMetaClass::fixFunctions()
{
    if (m_functionsFixed)
        return;
    else
        m_functionsFixed = true;

    AbstractMetaClass *superClass = baseClass();
    AbstractMetaFunctionList funcs = functions();

    if (superClass)
        superClass->fixFunctions();
    int iface_idx = 0;
    while (superClass || iface_idx < interfaces().size()) {
        // Since we always traverse the complete hierarchy we are only
        // interrested in what each super class implements, not what
        // we may have propagated from their base classes again.
        AbstractMetaFunctionList superFuncs;
        if (superClass) {
            // Super classes can never be final
            if (superClass->isFinalInTargetLang()) {
                ReportHandler::warning("Final class '" + superClass->name() + "' set to non-final, as it is extended by other classes");
                *superClass -= AbstractMetaAttributes::FinalInTargetLang;
            }
            superFuncs = superClass->queryFunctions(AbstractMetaClass::ClassImplements);
            AbstractMetaFunctionList virtuals = superClass->queryFunctions(AbstractMetaClass::VirtualInCppFunctions);
            superFuncs += virtuals;
        } else {
            superFuncs = interfaces().at(iface_idx)->queryFunctions(AbstractMetaClass::NormalFunctions);
            AbstractMetaFunctionList virtuals = interfaces().at(iface_idx)->queryFunctions(AbstractMetaClass::VirtualInCppFunctions);
            superFuncs += virtuals;
        }

        QSet<AbstractMetaFunction *> funcsToAdd;
        for (int sfi = 0; sfi < superFuncs.size(); ++sfi) {
            AbstractMetaFunction *sf = superFuncs.at(sfi);

            if (sf->isRemovedFromAllLanguages(sf->implementingClass()))
                continue;

            // skip functions added in base classes
            if (sf->isUserAdded() && sf->declaringClass() != this)
                continue;

            // we generally don't care about private functions, but we have to get the ones that are
            // virtual in case they override abstract functions.
            bool add = (sf->isNormal() || sf->isSignal() || sf->isEmptyFunction());
            for (int fi = 0; fi < funcs.size(); ++fi) {
                AbstractMetaFunction *f = funcs.at(fi);
                if (f->isRemovedFromAllLanguages(f->implementingClass()))
                    continue;


                uint cmp = f->compareTo(sf);

                if (cmp & AbstractMetaFunction::EqualModifiedName) {
                    add = false;
                    if (cmp & AbstractMetaFunction::EqualArguments) {
                        // Same function, propegate virtual...
                        if (!(cmp & AbstractMetaFunction::EqualAttributes)) {
                            if (!f->isEmptyFunction()) {
                                if (!sf->isFinalInCpp() && f->isFinalInCpp()) {
                                    *f -= AbstractMetaAttributes::FinalInCpp;
                                }
                                if (!sf->isFinalInTargetLang() && f->isFinalInTargetLang()) {
                                    *f -= AbstractMetaAttributes::FinalInTargetLang;
                                }
#if 0
                                if (!f->isFinalInTargetLang() && f->isPrivate()) {
                                    f->setFunctionType(AbstractMetaFunction::EmptyFunction);
                                    f->setVisibility(AbstractMetaAttributes::Protected);
                                    *f += AbstractMetaAttributes::FinalInTargetLang;
                                    ReportHandler::warning(QString("private virtual function '%1' in '%2'")
                                                           .arg(f->signature())
                                                           .arg(f->implementingClass()->name()));
                                }
#endif
                            }
                        }

                        if (f->visibility() != sf->visibility()) {
                            QString warn = QString("visibility of function '%1' modified in class '%2'")
                                           .arg(f->name()).arg(name());
                            ReportHandler::warning(warn);
#if 0
                            // If new visibility is private, we can't
                            // do anything. If it isn't, then we
                            // prefer the parent class's visibility
                            // setting for the function.
                            if (!f->isPrivate() && !sf->isPrivate())
                                f->setVisibility(sf->visibility());
#endif
                            // Private overrides of abstract functions have to go into the class or
                            // the subclasses will not compile as non-abstract classes.
                            // But they don't need to be implemented, since they can never be called.
                            if (f->isPrivate()) {
                                f->setFunctionType(AbstractMetaFunction::EmptyFunction);
                                *f += AbstractMetaAttributes::FinalInTargetLang;
                                *f += AbstractMetaAttributes::FinalInCpp;
                            }
                        }

                        // Set the class which first declares this function, afawk
                        f->setDeclaringClass(sf->declaringClass());

                        if (sf->isFinalInTargetLang() && !sf->isPrivate() && !f->isPrivate() && !sf->isStatic() && !f->isStatic()) {
                            // Shadowed funcion, need to make base class
                            // function non-virtual
                            if (f->implementingClass() != sf->implementingClass() && f->implementingClass()->inheritsFrom(sf->implementingClass())) {

                                // Check whether the superclass method has been redefined to non-final

                                bool hasNonFinalModifier = false;
                                bool isBaseImplPrivate = false;
                                FunctionModificationList mods = sf->modifications(sf->implementingClass());
                                foreach (FunctionModification mod, mods) {
                                    if (mod.isNonFinal()) {
                                        hasNonFinalModifier = true;
                                        break;
                                    } else if (mod.isPrivate()) {
                                        isBaseImplPrivate = true;
                                        break;
                                    }
                                }

                                if (!hasNonFinalModifier && !isBaseImplPrivate) {
                                    ReportHandler::warning(QString::fromLatin1("Shadowing: %1::%2 and %3::%4")
                                                           .arg(sf->implementingClass()->name())
                                                           .arg(sf->signature())
                                                           .arg(f->implementingClass()->name())
                                                           .arg(f->signature()));
                                }
                            }
                        }

                    }

                    if (cmp & AbstractMetaFunction::EqualDefaultValueOverload) {
                        AbstractMetaArgumentList arguments;
                        if (f->arguments().size() < sf->arguments().size())
                            arguments = sf->arguments();
                        else
                            arguments = f->arguments();
                        //TODO: fix this
                        //for (int i=0; i<arguments.size(); ++i)
                        //    arguments[i]->setDefaultValueExpression("<#>" + QString());
                    }


                    // Otherwise we have function shadowing and we can
                    // skip the thing...
                } else if (cmp & AbstractMetaFunction::EqualName && !sf->isSignal()) {
                    // In the case of function shadowing where the function name has been altered to
                    // avoid conflict, we don't copy in the original.
                    add = false;
                }
            }

            if (add)
                funcsToAdd << sf;
        }

        foreach (AbstractMetaFunction *f, funcsToAdd)
            funcs << f->copy();

        if (superClass)
            superClass = superClass->baseClass();
        else
            iface_idx++;
    }

    bool hasPrivateConstructors = false;
    bool hasPublicConstructors = false;
    foreach (AbstractMetaFunction *func, funcs) {
        FunctionModificationList mods = func->modifications(this);
        foreach (const FunctionModification &mod, mods) {
            if (mod.isRenameModifier()) {
                func->setName(mod.renamedTo());
            }
        }

        // Make sure class is abstract if one of the functions is
        if (func->isAbstract()) {
            (*this) += AbstractMetaAttributes::Abstract;
            (*this) -= AbstractMetaAttributes::Final;
        }

        if (func->isConstructor()) {
            if (func->isPrivate())
                hasPrivateConstructors = true;
            else
                hasPublicConstructors = true;
        }



        // Make sure that we include files for all classes that are in use

        if (!func->isRemovedFrom(this, TypeSystem::ShellCode))
            addExtraIncludesForFunction(this, func);
    }

    if (hasPrivateConstructors && !hasPublicConstructors) {
        (*this) += AbstractMetaAttributes::Abstract;
        (*this) -= AbstractMetaAttributes::Final;
    }

    foreach (AbstractMetaFunction *f1, funcs) {
        foreach (AbstractMetaFunction *f2, funcs) {
            if (f1 != f2) {
                uint cmp = f1->compareTo(f2);
                if ((cmp & AbstractMetaFunction::EqualName)
                    && !f1->isFinalInCpp()
                    && f2->isFinalInCpp()) {
                    *f2 += AbstractMetaAttributes::FinalOverload;
                }
            }
        }
    }

    setFunctions(funcs);
}


QString AbstractMetaType::minimalSignature() const
{
    QString minimalSignature;
    if (isConstant())
        minimalSignature += "const ";
    minimalSignature += typeEntry()->qualifiedCppName();
    if (hasInstantiations()) {
        AbstractMetaTypeList instantiations = this->instantiations();
        minimalSignature += "< ";
        for (int i = 0; i < instantiations.size(); ++i) {
            if (i > 0)
                minimalSignature += ",";
            minimalSignature += instantiations[i]->minimalSignature();
        }
        minimalSignature += " >";
    }

    for (int j = 0; j < indirections(); ++j)
        minimalSignature += "*";
    if (isReference())
        minimalSignature += "&";

    return minimalSignature;
}

bool AbstractMetaType::hasNativeId() const
{
    return (isQObject() || isValue() || isObject()) && typeEntry()->isNativeIdBased();
}


/*******************************************************************************
 * Other stuff...
 */


AbstractMetaEnum *AbstractMetaClassList::findEnum(const EnumTypeEntry *entry) const
{
    Q_ASSERT(entry->isEnum());

    QString qualifiedName = entry->qualifiedCppName();
    int pos = qualifiedName.lastIndexOf("::");

    QString enumName;
    QString className;

    if (pos > 0) {
        enumName = qualifiedName.mid(pos + 2);
        className = qualifiedName.mid(0, pos);
    } else {
        enumName = qualifiedName;
        className = TypeDatabase::globalNamespaceClassName(entry);
    }

    AbstractMetaClass *metaClass = findClass(className);
    if (!metaClass) {
        ReportHandler::warning(QString("AbstractMeta::findEnum(), unknown class '%1' in '%2'")
                               .arg(className).arg(entry->qualifiedCppName()));
        return 0;
    }

    return metaClass->findEnum(enumName);
}

AbstractMetaEnumValue *AbstractMetaEnumValueList::find(const QString &name) const
{
    for (int i = 0; i < size(); ++i) {
        if (name == at(i)->name())
            return at(i);
    }
    return 0;
}

AbstractMetaEnumValue *AbstractMetaClassList::findEnumValue(const QString &name) const
{
    QStringList lst = name.split(QLatin1String("::"));

    if (lst.size() > 1) {
        QString prefixName = lst.at(0);
        QString enumName = lst.at(1);

        AbstractMetaClass* cl = findClass(prefixName);
        if (cl)
            return cl->findEnumValue(enumName, 0);
    }

    foreach(AbstractMetaClass* metaClass, *this) {
        foreach(AbstractMetaEnum* metaEnum, metaClass->enums()) {
            AbstractMetaEnumValue* enumValue = metaClass->findEnumValue(name, metaEnum);
            if (enumValue)
                return enumValue;
        }
    }

    ReportHandler::warning(QString("no matching enum '%1'").arg(name));
    return 0;
}

/*!
 * Searches the list after a class that mathces \a name; either as
 * C++, Target language base name or complete Target language package.class name.
 */

AbstractMetaClass *AbstractMetaClassList::findClass(const QString &name) const
{
    if (name.isEmpty())
        return 0;

    foreach (AbstractMetaClass *c, *this) {
        if (c->qualifiedCppName() == name)
            return c;
    }

    foreach (AbstractMetaClass *c, *this) {
        if (c->fullName() == name)
            return c;
    }

    foreach (AbstractMetaClass *c, *this) {
        if (c->name() == name)
            return c;
    }

    return 0;
}

AbstractMetaClass *AbstractMetaClassList::findClass(const TypeEntry* typeEntry) const
{
    foreach (AbstractMetaClass* c, *this) {
        if (c->typeEntry() == typeEntry)
            return c;
    }
    return 0;
}
