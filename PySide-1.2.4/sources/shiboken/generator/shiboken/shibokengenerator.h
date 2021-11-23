/*
 * This file is part of the Shiboken Python Bindings Generator project.
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

#ifndef SHIBOKENGENERATOR_H
#define SHIBOKENGENERATOR_H

#define CONV_RULE_OUT_VAR_SUFFIX  "_out"
#define CPP_ARG                   "cppArg"
#define CPP_ARG0                  CPP_ARG"0"
#define CPP_ARG_REMOVED           "removed_" CPP_ARG
#define CPP_RETURN_VAR            "cppResult"
#define CPP_SELF_VAR              "cppSelf"
#define PYTHON_ARG                "pyArg"
#define PYTHON_ARGS               PYTHON_ARG "s"
#define PYTHON_OVERRIDE_VAR       "pyOverride"
#define PYTHON_RETURN_VAR         "pyResult"
#define PYTHON_SELF_VAR           "self"
#define THREAD_STATE_SAVER_VAR    "threadStateSaver"
#define BEGIN_ALLOW_THREADS       "PyThreadState* _save = PyEval_SaveThread(); // Py_BEGIN_ALLOW_THREADS"
#define END_ALLOW_THREADS         "PyEval_RestoreThread(_save); // Py_END_ALLOW_THREADS"
#define MIN_CTOR_ERROR_MSG        "Could not find a minimal constructor for type '%1'. "\
                                  "This will result in a compilation error."
#define PYTHON_TO_CPP_VAR         "pythonToCpp"

#define CHECKTYPE_REGEX           "%CHECKTYPE\\[([^\\[]*)\\]\\("
#define ISCONVERTIBLE_REGEX       "%ISCONVERTIBLE\\[([^\\[]*)\\]\\("
#define CONVERTTOPYTHON_REGEX     "%CONVERTTOPYTHON\\[([^\\[]*)\\]\\("
#define CONVERTTOCPP_REGEX        "(\\*?%?[a-zA-Z_][\\w\\.]*(?:\\[[^\\[^<^>]+\\])*)"\
                                  "(?:\\s+)=(?:\\s+)%CONVERTTOCPP\\[([^\\[]*)\\]\\("

#include <generator.h>
#include <QtCore/QTextStream>

#include "overloaddata.h"

class DocParser;

/**
 * Abstract generator that contains common methods used in CppGenerator and HeaderGenerator.
 */
class ShibokenGenerator : public Generator
{
public:
    ShibokenGenerator();
    virtual ~ShibokenGenerator();

    QString translateTypeForWrapperMethod(const AbstractMetaType* cType,
                                          const AbstractMetaClass* context, Options opt = NoOption) const;

    /**
    *   Returns a map with all functions grouped, the function name is used as key.
    *   Example ofg return value: { "foo" -> ["foo(int)", "foo(int, long)], "bar" -> "bar(double)"}
    * \param scope Where to search for functions, null means all global functions.
    */
    QMap<QString, AbstractMetaFunctionList> getFunctionGroups(const AbstractMetaClass* scope = 0);
    /**
    *   Returns all overloads for a function named \p functionName.
    *   \param scope scope used to search for overloads.
    *   \param functionName the function name.
    */
    AbstractMetaFunctionList getFunctionOverloads(const AbstractMetaClass* scope, const QString& functionName);
    /**
    *   Returns the minimun and maximun number of arguments which this function and all overloads
    *   can accept. Arguments removed by typesystem are considered as well.
    */
    QPair<int, int> getMinMaxArguments(const AbstractMetaFunction* metaFunction);
    /**
     *   Write a function argument in the C++ in the text stream \p s.
     *   This function just call \code s << argumentString(); \endcode
     *   \param s text stream used to write the output.
     *   \param func the current metafunction.
     *   \param argument metaargument information to be parsed.
     *   \param options some extra options.
     */
    void writeArgument(QTextStream &s,
                       const AbstractMetaFunction* func,
                       const AbstractMetaArgument* argument,
                       Options options = NoOption) const;
    /**
     *   Create a QString in the C++ format to an function argument.
     *   \param func the current metafunction.
     *   \param argument metaargument information to be parsed.
     *   \param options some extra options.
     */
    QString argumentString(const AbstractMetaFunction* func,
                           const AbstractMetaArgument* argument,
                           Options options = NoOption) const;

