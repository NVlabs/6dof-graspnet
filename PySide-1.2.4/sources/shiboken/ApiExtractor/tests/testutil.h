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

#ifndef TESTUTIL_H
#define TESTUTIL_H
#include <QtCore/QBuffer>
#include "abstractmetabuilder.h"
#include "reporthandler.h"
#include "typedatabase.h"

class TestUtil
{
public:
    TestUtil(const char* cppCode, const char* xmlCode,
             bool silent = true, const char* apiVersion = 0,
             QStringList dropTypeEntries = QStringList())
        : m_builder(0)
    {
        ReportHandler::setSilent(silent);
        m_builder = new AbstractMetaBuilder;
        TypeDatabase* td = TypeDatabase::instance(true);
        if (apiVersion)
            td->setApiVersion("*", apiVersion);
        td->setDropTypeEntries(dropTypeEntries);
        QBuffer buffer;
        // parse typesystem
        buffer.setData(xmlCode);
        td->parseFile(&buffer);
        buffer.close();
        // parse C++ code
        buffer.setData(cppCode);
        bool res = m_builder->build(&buffer);
        Q_UNUSED(res);
        Q_ASSERT(res);
    }

    ~TestUtil()
    {
        delete m_builder;
        m_builder = 0;
    }

    AbstractMetaBuilder* builder()
    {
        return m_builder;
    }

private:
    AbstractMetaBuilder* m_builder;
};

#endif
