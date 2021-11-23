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


#ifndef NAME_COMPILER_H
#define NAME_COMPILER_H

#include "default_visitor.h"
#include <QtCore/QStringList>

class TokenStream;
class Binder;

class NameCompiler: protected DefaultVisitor
{
public:
    NameCompiler(Binder *binder);

    void run(NameAST *node) {
        internal_run(node);
    }
    void run(UnqualifiedNameAST *node) {
        internal_run(node);
    }

    QString name() const {
        return _M_name.join("::");
    }
    QStringList qualifiedName() const {
        return _M_name;
    }

protected:
    virtual void visitUnqualifiedName(UnqualifiedNameAST *node);
    virtual void visitTemplateArgument(TemplateArgumentAST *node);

    QString internal_run(AST *node);
    QString decode_operator(std::size_t index) const;

private:
    Binder *_M_binder;
    TokenStream *_M_token_stream;
    QStringList _M_name;
};

#endif // NAME_COMPILER_H

// kate: space-indent on; indent-width 2; replace-tabs on;
