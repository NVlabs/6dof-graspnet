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

/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */


#include "type_compiler.h"
#include "name_compiler.h"
#include "lexer.h"
#include "symbol.h"
#include "tokens.h"
#include "binder.h"

#include <QtCore/QString>

TypeCompiler::TypeCompiler(Binder *binder)
        : _M_binder(binder), _M_token_stream(binder->tokenStream())
{
}

void TypeCompiler::run(TypeSpecifierAST *node)
{
    _M_type.clear();
    _M_cv.clear();

    visit(node);

    if (node && node->cv) {
        const ListNode<std::size_t> *it = node->cv->toFront();
        const ListNode<std::size_t> *end = it;
        do {
            int kind = _M_token_stream->kind(it->element);
            if (!_M_cv.contains(kind))
                _M_cv.append(kind);

            it = it->next;
        } while (it != end);
    }
}

void TypeCompiler::visitClassSpecifier(ClassSpecifierAST *node)
{
    visit(node->name);
}

void TypeCompiler::visitEnumSpecifier(EnumSpecifierAST *node)
{
    visit(node->name);
}

void TypeCompiler::visitElaboratedTypeSpecifier(ElaboratedTypeSpecifierAST *node)
{
    visit(node->name);
}

void TypeCompiler::visitSimpleTypeSpecifier(SimpleTypeSpecifierAST *node)
{
    if (const ListNode<std::size_t> *it = node->integrals) {
        it = it->toFront();
        const ListNode<std::size_t> *end = it;
        QString current_item;
        do {
            std::size_t token = it->element;
            current_item += token_name(_M_token_stream->kind(token));
            current_item += " ";
            it = it->next;
        } while (it != end);
        _M_type += current_item.trimmed();
    } else if (node->type_of) {
        // ### implement me
        _M_type += QLatin1String("typeof<...>");
    }

    visit(node->name);
}

void TypeCompiler::visitName(NameAST *node)
{
    NameCompiler name_cc(_M_binder);
    name_cc.run(node);
    _M_type = name_cc.qualifiedName();
}

QStringList TypeCompiler::cvString() const
{
    QStringList lst;

    foreach (int q, cv()) {
        if (q == Token_const)
            lst.append(QLatin1String("const"));
        else if (q == Token_volatile)
            lst.append(QLatin1String("volatile"));
    }

    return lst;
}

bool TypeCompiler::isConstant() const
{
    return _M_cv.contains(Token_const);
}

bool TypeCompiler::isVolatile() const
{
    return _M_cv.contains(Token_volatile);
}

// kate: space-indent on; indent-width 2; replace-tabs on;