    void writeArgumentNames(QTextStream &s,
                            const AbstractMetaFunction* func,
                            Options options = NoOption) const;

    /**
     *   Function used to write the fucntion arguments on the class buffer.
     *   \param s the class output buffer
     *   \param func the pointer to metafunction information
     *   \param count the number of function arguments
     *   \param options some extra options used during the parser
     */
    void writeFunctionArguments(QTextStream &s,
                                const AbstractMetaFunction* func,
                                Options options = NoOption) const;
    QString functionReturnType(const AbstractMetaFunction* func, Options options = NoOption) const;

    /// Utility function for writeCodeSnips.
    typedef QPair<const AbstractMetaArgument*, QString> ArgumentVarReplacementPair;
    typedef QList<ArgumentVarReplacementPair> ArgumentVarReplacementList;
    ArgumentVarReplacementList getArgumentReplacement(const AbstractMetaFunction* func,
                                                      bool usePyArgs, TypeSystem::Language language,
                                                      const AbstractMetaArgument* lastArg);

    /// Write user's custom code snippets at class or module level.
    void writeCodeSnips(QTextStream& s,
                        const CodeSnipList& codeSnips,
                        CodeSnip::Position position,
                        TypeSystem::Language language,
                        const AbstractMetaClass* context = 0);
    /// Write user's custom code snippets at function level.
    void writeCodeSnips(QTextStream& s,
                        const CodeSnipList& codeSnips,
                        CodeSnip::Position position,
                        TypeSystem::Language language,
                        const AbstractMetaFunction* func,
                        const AbstractMetaArgument* lastArg = 0);

    /// Returns a string with the user's custom code snippets that comply with \p position and \p language.
    QString getCodeSnippets(const CodeSnipList& codeSnips, CodeSnip::Position position, TypeSystem::Language language);

    /// Replaces variables for the user's custom code at global or class level.
    void processCodeSnip(QString& code, const AbstractMetaClass* context = 0);

    /// Replaces the %CONVERTTOPYTHON type system variable.
    inline void replaceConvertToPythonTypeSystemVariable(QString& code)
    {
        replaceConverterTypeSystemVariable(TypeSystemToPythonFunction, code);
    }
    /// Replaces the %CONVERTTOCPP type system variable.
    inline void replaceConvertToCppTypeSystemVariable(QString& code)
    {
        replaceConverterTypeSystemVariable(TypeSystemToCppFunction, code);
    }
    /// Replaces the %ISCONVERTIBLE type system variable.
    inline void replaceIsConvertibleToCppTypeSystemVariable(QString& code)
    {
        replaceConverterTypeSystemVariable(TypeSystemIsConvertibleFunction, code);
    }
    /// Replaces the %CHECKTYPE type system variable.
    inline void replaceTypeCheckTypeSystemVariable(QString& code)
    {
        replaceConverterTypeSystemVariable(TypeSystemCheckFunction, code);
    }

    /**
     *   Verifies if any of the function's code injections of the "target"
     *   type needs the type system variable "%CPPSELF".
     *   \param func the function to check
     *   \return true if the function's target code snippets use "%CPPSELF"
     */
    bool injectedCodeUsesCppSelf(const AbstractMetaFunction* func);

    /**
     *   Verifies if any of the function's code injections of the "native"
     *   type needs the type system variable "%PYSELF".
     *   \param func the function to check
     *   \return true if the function's native code snippets use "%PYSELF"
     */
    bool injectedCodeUsesPySelf(const AbstractMetaFunction* func);

    /**
     *   Verifies if any of the function's code injections makes a call
     *   to the C++ method. This is used by the generator to avoid writing calls
     *   to C++ when the user custom code already does this.
     *   \param func the function to check
     *   \return true if the function's code snippets call the wrapped C++ function
     */
    bool injectedCodeCallsCppFunction(const AbstractMetaFunction* func);

    /**
     *   Verifies if any of the function's code injections of the "native" class makes a
     *   call to the C++ method. This is used by the generator to avoid writing calls to
     *   Python overrides of C++ virtual methods when the user custom code already does this.
     *   \param func the function to check
     *   \return true if the function's code snippets call the Python override for a C++ virtual method
     */
    bool injectedCodeCallsPythonOverride(const AbstractMetaFunction* func);

