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

#ifndef LIST_H
#define LIST_H

#include <list>
#include "libsamplemacros.h"
#include "point.h"

class ObjectType;

template<class T>
class List : public std::list<T>
{
};

class IntList : public List<int>
{
public:
    enum CtorEnum {
        NoParamsCtor,
        IntCtor,
        CopyCtor,
        ListOfIntCtor
    };

    inline IntList() : m_ctorUsed(NoParamsCtor) {}
    inline explicit IntList(int val) : m_ctorUsed(IntCtor) { push_back(val); }
    inline IntList(const IntList& lst) : List<int>(lst), m_ctorUsed(CopyCtor) {}
    inline IntList(const List<int>& lst) : List<int>(lst), m_ctorUsed(ListOfIntCtor) {}

    inline void append(int v) { insert(end(), v); }
    CtorEnum constructorUsed() { return m_ctorUsed; }
private:
    CtorEnum m_ctorUsed;
};

class PointValueList : public List<Point>
{
public:
    enum CtorEnum {
        NoParamsCtor,
        PointCtor,
        CopyCtor,
        ListOfPointValuesCtor
    };

    inline PointValueList() : m_ctorUsed(NoParamsCtor) {}
    inline explicit PointValueList(Point val) : m_ctorUsed(PointCtor) { push_back(val); }
    inline PointValueList(const PointValueList& lst) : List<Point>(lst), m_ctorUsed(CopyCtor) {}
    inline PointValueList(const List<Point>& lst) : List<Point>(lst), m_ctorUsed(ListOfPointValuesCtor) {}

    inline void append(Point v) { insert(end(), v); }
    CtorEnum constructorUsed() { return m_ctorUsed; }
private:
    CtorEnum m_ctorUsed;
};

class ObjectTypePtrList : public List<ObjectType*>
{
public:
    enum CtorEnum {
        NoParamsCtor,
        ObjectTypeCtor,
        CopyCtor,
        ListOfObjectTypePtrCtor
    };

    inline ObjectTypePtrList() : m_ctorUsed(NoParamsCtor) {}
    inline explicit ObjectTypePtrList(ObjectType* val) : m_ctorUsed(ObjectTypeCtor) { push_back(val); }
    inline ObjectTypePtrList(const ObjectTypePtrList& lst) : List<ObjectType*>(lst), m_ctorUsed(CopyCtor) {}
    inline ObjectTypePtrList(const List<ObjectType*>& lst) : List<ObjectType*>(lst), m_ctorUsed(ListOfObjectTypePtrCtor) {}

    inline void append(ObjectType* v) { insert(end(), v); }
    CtorEnum constructorUsed() { return m_ctorUsed; }
private:
    CtorEnum m_ctorUsed;
};

#endif // LIST_H
