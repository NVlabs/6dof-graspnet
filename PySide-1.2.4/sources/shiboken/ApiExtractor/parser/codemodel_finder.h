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


#ifndef CODEMODEL_FINDER_H
#define CODEMODEL_FINDER_H

#include <default_visitor.h>
#include <codemodel_fwd.h>
#include <name_compiler.h>

class TokenStream;
class Binder;

class CodeModelFinder: protected DefaultVisitor
{
    enum ResolvePolicy {
        ResolveScope,
        ResolveItem
    };

public:
    CodeModelFinder(CodeModel *model, Binder *binder);
    virtual ~CodeModelFinder();

    ScopeModelItem resolveScope(NameAST *name, ScopeModelItem scope);

    inline CodeModel *model() const
    {
        return _M_model;
    }

protected:
    virtual void visitName(NameAST *node);
    virtual void visitUnqualifiedName(UnqualifiedNameAST *node);

    ScopeModelItem changeCurrentScope(ScopeModelItem scope);

private:
    CodeModel *_M_model;
    Binder *_M_binder;
    TokenStream *_M_token_stream;
    NameCompiler name_cc;

    ScopeModelItem _M_current_scope;
    ResolvePolicy _M_resolve_policy;
};

#endif // CODEMODEL_FINDER_H
