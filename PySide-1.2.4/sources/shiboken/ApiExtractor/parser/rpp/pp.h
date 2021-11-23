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

#ifndef PP_H
#define PP_H

#if defined(_WIN64) || defined(WIN64) || defined(__WIN64__) \
    || defined(_WIN32) || defined(WIN32) || defined(__WIN32__)
#  define PP_OS_WIN
#endif

#include <set>
#include <map>
#include <vector>
#include <string>
#include <iterator>
#include <iostream>
#include <cassert>
#include <cctype>
#include <cstdio>

#include <fcntl.h>

#ifdef HAVE_MMAP
#  include <sys/mman.h>
#endif

#include <sys/stat.h>
#include <sys/types.h>

#if (_MSC_VER >= 1400)
#  define FILENO _fileno
#else
#  define FILENO fileno
#endif

#if defined (PP_OS_WIN)
#  define PATH_SEPARATOR '\\'
#else
#  define PATH_SEPARATOR '/'
#endif

#if defined (RPP_JAMBI)
#  include "parser/rxx_allocator.h"
#else
#  include "parser/rpp-allocator.h"
#endif

#if defined (_MSC_VER)
#  define pp_snprintf _snprintf
#else
#  define pp_snprintf snprintf
#endif

#include "pp-fwd.h"
#include "pp-cctype.h"
#include "pp-string.h"
#include "pp-symbol.h"
#include "pp-internal.h"
#include "pp-iterator.h"
#include "pp-macro.h"
#include "pp-environment.h"
#include "pp-scanner.h"
#include "pp-macro-expander.h"
#include "pp-engine.h"
#include "pp-engine-bits.h"

#endif // PP_H

// kate: space-indent on; indent-width 2; replace-tabs on;