    /**
     *   Verifies if any of the function's code injections attributes values to
     *   the return variable (%0 or %PYARG_0).
     *   \param func        the function to check
     *   \param language    the kind of code snip
     *   \return true if the function's code attributes values to "%0" or "%PYARG_0"
     */
    bool injectedCodeHasReturnValueAttribution(const AbstractMetaFunction* func, TypeSystem::Language language = TypeSystem::TargetLangCode);

    /**
     *   Verifies if any of the function's code injections uses the type system variable
     *   for function arguments of a given index.
     */
    bool injectedCodeUsesArgument(const AbstractMetaFunction* func, int argumentIndex);

    /**
     *   Function which parse the metafunction information
     *   \param func the function witch will be parserd
     *   \param option some extra options
     *   \param arg_count the number of function arguments
     */
    QString functionSignature(const AbstractMetaFunction* func,
                              QString prepend = "",
                              QString append = "",
                              Options options = NoOption,
                              int arg_count = -1) const;

    /// Returns true if there are cases of multiple inheritance in any of its ancestors.
    bool hasMultipleInheritanceInAncestry(const AbstractMetaClass* metaClass);

    /// Returns true if the class needs to have a getattro function.
    bool classNeedsGetattroFunction(const AbstractMetaClass* metaClass);

    /// Returns a list of methods of the given class where each one is part of a different overload with both static and non-static method.
    AbstractMetaFunctionList getMethodsWithBothStaticAndNonStaticMethods(const AbstractMetaClass* metaClass);

    /// Returns a list of parent classes for a given class.
    AbstractMetaClassList getBaseClasses(const AbstractMetaClass* metaClass) const;

    /// Returns a list of all ancestor classes for the given class.
    AbstractMetaClassList getAllAncestors(const AbstractMetaClass* metaClass) const;

    const AbstractMetaClass* getMultipleInheritingClass(const AbstractMetaClass* metaClass);

    void writeToPythonConversion(QTextStream& s, const AbstractMetaType* type,
                                 const AbstractMetaClass* context, const QString& argumentName);
    void writeToCppConversion(QTextStream& s, const AbstractMetaType* type, const AbstractMetaClass* context, const QString& inArgName, const QString& outArgName);
    void writeToCppConversion(QTextStream& s, const AbstractMetaClass* metaClass, const QString& inArgName, const QString& outArgName);

    /// Returns true if the argument is a pointer that rejects NULL values.
    bool shouldRejectNullPointerArgument(const AbstractMetaFunction* func, int argIndex);

    /// Verifies if the class should have a C++ wrapper generated for it, instead of only a Python wrapper.
    bool shouldGenerateCppWrapper(const AbstractMetaClass* metaClass) const;

    /// Adds enums eligible for generation from classes/namespaces marked not to be generated.
    static void lookForEnumsInClassesNotToBeGenerated(AbstractMetaEnumList& enumList, const AbstractMetaClass* metaClass);
    /// Returns the enclosing class for an enum, or NULL if it should be global.
    const AbstractMetaClass* getProperEnclosingClassForEnum(const AbstractMetaEnum* metaEnum);

    QString wrapperName(const AbstractMetaClass* metaClass) const;

    static QString fullPythonFunctionName(const AbstractMetaFunction* func);
    static QString protectedEnumSurrogateName(const AbstractMetaEnum* metaEnum);
    static QString protectedFieldGetterName(const AbstractMetaField* field);
    static QString protectedFieldSetterName(const AbstractMetaField* field);

    static QString pythonPrimitiveTypeName(const QString& cppTypeName);
    static QString pythonPrimitiveTypeName(const PrimitiveTypeEntry* type);

    static QString pythonOperatorFunctionName(QString cppOpFuncName);
    static QString pythonOperatorFunctionName(const AbstractMetaFunction* func);
    static QString pythonRichCompareOperatorId(QString cppOpFuncName);
    static QString pythonRichCompareOperatorId(const AbstractMetaFunction* func);

    static QString cpythonOperatorFunctionName(const AbstractMetaFunction* func);

    static QString fixedCppTypeName(const CustomConversion::TargetToNativeConversion* toNative);
    static QString fixedCppTypeName(const AbstractMetaType* type);
    static QString fixedCppTypeName(const TypeEntry* type, QString typeName = QString());

