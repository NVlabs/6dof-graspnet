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

#include "generator.h"
#include "reporthandler.h"
#include "fileout.h"
#include "apiextractor.h"

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QDebug>
#include <typedatabase.h>

struct Generator::GeneratorPrivate {
    const ApiExtractor* apiextractor;
    QString outDir;
    // License comment
    QString licenseComment;
    QString packageName;
    int numGenerated;
    int numGeneratedWritten;
    QStringList instantiatedContainersNames;
    QList<const AbstractMetaType*> instantiatedContainers;
};

Generator::Generator() : m_d(new GeneratorPrivate)
{
    m_d->numGenerated = 0;
    m_d->numGeneratedWritten = 0;
    m_d->instantiatedContainers = QList<const AbstractMetaType*>();
    m_d->instantiatedContainersNames = QStringList();
}

Generator::~Generator()
{
    delete m_d;
}

bool Generator::setup(const ApiExtractor& extractor, const QMap< QString, QString > args)
{
    m_d->apiextractor = &extractor;
    TypeEntryHash allEntries = TypeDatabase::instance()->allEntries();
    TypeEntry* entryFound = 0;
    foreach (QList<TypeEntry*> entryList, allEntries.values()) {
        foreach (TypeEntry* entry, entryList) {
            if (entry->type() == TypeEntry::TypeSystemType && entry->generateCode()) {
                entryFound = entry;
                break;
            }
        }
        if (entryFound)
            break;
    }
    if (entryFound)
        m_d->packageName = entryFound->name();
    else
        ReportHandler::warning("Couldn't find the package name!!");

    collectInstantiatedContainers();

    return doSetup(args);
}

QString Generator::getSimplifiedContainerTypeName(const AbstractMetaType* type)
{
    if (!type->typeEntry()->isContainer())
        return type->cppSignature();
    QString typeName = type->cppSignature();
    if (type->isConstant())
        typeName.remove(0, sizeof("const ") / sizeof(char) - 1);
    if (type->isReference())
        typeName.chop(1);
    while (typeName.endsWith('*') || typeName.endsWith(' '))
        typeName.chop(1);
    return typeName;
}

void Generator::addInstantiatedContainers(const AbstractMetaType* type)
{
    if (!type)
        return;
    foreach (const AbstractMetaType* t, type->instantiations())
        addInstantiatedContainers(t);
    if (!type->typeEntry()->isContainer())
        return;
    QString typeName = getSimplifiedContainerTypeName(type);
    if (!m_d->instantiatedContainersNames.contains(typeName)) {
        m_d->instantiatedContainersNames.append(typeName);
        m_d->instantiatedContainers.append(type);
    }
}

void Generator::collectInstantiatedContainers(const AbstractMetaFunction* func)
{
    addInstantiatedContainers(func->type());
    foreach (const AbstractMetaArgument* arg, func->arguments())
        addInstantiatedContainers(arg->type());
}

void Generator::collectInstantiatedContainers(const AbstractMetaClass* metaClass)
{
    if (!metaClass->typeEntry()->generateCode())
        return;
    foreach (const AbstractMetaFunction* func, metaClass->functions())
        collectInstantiatedContainers(func);
    foreach (const AbstractMetaField* field, metaClass->fields())
        addInstantiatedContainers(field->type());
    foreach (AbstractMetaClass* innerClass, metaClass->innerClasses())
        collectInstantiatedContainers(innerClass);
}

void Generator::collectInstantiatedContainers()
{
    foreach (const AbstractMetaFunction* func, globalFunctions())
        collectInstantiatedContainers(func);
    foreach (const AbstractMetaClass* metaClass, classes())
        collectInstantiatedContainers(metaClass);
}

QList<const AbstractMetaType*> Generator::instantiatedContainers() const
{
    return m_d->instantiatedContainers;
}

QMap< QString, QString > Generator::options() const
{
    return QMap<QString, QString>();
}

AbstractMetaClassList Generator::classes() const
{
    return m_d->apiextractor->classes();
}

AbstractMetaClassList Generator::classesTopologicalSorted() const
{
    return m_d->apiextractor->classesTopologicalSorted();
}

AbstractMetaFunctionList Generator::globalFunctions() const
{
    return m_d->apiextractor->globalFunctions();
}

