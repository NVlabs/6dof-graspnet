/*
 * This file is part of the API Extractor project.
 *
 * Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
 * Copyright (C) 2002-2005 Roberto Raggi <roberto@kdevelop.org>
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


#ifndef CODEMODEL_FWD_H
#define CODEMODEL_FWD_H

#include "codemodel_pointer.h"
#include <QtCore/QList>

// forward declarations
class CodeModel;
class _ArgumentModelItem;
class _ClassModelItem;
class _CodeModelItem;
class _EnumModelItem;
class _EnumeratorModelItem;
class _FileModelItem;
class _FunctionDefinitionModelItem;
class _FunctionModelItem;
class _NamespaceModelItem;
class _ScopeModelItem;
class _TemplateParameterModelItem;
class _TypeAliasModelItem;
class _VariableModelItem;
class _MemberModelItem;
class TypeInfo;

typedef CodeModelPointer<_ArgumentModelItem> ArgumentModelItem;
typedef CodeModelPointer<_ClassModelItem> ClassModelItem;
typedef CodeModelPointer<_CodeModelItem> CodeModelItem;
typedef CodeModelPointer<_EnumModelItem> EnumModelItem;
typedef CodeModelPointer<_EnumeratorModelItem> EnumeratorModelItem;
typedef CodeModelPointer<_FileModelItem> FileModelItem;
typedef CodeModelPointer<_FunctionDefinitionModelItem> FunctionDefinitionModelItem;
typedef CodeModelPointer<_FunctionModelItem> FunctionModelItem;
typedef CodeModelPointer<_NamespaceModelItem> NamespaceModelItem;
typedef CodeModelPointer<_ScopeModelItem> ScopeModelItem;
typedef CodeModelPointer<_TemplateParameterModelItem> TemplateParameterModelItem;
typedef CodeModelPointer<_TypeAliasModelItem> TypeAliasModelItem;
typedef CodeModelPointer<_VariableModelItem> VariableModelItem;
typedef CodeModelPointer<_MemberModelItem> MemberModelItem;

typedef QList<ArgumentModelItem> ArgumentList;
typedef QList<ClassModelItem> ClassList;
typedef QList<CodeModelItem> ItemList;
typedef QList<EnumModelItem> EnumList;
typedef QList<EnumeratorModelItem> EnumeratorList;
typedef QList<FileModelItem> FileList;
typedef QList<FunctionDefinitionModelItem> FunctionDefinitionList;
typedef QList<FunctionModelItem> FunctionList;
typedef QList<NamespaceModelItem> NamespaceList;
typedef QList<ScopeModelItem> ScopeList;
typedef QList<TemplateParameterModelItem> TemplateParameterList;
typedef QList<TypeAliasModelItem> TypeAliasList;
typedef QList<VariableModelItem> VariableList;
typedef QList<MemberModelItem> MemberList;

#endif // CODEMODEL_FWD_H
