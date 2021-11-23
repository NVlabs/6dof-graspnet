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


#include "class_compiler.h"
#include "lexer.h"
#include "binder.h"

ClassCompiler::ClassCompiler(Binder *binder)
        : _M_binder(binder),
        _M_token_stream(binder->tokenStream()),
        name_cc(_M_binder),
        type_cc(_M_binder)
{
}

ClassCompiler::~ClassCompiler()
{
}

void ClassCompiler::run(ClassSpecifierAST *node)
{
    name_cc.run(node->name);
    _M_name = name_cc.name();
    _M_base_classes.clear();

    visit(node);
}

void ClassCompiler::visitClassSpecifier(ClassSpecifierAST *node)
{
    visit(node->base_clause);
}

void ClassCompiler::visitBaseSpecifier(BaseSpecifierAST *node)
{
    name_cc.run(node->name);
    QString name = name_cc.name();

    if (!name.isEmpty())
        _M_base_classes.append(name);
}


// kate: space-indent on; indent-width 2; replace-tabs on;