AbstractMetaEnumList Generator::globalEnums() const
{
    return m_d->apiextractor->globalEnums();
}

QList<const PrimitiveTypeEntry*> Generator::primitiveTypes() const
{
    return m_d->apiextractor->primitiveTypes();
}

QList<const ContainerTypeEntry*> Generator::containerTypes() const
{
    return m_d->apiextractor->containerTypes();
}

const AbstractMetaEnum* Generator::findAbstractMetaEnum(const EnumTypeEntry* typeEntry) const
{
    return m_d->apiextractor->findAbstractMetaEnum(typeEntry);
}

const AbstractMetaEnum* Generator::findAbstractMetaEnum(const TypeEntry* typeEntry) const
{
    return m_d->apiextractor->findAbstractMetaEnum(typeEntry);
}

const AbstractMetaEnum* Generator::findAbstractMetaEnum(const FlagsTypeEntry* typeEntry) const
{
    return m_d->apiextractor->findAbstractMetaEnum(typeEntry);
}

const AbstractMetaEnum* Generator::findAbstractMetaEnum(const AbstractMetaType* metaType) const
{
    return m_d->apiextractor->findAbstractMetaEnum(metaType);
}

QSet< QString > Generator::qtMetaTypeDeclaredTypeNames() const
{
    return m_d->apiextractor->qtMetaTypeDeclaredTypeNames();
}

QString Generator::licenseComment() const
{
    return m_d->licenseComment;
}

void Generator::setLicenseComment(const QString& licenseComment)
{
    m_d->licenseComment = licenseComment;
}

QString Generator::packageName() const
{
    return m_d->packageName;
}

QString Generator::moduleName() const
{
    QString& pkgName = m_d->packageName;
    return QString(pkgName).remove(0, pkgName.lastIndexOf('.') + 1);
}

QString Generator::outputDirectory() const
{
    return m_d->outDir;
}

void Generator::setOutputDirectory(const QString &outDir)
{
    m_d->outDir = outDir;
}

int Generator::numGenerated() const
{
    return m_d->numGenerated;
}

int Generator::numGeneratedAndWritten() const
{
    return m_d->numGeneratedWritten;
}

void Generator::generate()
{
    foreach (AbstractMetaClass *cls, m_d->apiextractor->classes()) {
        if (!shouldGenerate(cls))
            continue;

        QString fileName = fileNameForClass(cls);
        if (fileName.isNull())
            continue;
        ReportHandler::debugSparse(QString("generating: %1").arg(fileName));

        FileOut fileOut(outputDirectory() + '/' + subDirectoryForClass(cls) + '/' + fileName);
        generateClass(fileOut.stream, cls);

        if (fileOut.done())
            ++m_d->numGeneratedWritten;
        ++m_d->numGenerated;
    }
    finishGeneration();
}

bool Generator::shouldGenerateTypeEntry(const TypeEntry* type) const
{
    return type->codeGeneration() & TypeEntry::GenerateTargetLang;
}

bool Generator::shouldGenerate(const AbstractMetaClass* metaClass) const
{
    return shouldGenerateTypeEntry(metaClass->typeEntry());
}

void verifyDirectoryFor(const QFile &file)
{
    QDir dir = QFileInfo(file).dir();
    if (!dir.exists()) {
        if (!dir.mkpath(dir.absolutePath()))
            ReportHandler::warning(QString("unable to create directory '%1'")
                                   .arg(dir.absolutePath()));
    }
}

void Generator::replaceTemplateVariables(QString &code, const AbstractMetaFunction *func)
{
    const AbstractMetaClass *cpp_class = func->ownerClass();
    if (cpp_class)
        code.replace("%TYPE", cpp_class->name());

    foreach (AbstractMetaArgument *arg, func->arguments())
        code.replace("%" + QString::number(arg->argumentIndex() + 1), arg->name());

    //template values
    code.replace("%RETURN_TYPE", translateType(func->type(), cpp_class));
    code.replace("%FUNCTION_NAME", func->originalName());

    if (code.contains("%ARGUMENT_NAMES")) {
        QString str;
        QTextStream aux_stream(&str);
        writeArgumentNames(aux_stream, func, Generator::SkipRemovedArguments);
        code.replace("%ARGUMENT_NAMES", str);
    }

    if (code.contains("%ARGUMENTS")) {
        QString str;
        QTextStream aux_stream(&str);
        writeFunctionArguments(aux_stream, func, Options(SkipDefaultValues) | SkipRemovedArguments);
        code.replace("%ARGUMENTS", str);
    }
}

