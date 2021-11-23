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


#ifndef COMPILER_UTILS_H
#define COMPILER_UTILS_H

#include <utility>

#include "codemodel.h"

class QString;
class QStringList;
struct TypeSpecifierAST;
struct DeclaratorAST;
class TokenStream;
class Binder;

namespace CompilerUtils
{

TypeInfo typeDescription(TypeSpecifierAST *type_specifier, DeclaratorAST *declarator, Binder *binder);

} // namespace CompilerUtils

#endif // COMPILER_UTILS_H
