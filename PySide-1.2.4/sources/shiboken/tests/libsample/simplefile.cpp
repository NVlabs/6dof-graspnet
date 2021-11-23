/*
 * This file is part of the Shiboken Python Binding Generator project.
 *
 * Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
 *
 * Contact: PySide team <contact@pyside.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <stdlib.h>
#include <string.h>
#include <fstream>
#include "simplefile.h"

class SimpleFile_p
{
public:
    SimpleFile_p(const char* filename) : m_descriptor(0), m_size(0)
    {
        m_filename = strdup(filename);
    }

    ~SimpleFile_p()
    {
        free(m_filename);
    }

    char* m_filename;
    FILE* m_descriptor;
    long m_size;
};

SimpleFile::SimpleFile(const char* filename)
{
    p = new SimpleFile_p(filename);
}

SimpleFile::~SimpleFile()
{
    close();
    delete p;
}

const char* SimpleFile::filename()
{
    return p->m_filename;
}

long SimpleFile::size()
{
    return p->m_size;
}

bool
SimpleFile::open()
{
    if ((p->m_descriptor = fopen(p->m_filename, "rb")) == 0)
        return false;

    fseek(p->m_descriptor, 0, SEEK_END);
    p->m_size = ftell(p->m_descriptor);
    rewind(p->m_descriptor);

    return true;
}

void
SimpleFile::close()
{
    if (p->m_descriptor) {
        fclose(p->m_descriptor);
        p->m_descriptor = 0;
    }
}

bool
SimpleFile::exists() const
{
    std::ifstream ifile(p->m_filename);
    return ifile;
}

bool
SimpleFile::exists(const char* filename)
{
    std::ifstream ifile(filename);
    return ifile;
}