QTextStream& formatCode(QTextStream &s, const QString& code, Indentor &indentor)
{
    // detect number of spaces before the first character
    QStringList lst(code.split("\n"));
    QRegExp nonSpaceRegex("[^\\s]");
    int spacesToRemove = 0;
    foreach(QString line, lst) {
        if (!line.trimmed().isEmpty()) {
            spacesToRemove = line.indexOf(nonSpaceRegex);
            if (spacesToRemove == -1)
                spacesToRemove = 0;
            break;
        }
    }

    static QRegExp emptyLine("\\s*[\\r]?[\\n]?\\s*");

    foreach(QString line, lst) {
        if (!line.isEmpty() && !emptyLine.exactMatch(line)) {
            while (line.end()->isSpace())
                line.chop(1);
            int limit = 0;
            for(int i = 0; i < spacesToRemove; ++i) {
                if (!line[i].isSpace())
                    break;
                limit++;
            }

            s << indentor << line.remove(0, limit);
        }
        s << endl;
    }
    return s;
}

AbstractMetaFunctionList Generator::implicitConversions(const TypeEntry* type) const
{
    if (type->isValue()) {
        const AbstractMetaClass* metaClass = classes().findClass(type);
        if (metaClass)
            return metaClass->implicitConversions();
    }
    return AbstractMetaFunctionList();
}

AbstractMetaFunctionList Generator::implicitConversions(const AbstractMetaType* metaType) const
{
    return implicitConversions(metaType->typeEntry());
}

bool Generator::isObjectType(const TypeEntry* type)
{
    if (type->isComplex())
        return Generator::isObjectType((const ComplexTypeEntry*)type);
    return type->isObject();
}
bool Generator::isObjectType(const ComplexTypeEntry* type)
{
    return type->isObject() || type->isQObject();
}
bool Generator::isObjectType(const AbstractMetaClass* metaClass)
{
    return Generator::isObjectType(metaClass->typeEntry());
}
bool Generator::isObjectType(const AbstractMetaType* metaType)
{
    return isObjectType(metaType->typeEntry());
}

bool Generator::isPointer(const AbstractMetaType* type)
{
    return type->indirections() > 0
            || type->isNativePointer()
            || type->isValuePointer();
}

bool Generator::isCString(const AbstractMetaType* type)
{
    return type->isNativePointer()
            && type->indirections() == 1
            && type->name() == "char";
}

bool Generator::isVoidPointer(const AbstractMetaType* type)
{
    return type->isNativePointer()
            && type->indirections() == 1
            && type->name() == "void";
}

QString Generator::getFullTypeName(const TypeEntry* type) const
{
    return QString("%1%2").arg(type->isCppPrimitive() ? "" : "::").arg(type->qualifiedCppName());
}

QString Generator::getFullTypeName(const AbstractMetaType* type) const
{
    if (isCString(type))
        return "const char*";
    if (isVoidPointer(type))
        return "void*";
    if (type->typeEntry()->isContainer())
        return QString("::%1").arg(type->cppSignature());
    QString typeName;
    if (type->typeEntry()->isComplex() && type->hasInstantiations())
        typeName = getFullTypeNameWithoutModifiers(type);
    else
        typeName = getFullTypeName(type->typeEntry());
    return typeName + QString("*").repeated(type->indirections());
}

QString Generator::getFullTypeName(const AbstractMetaClass* metaClass) const
{
    return QString("::%1").arg(metaClass->qualifiedCppName());
}

QString Generator::getFullTypeNameWithoutModifiers(const AbstractMetaType* type) const
{
    if (isCString(type))
        return "const char*";
    if (isVoidPointer(type))
        return "void*";
    if (!type->hasInstantiations())
        return getFullTypeName(type->typeEntry());
    QString typeName = type->cppSignature();
    if (type->isConstant())
        typeName.remove(0, sizeof("const ") / sizeof(char) - 1);
    if (type->isReference())
        typeName.chop(1);
    while (typeName.endsWith('*') || typeName.endsWith(' '))
        typeName.chop(1);
    return QString("::%1").arg(typeName);
}

