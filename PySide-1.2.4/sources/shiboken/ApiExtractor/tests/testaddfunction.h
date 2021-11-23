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

#ifndef TESTADDFUNCTION_H
#define TESTADDFUNCTION_H
#include <QObject>

class TestAddFunction : public QObject
{
    Q_OBJECT
private slots:
    void testParsingFuncNameAndConstness();
    void testAddFunction();
    void testAddFunctionConstructor();
    void testAddFunctionTagDefaultValues();
    void testAddFunctionCodeSnippets();
    void testAddFunctionWithoutParenteses();
    void testAddFunctionWithDefaultArgs();
    void testAddFunctionAtModuleLevel();
    void testAddFunctionWithVarargs();
    void testAddStaticFunction();
    void testAddGlobalFunction();
    void testAddFunctionWithApiVersion();
    void testModifyAddedFunction();
    void testAddFunctionOnTypedef();
    void testAddFunctionWithTemplateArg();
};

#endif