    static bool isNumber(QString cpythonApiName);
    static bool isNumber(const TypeEntry* type);
    static bool isNumber(const AbstractMetaType* type);
    static bool isPyInt(const TypeEntry* type);
    static bool isPyInt(const AbstractMetaType* type);
    static bool isPairContainer(const AbstractMetaType* type);

    /**
     *  Returns true if the type passed has a Python wrapper for it.
     *  Although namespace has a Python wrapper, it's not considered a type.
     */
    static bool isWrapperType(const TypeEntry* type);
    static bool isWrapperType(const ComplexTypeEntry* type);
    static bool isWrapperType(const AbstractMetaType* metaType);

    /**
     *  Checks if the type is an Object/QObject or pointer to Value Type.
     *  In other words, tells if the type is "T*" and T has a Python wrapper.
     */
    static bool isPointerToWrapperType(const AbstractMetaType* type);

    /**
     *  Returns true if \p type is an Object Type used as a value.
     */
    static bool isObjectTypeUsedAsValueType(const AbstractMetaType* type);

    static bool isValueTypeWithCopyConstructorOnly(const AbstractMetaClass* metaClass);
    bool isValueTypeWithCopyConstructorOnly(const TypeEntry* type) const;
    bool isValueTypeWithCopyConstructorOnly(const AbstractMetaType* type) const;

    /// Returns true if the type is a primitive but not a C++ primitive.
    static bool isUserPrimitive(const TypeEntry* type);
    static bool isUserPrimitive(const AbstractMetaType* type);

    /// Returns true if the type is a C++ primitive, a void*, a const char*, or a std::string.
    static bool isCppPrimitive(const TypeEntry* type);
    static bool isCppPrimitive(const AbstractMetaType* type);

    /// Returns true if the type is a C++ integral primitive, i.e. bool, char, int, long, and their unsigned counterparts.
    static bool isCppIntegralPrimitive(const TypeEntry* type);
    static bool isCppIntegralPrimitive(const AbstractMetaType* type);

    /// Checks if an argument type should be dereferenced by the Python method wrapper before calling the C++ method.
    static bool shouldDereferenceArgumentPointer(const AbstractMetaArgument* arg);
    /// Checks if a meta type should be dereferenced by the Python method wrapper passing it to C++.
    static bool shouldDereferenceAbstractMetaTypePointer(const AbstractMetaType* metaType);

    static bool visibilityModifiedToPrivate(const AbstractMetaFunction* func);

    QString converterObject(const AbstractMetaType* type);
    QString converterObject(const TypeEntry* type);

    QString cpythonBaseName(const AbstractMetaClass* metaClass);
    QString cpythonBaseName(const TypeEntry* type);
    QString cpythonBaseName(const AbstractMetaType* type);
    QString cpythonTypeName(const AbstractMetaClass* metaClass);
    QString cpythonTypeName(const TypeEntry* type);
    QString cpythonTypeNameExt(const TypeEntry* type);
    QString cpythonTypeNameExt(const AbstractMetaType* type);
    QString cpythonCheckFunction(const TypeEntry* type, bool genericNumberType = false);
    QString cpythonCheckFunction(const AbstractMetaType* metaType, bool genericNumberType = false);
    /**
     *  Receives the argument \p type and tries to find the appropriate AbstractMetaType for it
     *  or a custom type check.
     *  \param type     A string representing the type to be discovered.
     *  \param metaType A pointer to an AbstractMetaType pointer, to where write a new meta type object
     *                  if one is produced from the \p type string. This object must be deallocated by
     *                  the caller. It will set the target variable to NULL, is \p type is a Python type.
     *  \return A custom check if \p type is a custom type, or an empty string if \p metaType
     *          receives an existing type object.
     */
    QString guessCPythonCheckFunction(const QString& type, AbstractMetaType** metaType);
    QString cpythonIsConvertibleFunction(const TypeEntry* type, bool genericNumberType = false, bool checkExact = false);
    QString cpythonIsConvertibleFunction(const AbstractMetaType* metaType, bool genericNumberType = false);
    inline QString cpythonIsConvertibleFunction(const AbstractMetaArgument* metaArg, bool genericNumberType = false)
    {
        return cpythonIsConvertibleFunction(metaArg->type(), genericNumberType);
    }
    QString guessCPythonIsConvertible(const QString& type);

