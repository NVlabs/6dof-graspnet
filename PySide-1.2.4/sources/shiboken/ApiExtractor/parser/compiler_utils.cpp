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


#include "compiler_utils.h"
#include "type_compiler.h"
#include "name_compiler.h"
#include "declarator_compiler.h"
#include "ast.h"
#include "binder.h"

TypeInfo CompilerUtils::typeDescription(TypeSpecifierAST *type_specifier, DeclaratorAST *declarator, Binder *binder)
{
    TypeCompiler type_cc(binder);
    DeclaratorCompiler decl_cc(binder);

    type_cc.run(type_specifier);
    decl_cc.run(declarator);

    TypeInfo typeInfo;
    typeInfo.setQualifiedName(type_cc.qualifiedName());
    typeInfo.setConstant(type_cc.isConstant());
    typeInfo.setVolatile(type_cc.isVolatile());
    typeInfo.setReference(decl_cc.isReference());
    typeInfo.setIndirections(decl_cc.indirection());
    typeInfo.setArrayElements(decl_cc.arrayElements());

    return typeInfo;
}
