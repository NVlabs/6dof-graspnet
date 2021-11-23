/*
 * This file is part of the API Extractor project.
 *
 * Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
 * Copyright (C) 2006 Roberto Raggi <roberto@kdevelop.org>
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

#ifndef CODEMODEL_POINTER_H
#define CODEMODEL_POINTER_H

#include <QtCore/QSharedData>
#include <QAtomicPointer>

template <class T> class CodeModelPointer : public QAtomicPointer<T>
{
public:
    typedef T Type;

    inline CodeModelPointer(T *value = 0) : QAtomicPointer<T>(value) {}

    inline CodeModelPointer &operator=(T *o)
    {
        QAtomicPointer<T>::operator=(o);
        return *this;
    }

    inline T *data()
    {
        return (T *) *this;
    }

    inline const T *data() const
    {
        return (const T *) *this;
    }

    inline const T *constData() const
    {
        return (const T *) *this;
    }
};

#endif // CODEMODEL_POINTER_H