    QString cpythonToCppConversionFunction(const AbstractMetaClass* metaClass);
    QString cpythonToCppConversionFunction(const AbstractMetaType* type, const AbstractMetaClass* context = 0);
    QString cpythonToPythonConversionFunction(const AbstractMetaType* type, const AbstractMetaClass* context = 0);
    QString cpythonToPythonConversionFunction(const AbstractMetaClass* metaClass);
    QString cpythonToPythonConversionFunction(const TypeEntry* type);

    QString cpythonFunctionName(const AbstractMetaFunction* func);
    QString cpythonMethodDefinitionName(const AbstractMetaFunction* func);
    QString cpythonGettersSettersDefinitionName(const AbstractMetaClass* metaClass);
    QString cpythonGetattroFunctionName(const AbstractMetaClass* metaClass);
    QString cpythonSetattroFunctionName(const AbstractMetaClass* metaClass);
    QString cpythonGetterFunctionName(const AbstractMetaField* metaField);
    QString cpythonSetterFunctionName(const AbstractMetaField* metaField);
    QString cpythonWrapperCPtr(const AbstractMetaClass* metaClass, QString argName = PYTHON_SELF_VAR);
    QString cpythonWrapperCPtr(const AbstractMetaType* metaType, QString argName);
    QString cpythonWrapperCPtr(const TypeEntry* type, QString argName);

    /// Guesses the scope to where belongs an argument's default value.
    QString guessScopeForDefaultValue(const AbstractMetaFunction* func, const AbstractMetaArgument* arg);

    QString cpythonEnumName(const EnumTypeEntry* enumEntry);
    inline QString cpythonEnumName(const AbstractMetaEnum* metaEnum)
    {
        return cpythonEnumName(metaEnum->typeEntry());
    }

    QString cpythonFlagsName(const FlagsTypeEntry* flagsEntry);
    inline QString cpythonFlagsName(const AbstractMetaEnum* metaEnum)
    {
        FlagsTypeEntry* flags = metaEnum->typeEntry()->flags();
        if (!flags)
            return QString();
        return cpythonFlagsName(flags);
    }
    /// Returns the special cast function name, the function used to proper cast class with multiple inheritance.
    QString cpythonSpecialCastFunctionName(const AbstractMetaClass* metaClass);

    QString getFunctionReturnType(const AbstractMetaFunction* func, Options options = NoOption) const;
    QString getFormatUnitString(const AbstractMetaFunction* func, bool incRef = false) const;

    /// Returns the file name for the module global header. If no module name is provided the current will be used.
    QString getModuleHeaderFileName(const QString& moduleName = QString()) const;

    QString extendedIsConvertibleFunctionName(const TypeEntry* targetType) const;
    QString extendedToCppFunctionName(const TypeEntry* targetType) const;

    QMap< QString, QString > options() const;

    /// Returns true if the user enabled the so called "parent constructor heuristic".
    bool useCtorHeuristic() const;
    /// Returns true if the user enabled the so called "return value heuristic".
    bool useReturnValueHeuristic() const;
    /// Returns true if the user enabled PySide extensions.
    bool usePySideExtensions() const;
    /// Returns true if the generator should use the result of isNull()const to compute boolean casts.
    bool useIsNullAsNbNonZero() const;
    /// Returns true if the generated code should use the "#define protected public" hack.
    bool avoidProtectedHack() const;
    QString cppApiVariableName(const QString& moduleName = QString()) const;
    QString convertersVariableName(const QString& moduleName = QString()) const;
    /**
     *  Returns the type index variable name for a given class. If \p alternativeTemplateName is true
     *  and the class is a typedef for a template class instantiation, it will return an alternative name
     *  made of the template class and the instantiation values, or an empty string if the class isn't
     *  derived from a template class at all.
     */
    QString getTypeIndexVariableName(const AbstractMetaClass* metaClass, bool alternativeTemplateName = false);
    QString getTypeIndexVariableName(const TypeEntry* type);
    QString getTypeIndexVariableName(const AbstractMetaType* type);

    /// Returns true if the user don't want verbose error messages on the generated bindings.
    bool verboseErrorMessagesDisabled() const;

