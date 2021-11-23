/*
* This file is part of the API Extractor project.
*
* Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
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

#ifndef GENERATORRUNNERMACROS_H
#define GENERATORRUNNERMACROS_H

// GENRUNNER_API is used for the public API symbols.
#if defined _WIN32
    #define GENRUNNER_EXPORT __declspec(dllexport)
    #if GENRUNNER_EXPORTS
        #define GENRUNNER_API GENRUNNER_EXPORT
    #endif
#elif __GNUC__ >= 4
    #define GENRUNNER_EXPORT __attribute__ ((visibility("default")))
    #define GENRUNNER_API GENRUNNER_EXPORT
#elif __GNUC__ < 4
    #define GENRUNNER_EXPORT
#endif

#ifndef GENRUNNER_API
    #define GENRUNNER_API
#endif
#endif
