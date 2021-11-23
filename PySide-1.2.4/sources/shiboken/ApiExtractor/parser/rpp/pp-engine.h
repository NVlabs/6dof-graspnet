/*
 * This file is part of the API Extractor project.
 *
 * Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
 * Copyright 2005 Roberto Raggi <roberto@kdevelop.org>
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

#ifndef PP_ENGINE_H
#define PP_ENGINE_H

#include <string>
#include <vector>
#include "pp-scanner.h"
#include "pp-macro-expander.h"
#include "pp-environment.h"

namespace rpp
{

struct Value {
    Value() : kind(Kind_Long), l(0) {}

    enum Kind {
        Kind_Long,
        Kind_ULong,
    };

    Kind kind;

    union {
        long l;
        unsigned long ul;
    };

    inline bool is_ulong() const {
        return kind == Kind_ULong;
    }

    inline void set_ulong(unsigned long v) {
        ul = v;
        kind = Kind_ULong;
    }

    inline void set_long(long v) {
        l = v;
        kind = Kind_Long;
    }

    inline bool is_zero() const {
        return l == 0;
    }

#define PP_DEFINE_BIN_OP(name, op) \
    inline Value &name (const Value &other) \
    { \
        if (is_ulong () || other.is_ulong ()) \
            set_ulong (ul op other.ul); \
        else \
            set_long (l op other.l); \
        return *this; \
    }

    PP_DEFINE_BIN_OP(op_add, +)
    PP_DEFINE_BIN_OP(op_sub, -)
    PP_DEFINE_BIN_OP(op_mult, *)
    PP_DEFINE_BIN_OP(op_div, /)
    PP_DEFINE_BIN_OP(op_mod, %)
    PP_DEFINE_BIN_OP(op_lhs, <<)
    PP_DEFINE_BIN_OP(op_rhs, >>)
    PP_DEFINE_BIN_OP(op_lt, <)
    PP_DEFINE_BIN_OP(op_gt, >)
    PP_DEFINE_BIN_OP(op_le, <=)
    PP_DEFINE_BIN_OP(op_ge, >=)
    PP_DEFINE_BIN_OP(op_eq, ==)
    PP_DEFINE_BIN_OP(op_ne, !=)
    PP_DEFINE_BIN_OP(op_bit_and, &)
    PP_DEFINE_BIN_OP(op_bit_or, |)
    PP_DEFINE_BIN_OP(op_bit_xor, ^)
    PP_DEFINE_BIN_OP(op_and, &&)
    PP_DEFINE_BIN_OP(op_or, ||)

#undef PP_DEFINE_BIN_OP
};

class pp
{
    pp_environment &env;
    pp_macro_expander expand;
    pp_skip_identifier skip_identifier;
    pp_skip_comment_or_divop skip_comment_or_divop;
    pp_skip_blanks skip_blanks;
    pp_skip_number skip_number;
    std::vector<std::string> include_paths;
    std::string _M_current_text;

    enum { MAX_LEVEL = 512 };
    int _M_skipping[MAX_LEVEL];
    int _M_true_test[MAX_LEVEL];
    int iflevel;

    union {
        long token_value;
        unsigned long token_uvalue;
        std::string *token_text;
    };

    enum INCLUDE_POLICY {
        INCLUDE_GLOBAL,
        INCLUDE_LOCAL
    };

    enum TOKEN_TYPE {
        TOKEN_NUMBER = 1000,
        TOKEN_UNUMBER,
        TOKEN_IDENTIFIER,
        TOKEN_DEFINED,
        TOKEN_LT_LT,
        TOKEN_LT_EQ,
        TOKEN_GT_GT,
        TOKEN_GT_EQ,
        TOKEN_EQ_EQ,
        TOKEN_NOT_EQ,
        TOKEN_OR_OR,
        TOKEN_AND_AND,
    };

    enum PP_DIRECTIVE_TYPE {
        PP_UNKNOWN_DIRECTIVE,
        PP_UNNAMED_DIRECTIVE,
        PP_DEFINE,
        PP_INCLUDE,
        PP_INCLUDE_NEXT,
        PP_ELIF,
        PP_ELSE,
        PP_ENDIF,
        PP_IF,
        PP_IFDEF,
        PP_IFNDEF,
        PP_UNDEF,
        PP_PRAGMA,
        PP_ERROR,
        PP_WARNING
    };

public:
    pp(pp_environment &__env);

    inline std::back_insert_iterator<std::vector<std::string> > include_paths_inserter();

    inline void push_include_path(std::string const &__path);

    inline std::vector<std::string>::iterator include_paths_begin();
    inline std::vector<std::string>::iterator include_paths_end();

    inline std::vector<std::string>::const_iterator include_paths_begin() const;
    inline std::vector<std::string>::const_iterator include_paths_end() const;

    template <typename _InputIterator>
    inline _InputIterator eval_expression(_InputIterator __first, _InputIterator __last, Value *result);

    template <typename _OutputIterator>
    void file(std::string const &filename, _OutputIterator __result);

    template <typename _OutputIterator>
    void file(FILE *fp, _OutputIterator __result);

    template <typename _InputIterator, typename _OutputIterator>
    void operator()(_InputIterator __first, _InputIterator __last, _OutputIterator __result);

private:
    inline bool file_isdir(std::string const &__filename) const;
    inline bool file_exists(std::string const &__filename) const;
    FILE *find_include_file(std::string const &__filename, std::string *__filepath,
                            INCLUDE_POLICY __include_policy, bool __skip_current_path = false) const;

    inline int skipping() const;
    bool test_if_level();

    inline std::string fix_file_path(std::string const &filename) const;
    inline bool is_absolute(std::string const &filename) const;

    PP_DIRECTIVE_TYPE find_directive(char const *__directive, std::size_t __size) const;

    template <typename _InputIterator>
    bool find_header_protection(_InputIterator __first, _InputIterator __last, std::string *__prot);

    template <typename _InputIterator>
    _InputIterator skip(_InputIterator __first, _InputIterator __last);

    template <typename _InputIterator>
    _InputIterator eval_primary(_InputIterator __first, _InputIterator __last, Value *result);

    template <typename _InputIterator>
    _InputIterator eval_multiplicative(_InputIterator __first, _InputIterator __last, Value *result);

    template <typename _InputIterator>
    _InputIterator eval_additive(_InputIterator __first, _InputIterator __last, Value *result);

    template <typename _InputIterator>
    _InputIterator eval_shift(_InputIterator __first, _InputIterator __last, Value *result);

    template <typename _InputIterator>
    _InputIterator eval_relational(_InputIterator __first, _InputIterator __last, Value *result);

    template <typename _InputIterator>
    _InputIterator eval_equality(_InputIterator __first, _InputIterator __last, Value *result);

    template <typename _InputIterator>
    _InputIterator eval_and(_InputIterator __first, _InputIterator __last, Value *result);

    template <typename _InputIterator>
    _InputIterator eval_xor(_InputIterator __first, _InputIterator __last, Value *result);

    template <typename _InputIterator>
    _InputIterator eval_or(_InputIterator __first, _InputIterator __last, Value *result);

    template <typename _InputIterator>
    _InputIterator eval_logical_and(_InputIterator __first, _InputIterator __last, Value *result);

    template <typename _InputIterator>
    _InputIterator eval_logical_or(_InputIterator __first, _InputIterator __last, Value *result);

    template <typename _InputIterator>
    _InputIterator eval_constant_expression(_InputIterator __first, _InputIterator __last, Value *result);

    template <typename _InputIterator, typename _OutputIterator>
    _InputIterator handle_directive(char const *__directive, std::size_t __size,
                                    _InputIterator __first, _InputIterator __last, _OutputIterator __result);

    template <typename _InputIterator, typename _OutputIterator>
    _InputIterator handle_include(bool skip_current_path, _InputIterator __first, _InputIterator __last,
                                  _OutputIterator __result);

    template <typename _InputIterator>
    _InputIterator handle_define(_InputIterator __first, _InputIterator __last);

    template <typename _InputIterator>
    _InputIterator handle_if(_InputIterator __first, _InputIterator __last);

    template <typename _InputIterator>
    _InputIterator handle_else(_InputIterator __first, _InputIterator __last);

    template <typename _InputIterator>
    _InputIterator handle_elif(_InputIterator __first, _InputIterator __last);

    template <typename _InputIterator>
    _InputIterator handle_endif(_InputIterator __first, _InputIterator __last);

    template <typename _InputIterator>
    _InputIterator handle_ifdef(bool check_undefined, _InputIterator __first, _InputIterator __last);

    template <typename _InputIterator>
    _InputIterator handle_undef(_InputIterator __first, _InputIterator __last);

    template <typename _InputIterator>
    inline char peek_char(_InputIterator __first, _InputIterator __last);

    template <typename _InputIterator>
    _InputIterator next_token(_InputIterator __first, _InputIterator __last, int *kind);
};

} // namespace rpp

#endif // PP_ENGINE_H

// kate: space-indent on; indent-width 2; replace-tabs on;
