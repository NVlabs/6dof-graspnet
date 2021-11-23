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

#ifndef MODELINDEX_H
#define MODELINDEX_H

#include "libsamplemacros.h"

class ModelIndex
{
public:
    ModelIndex() : m_value(0) {}
    ModelIndex(const ModelIndex& other) { m_value = other.m_value; }
    inline void setValue(int value) { m_value = value; }
    inline int value() const { return m_value; }
    static int getValue(const ModelIndex& index) { return index.value(); }
private:
    int m_value;
};

class ReferentModelIndex
{
public:
    ReferentModelIndex() {}
    ReferentModelIndex(const ModelIndex& index) : m_index(index) {}
    ReferentModelIndex(const ReferentModelIndex& other) { m_index = other.m_index; }
    inline void setValue(int value) { m_index.setValue(value); }
    inline int value() const { return m_index.value(); }
    operator const ModelIndex&() const { return m_index; }
private:
    ModelIndex m_index;
};

class PersistentModelIndex
{
public:
    PersistentModelIndex() {}
    PersistentModelIndex(const ModelIndex& index) : m_index(index) {}
    PersistentModelIndex(const PersistentModelIndex& other) { m_index = other.m_index; }
    inline void setValue(int value) { m_index.setValue(value); }
    inline int value() const { return m_index.value(); }
    operator ModelIndex() const { return m_index; }
private:
    ModelIndex m_index;
};

#endif
