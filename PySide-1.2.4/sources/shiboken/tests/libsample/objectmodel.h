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

#ifndef OBJECTMODEL_H
#define OBJECTMODEL_H

#include "objecttype.h"
#include "libsamplemacros.h"

class LIBSAMPLE_API ObjectModel : public ObjectType
{
public:
    explicit ObjectModel(ObjectType* parent = 0)
        : ObjectType(parent), m_data(0)
    {}

    void setData(ObjectType* data);
    virtual ObjectType* data() const;

    // The MethodCalled enum and related static methods were created to
    // test bug #630 [http://bugs.openbossa.org/show_bug.cgi?id=630]
    enum MethodCalled { ObjectTypeCalled, ObjectModelCalled };
    static MethodCalled receivesObjectTypeFamily(const ObjectType& object) { return ObjectModel::ObjectTypeCalled; }
    static MethodCalled receivesObjectTypeFamily(const ObjectModel& object) { return ObjectModel::ObjectModelCalled; }

private:
    // The model holds only one piece of data.
    // (This is just a test after all.)
    ObjectType* m_data;
};

#endif // OBJECTMODEL_H

