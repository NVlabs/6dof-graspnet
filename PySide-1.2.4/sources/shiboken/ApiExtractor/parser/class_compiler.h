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


#ifndef CLASS_COMPILER_H
#define CLASS_COMPILER_H

#include <QtCore/qglobal.h>
#include <QtCore/QStringList>

#include <default_visitor.h>
#include <name_compiler.h>
#include <type_compiler.h>

class TokenStream;
class Binder;

class ClassCompiler: protected DefaultVisitor
{
public:
    ClassCompiler(Binder *binder);
    virtual ~ClassCompiler();

    inline QString name() const
    {
        return _M_name;
    }

    inline QStringList baseClasses() const
    {
        return _M_base_classes;
    }

    void run(ClassSpecifierAST *node);

protected:
    virtual void visitClassSpecifier(ClassSpecifierAST *node);
    virtual void visitBaseSpecifier(BaseSpecifierAST *node);

private:
    Binder *_M_binder;
    TokenStream *_M_token_stream;
    QString _M_name;
    QStringList _M_base_classes;
    NameCompiler name_cc;
    TypeCompiler type_cc;
};

#endif // CLASS_COMPILER_H

// kate: space-indent on; indent-width 2; replace-tabs on;
