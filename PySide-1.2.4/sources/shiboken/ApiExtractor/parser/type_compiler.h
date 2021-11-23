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


#ifndef TYPE_COMPILER_H
#define TYPE_COMPILER_H

#include "default_visitor.h"

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QList>

class TokenStream;
class Binder;

class TypeCompiler: protected DefaultVisitor
{
public:
    TypeCompiler(Binder *binder);

    inline QStringList qualifiedName() const { return _M_type; }
    inline QList<int> cv() const { return _M_cv; }

    bool isConstant() const;
    bool isVolatile() const;

    QStringList cvString() const;

    void run(TypeSpecifierAST *node);

protected:
    virtual void visitClassSpecifier(ClassSpecifierAST *node);
    virtual void visitEnumSpecifier(EnumSpecifierAST *node);
    virtual void visitElaboratedTypeSpecifier(ElaboratedTypeSpecifierAST *node);
    virtual void visitSimpleTypeSpecifier(SimpleTypeSpecifierAST *node);

    virtual void visitName(NameAST *node);

private:
    Binder *_M_binder;
    TokenStream *_M_token_stream;
    QStringList _M_type;
    QList<int> _M_cv;
};

#endif // TYPE_COMPILER_H

// kate: space-indent on; indent-width 2; replace-tabs on;