    /**
     *   Builds an AbstractMetaType object from a QString.
     *   Returns NULL if no type could be built from the string.
     *   \param typeSignature The string describing the type to be built.
     *   \return A new AbstractMetaType object that must be deleted by the caller, or a NULL pointer in case of failure.
     */
    AbstractMetaType* buildAbstractMetaTypeFromString(QString typeSignature);

    /// Creates an AbstractMetaType object from a TypeEntry.
    AbstractMetaType* buildAbstractMetaTypeFromTypeEntry(const TypeEntry* typeEntry);
    /// Creates an AbstractMetaType object from an AbstractMetaClass.
    AbstractMetaType* buildAbstractMetaTypeFromAbstractMetaClass(const AbstractMetaClass* metaClass);

    void writeMinimalConstructorExpression(QTextStream& s, const AbstractMetaType* type, const QString& defaultCtor = QString());
    void writeMinimalConstructorExpression(QTextStream& s, const TypeEntry* type, const QString& defaultCtor = QString());

    /**
     *  Helper function to return the flags to be used by a meta type when
     * it needs to write some converter code.
     */
    static Options getConverterOptions(const AbstractMetaType* metaType);

    /**
     * Helper function to find for argument default value
     */
    static QString getDefaultValue(const AbstractMetaFunction* func, const AbstractMetaArgument* arg);
protected:
    bool doSetup(const QMap<QString, QString>& args);
    void collectContainerTypesFromConverterMacros(const QString& code, bool toPythonMacro);
    // verify whether the class is copyable
    bool isCopyable(const AbstractMetaClass* metaClass);

    bool m_native_jump_table;
    static QHash<QString, QString> m_pythonPrimitiveTypeName;
    static QHash<QString, QString> m_pythonOperators;
    static QHash<QString, QString> m_formatUnits;
    static QHash<QString, QString> m_tpFuncs;
    static QStringList m_knownPythonTypes;

    void clearTpFuncs();

    const char* name() const { return "Shiboken"; }

    /// Initializes correspondences between primitive and Python types.
    static void initPrimitiveTypesCorrespondences();
    /// Initializes a list of Python known type names.
    static void initKnownPythonTypes();

    void writeFunctionCall(QTextStream& s,
                           const AbstractMetaFunction* metaFunc,
                           Options options = NoOption) const;

    void writeUnusedVariableCast(QTextStream& s, const QString& variableName);

    AbstractMetaFunctionList filterFunctions(const AbstractMetaClass* metaClass);

    // All data about extended converters: the type entries of the target type, and a
    // list of AbstractMetaClasses accepted as argument for the conversion.
    typedef QHash<const TypeEntry*, QList<const AbstractMetaClass*> > ExtendedConverterData;
    /// Returns all extended conversions for the current module.
    ExtendedConverterData getExtendedConverters() const;

    /// Returns a list of converters for the non wrapper types of the current module.
    QList<const CustomConversion*> getPrimitiveCustomConversions();

    /// Returns true if the Python wrapper for the received OverloadData must accept a list of arguments.
    static bool pythonFunctionWrapperUsesListOfArguments(const OverloadData& overloadData);

    Indentor INDENT;

    enum TypeSystemConverterVariable {
        TypeSystemCheckFunction = 0,
        TypeSystemIsConvertibleFunction,
        TypeSystemToCppFunction,
        TypeSystemToPythonFunction,
        TypeSystemConverterVariables
    };
    void replaceConverterTypeSystemVariable(TypeSystemConverterVariable converterVariable, QString& code);

private:
    bool m_useCtorHeuristic;
    bool m_userReturnValueHeuristic;
    bool m_usePySideExtensions;
    bool m_verboseErrorMessagesDisabled;
    bool m_useIsNullAsNbNonZero;
    bool m_avoidProtectedHack;

    typedef QHash<QString, AbstractMetaType*> AbstractMetaTypeCache;
    AbstractMetaTypeCache m_metaTypeFromStringCache;

    /// Type system converter variable replacement names and regular expressions.
    QString m_typeSystemConvName[TypeSystemConverterVariables];
    QRegExp m_typeSystemConvRegEx[TypeSystemConverterVariables];
};

#endif // SHIBOKENGENERATOR_H
