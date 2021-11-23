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

#ifndef OVERLOADDATA_H
#define OVERLOADDATA_H

#include <abstractmetalang.h>
#include <QtCore/QList>
#include <QtCore/QBitArray>

class ShibokenGenerator;

class OverloadData;
typedef QList<OverloadData*> OverloadDataList;

class OverloadData
{
public:
    OverloadData(const AbstractMetaFunctionList& overloads, const ShibokenGenerator* generator);
    ~OverloadData();

    int minArgs() const { return m_headOverloadData->m_minArgs; }
    int maxArgs() const { return m_headOverloadData->m_maxArgs; }
    int argPos() const { return m_argPos; }

    const AbstractMetaType* argType() const { return m_argType; }

    /// Returns a string list containing all the possible return types (including void) for the current OverloadData.
    QStringList returnTypes() const;

    /// Returns true if any of the overloads for the current OverloadData has a return type different from void.
    bool hasNonVoidReturnType() const;

    /// Returns true if any of the overloads for the current OverloadData has a varargs argument.
    bool hasVarargs() const;

    /// Returns true if any of the overloads for the current OverloadData allows threads when called.
    bool hasAllowThread() const;

    /// Returns true if any of the overloads for the current OverloadData is static.
    bool hasStaticFunction() const;

    /// Returns true if any of the overloads passed as argument is static.
    static bool hasStaticFunction(const AbstractMetaFunctionList& overloads);

    /// Returns true if any of the overloads for the current OverloadData is not static.
    bool hasInstanceFunction() const;

    /// Returns true if any of the overloads passed as argument is not static.
    static bool hasInstanceFunction(const AbstractMetaFunctionList& overloads);

    /// Returns true if among the overloads for the current OverloadData there are static and non-static methods altogether.
    bool hasStaticAndInstanceFunctions() const;

    /// Returns true if among the overloads passed as argument there are static and non-static methods altogether.
    static bool hasStaticAndInstanceFunctions(const AbstractMetaFunctionList& overloads);

    const AbstractMetaFunction* referenceFunction() const;
    const AbstractMetaArgument* argument(const AbstractMetaFunction* func) const;
    OverloadDataList overloadDataOnPosition(int argPos) const;

    bool isHeadOverloadData() const { return this == m_headOverloadData; }

    /// Returns the root OverloadData object that represents all the overloads.
    OverloadData* headOverloadData() const { return m_headOverloadData; }

    /// Returns the function that has a default value at the current OverloadData argument position, otherwise returns null.
    const AbstractMetaFunction* getFunctionWithDefaultValue() const;

    bool nextArgumentHasDefaultValue() const;
    /// Returns the nearest occurrence, including this instance, of an argument with a default value.
    OverloadData* findNextArgWithDefault();
    bool isFinalOccurrence(const AbstractMetaFunction* func) const;

    /// Returns the list of overloads removing repeated constant functions (ex.: "foo()" and "foo()const", the second is removed).
    QList<const AbstractMetaFunction*> overloadsWithoutRepetition() const;
    const QList<const AbstractMetaFunction*>& overloads() const { return m_overloads; }
    OverloadDataList nextOverloadData() const { return m_nextOverloadData; }
    OverloadData* previousOverloadData() const { return m_previousOverloadData; }

    QList<int> invalidArgumentLengths() const;

    static int numberOfRemovedArguments(const AbstractMetaFunction* func, int finalArgPos = -1);
    static QPair<int, int> getMinMaxArguments(const AbstractMetaFunctionList& overloads);
    /// Returns true if all overloads have no more than one argument.
    static bool isSingleArgument(const AbstractMetaFunctionList& overloads);

    void dumpGraph(QString filename) const;
    QString dumpGraph() const;

    bool hasArgumentTypeReplace() const;
    QString argumentTypeReplaced() const;

    bool hasArgumentWithDefaultValue() const;
    static bool hasArgumentWithDefaultValue(const AbstractMetaFunctionList& overloads);
    static bool hasArgumentWithDefaultValue(const AbstractMetaFunction* func);

    /// Returns a list of function arguments which have default values and were not removed.
    static AbstractMetaArgumentList getArgumentsWithDefaultValues(const AbstractMetaFunction* func);

private:
    OverloadData(OverloadData* headOverloadData, const AbstractMetaFunction* func,
                 const AbstractMetaType* argType, int argPos);

    void addOverload(const AbstractMetaFunction* func);
    OverloadData* addOverloadData(const AbstractMetaFunction* func, const AbstractMetaArgument* arg);

    void sortNextOverloads();

    int functionNumber(const AbstractMetaFunction* func) const;
    OverloadDataList overloadDataOnPosition(OverloadData* overloadData, int argPos) const;

    int m_minArgs;
    int m_maxArgs;
    int m_argPos;
    const AbstractMetaType* m_argType;
    QString m_argTypeReplaced;
    QList<const AbstractMetaFunction*> m_overloads;

    OverloadData* m_headOverloadData;
    OverloadDataList m_nextOverloadData;
    OverloadData* m_previousOverloadData;
    const ShibokenGenerator* m_generator;
};


#endif // OVERLOADDATA_H