QString Generator::minimalConstructor(const AbstractMetaType* type) const
{
    if (!type || (type->isReference() && Generator::isObjectType(type)))
        return QString();

    if (type->isContainer()) {
        QString ctor = type->cppSignature();
        if (ctor.endsWith("*"))
            return QString("0");
        if (ctor.startsWith("const "))
            ctor.remove(0, sizeof("const ") / sizeof(char) - 1);
        if (ctor.endsWith("&")) {
            ctor.chop(1);
            ctor = ctor.trimmed();
        }
        return QString("::%1()").arg(ctor);
    }

    if (type->isNativePointer())
        return QString("((%1*)0)").arg(type->typeEntry()->qualifiedCppName());

    if (Generator::isPointer(type))
        return QString("((::%1*)0)").arg(type->typeEntry()->qualifiedCppName());

    if (type->typeEntry()->isComplex()) {
        const ComplexTypeEntry* cType = reinterpret_cast<const ComplexTypeEntry*>(type->typeEntry());
        QString ctor = cType->defaultConstructor();
        if (!ctor.isEmpty())
            return ctor;
        ctor = minimalConstructor(classes().findClass(cType));
        if (type->hasInstantiations())
            ctor = ctor.replace(getFullTypeName(cType), getFullTypeNameWithoutModifiers(type));
        return ctor;
    }

    return minimalConstructor(type->typeEntry());
}

QString Generator::minimalConstructor(const TypeEntry* type) const
{
    if (!type)
        return QString();

    if (type->isCppPrimitive())
        return QString("((%1)0)").arg(type->qualifiedCppName());

    if (type->isEnum() || type->isFlags())
        return QString("((::%1)0)").arg(type->qualifiedCppName());

    if (type->isPrimitive()) {
        QString ctor = reinterpret_cast<const PrimitiveTypeEntry*>(type)->defaultConstructor();
        // If a non-C++ (i.e. defined by the user) primitive type does not have
        // a default constructor defined by the user, the empty constructor is
        // heuristically returned. If this is wrong the build of the generated
        // bindings will tell.
        return (ctor.isEmpty()) ? QString("::%1()").arg(type->qualifiedCppName()) : ctor;
    }

    if (type->isComplex())
        return minimalConstructor(classes().findClass(type));

    return QString();
}

QString Generator::minimalConstructor(const AbstractMetaClass* metaClass) const
{
    if (!metaClass)
        return QString();

    const ComplexTypeEntry* cType = reinterpret_cast<const ComplexTypeEntry*>(metaClass->typeEntry());
    if (cType->hasDefaultConstructor())
        return cType->defaultConstructor();

    AbstractMetaFunctionList constructors = metaClass->queryFunctions(AbstractMetaClass::Constructors);
    int maxArgs = 0;
    foreach (const AbstractMetaFunction* ctor, constructors) {
        if (ctor->isUserAdded() || ctor->isPrivate() || ctor->isCopyConstructor())
            continue;
        int numArgs = ctor->arguments().size();
        if (numArgs == 0) {
            maxArgs = 0;
            break;
        }
        if (numArgs > maxArgs)
            maxArgs = numArgs;
    }

    QString qualifiedCppName = metaClass->typeEntry()->qualifiedCppName();
    QStringList templateTypes;
    foreach (TypeEntry* templateType, metaClass->templateArguments())
        templateTypes << templateType->qualifiedCppName();
    QString fixedTypeName = QString("%1<%2 >").arg(qualifiedCppName).arg(templateTypes.join(", "));

    // Empty constructor.
    if (maxArgs == 0)
        return QString("::%1()").arg(qualifiedCppName);

    QList<const AbstractMetaFunction*> candidates;

    // Constructors with C++ primitive types, enums or pointers only.
    // Start with the ones with fewer arguments.
    for (int i = 1; i <= maxArgs; ++i) {
        foreach (const AbstractMetaFunction* ctor, constructors) {
            if (ctor->isUserAdded() || ctor->isPrivate() || ctor->isCopyConstructor())
                continue;

            AbstractMetaArgumentList arguments = ctor->arguments();
            if (arguments.size() != i)
                continue;

            QStringList args;
            foreach (const AbstractMetaArgument* arg, arguments) {
                const TypeEntry* type = arg->type()->typeEntry();
                if (type == metaClass->typeEntry()) {
                    args.clear();
                    break;
                }

                if (!arg->originalDefaultValueExpression().isEmpty()) {
                    if (!arg->defaultValueExpression().isEmpty()
                        && arg->defaultValueExpression() != arg->originalDefaultValueExpression()) {
                        args << arg->defaultValueExpression();
                    }
                    break;
                }

                if (type->isCppPrimitive() || type->isEnum() || isPointer(arg->type())) {
                    QString argValue = minimalConstructor(arg->type());
                    if (argValue.isEmpty()) {
                        args.clear();
                        break;
                    }
                    args << argValue;
                } else {
                    args.clear();
                    break;
                }
            }

            if (!args.isEmpty())
                return QString("::%1(%2)").arg(qualifiedCppName).arg(args.join(", "));

            candidates << ctor;
        }
    }

    // Constructors with C++ primitive types, enums, pointers, value types,
    // and user defined primitive types.
    // Builds the minimal constructor recursively.
    foreach (const AbstractMetaFunction* ctor, candidates) {
        QStringList args;
        foreach (const AbstractMetaArgument* arg, ctor->arguments()) {
            if (arg->type()->typeEntry() == metaClass->typeEntry()) {
                args.clear();
                break;
            }
            QString argValue = minimalConstructor(arg->type());
            if (argValue.isEmpty()) {
                args.clear();
                break;
            }
            args << argValue;
        }
        if (!args.isEmpty()) {
            return QString("::%1(%2)").arg(qualifiedCppName)
                                      .arg(args.join(", "));
        }
    }

    return QString();
}

