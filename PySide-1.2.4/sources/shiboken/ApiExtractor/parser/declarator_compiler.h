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


#ifndef DECLARATOR_COMPILER_H
#define DECLARATOR_COMPILER_H

#include "default_visitor.h"
#include "codemodel.h"

#include <QtCore/QString>
#include <QtCore/QList>

class TokenStream;
class Binder;

class DeclaratorCompiler: protected DefaultVisitor
{
public:
    struct Parameter {
        TypeInfo type;
        QString name;
        QString defaultValueExpression;
        bool defaultValue;

        Parameter(): defaultValue(false) {}
    };

public:
    DeclaratorCompiler(Binder *binder);

    void run(DeclaratorAST *node);

    inline QString id() const {
        return _M_id;
    }
    inline QStringList arrayElements() const {
        return _M_array;
    }
    inline bool isFunction() const {
        return _M_function;
    }
    inline bool isVariadics() const {
        return _M_variadics;
    }
    inline bool isReference() const {
        return _M_reference;
    }
    inline int indirection() const {
        return _M_indirection;
    }
    inline QList<Parameter> parameters() const {
        return _M_parameters;
    }

protected:
    virtual void visitPtrOperator(PtrOperatorAST *node);
    virtual void visitParameterDeclaration(ParameterDeclarationAST *node);

private:
    Binder *_M_binder;
    TokenStream *_M_token_stream;

    bool _M_function;
    bool _M_reference;
    bool _M_variadics;
    int _M_indirection;
    QString _M_id;
    QStringList _M_array;
    QList<Parameter> _M_parameters;
};

#endif // DECLARATOR_COMPILER_H

// kate: space-indent on; indent-width 2; replace-tabs on;