QString Generator::translateType(const AbstractMetaType *cType,
                                 const AbstractMetaClass *context,
                                 Options options) const
{
    QString s;
    static int constLen = strlen("const");

    if (context && cType &&
        context->typeEntry()->isGenericClass() &&
        cType->originalTemplateType()) {
        cType = cType->originalTemplateType();
    }

    if (!cType) {
        s = "void";
    } else if (cType->isArray()) {
        s = translateType(cType->arrayElementType(), context, options) + "[]";
    } else if (options & Generator::EnumAsInts && (cType->isEnum() || cType->isFlags())) {
        s = "int";
    } else {
        if (options & Generator::OriginalName) {
            s = cType->originalTypeDescription().trimmed();
            if ((options & Generator::ExcludeReference) && s.endsWith("&"))
                s = s.left(s.size()-1);

            // remove only the last const (avoid remove template const)
            if (options & Generator::ExcludeConst) {
                int index = s.lastIndexOf("const");

                if (index >= (s.size() - (constLen + 1))) // (VarType const)  or (VarType const[*|&])
                    s = s.remove(index, constLen);
            }
        } else if (options & Generator::ExcludeConst || options & Generator::ExcludeReference) {
            AbstractMetaType* copyType = cType->copy();

            if (options & Generator::ExcludeConst)
                copyType->setConstant(false);

            if (options & Generator::ExcludeReference)
                copyType->setReference(false);

            s = copyType->cppSignature();
            if (!copyType->typeEntry()->isVoid() && !copyType->typeEntry()->isCppPrimitive())
                s.prepend("::");
            delete copyType;
        } else {
            s = cType->cppSignature();
        }
    }

    return s;
}


QString Generator::subDirectoryForClass(const AbstractMetaClass* clazz) const
{
    return subDirectoryForPackage(clazz->package());
}

QString Generator::subDirectoryForPackage(QString packageName) const
{
    if (packageName.isEmpty())
        packageName = m_d->packageName;
    return QString(packageName).replace(".", QDir::separator());
}

template<typename T>
static QString getClassTargetFullName_(const T* t, bool includePackageName)
{
    QString name = t->name();
    const AbstractMetaClass* context = t->enclosingClass();
    while (context) {
        name.prepend('.');
        name.prepend(context->name());
        context = context->enclosingClass();
    }
    if (includePackageName) {
        name.prepend('.');
        name.prepend(t->package());
    }
    return name;
}

QString getClassTargetFullName(const AbstractMetaClass* metaClass, bool includePackageName)
{
    return getClassTargetFullName_(metaClass, includePackageName);
}

QString getClassTargetFullName(const AbstractMetaEnum* metaEnum, bool includePackageName)
{
    return getClassTargetFullName_(metaEnum, includePackageName);
}
