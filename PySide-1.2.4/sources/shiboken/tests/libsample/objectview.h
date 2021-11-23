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

#ifndef OBJECTVIEW_H
#define OBJECTVIEW_H

#include "objecttype.h"
#include "libsamplemacros.h"

class Str;
class ObjectModel;

class LIBSAMPLE_API ObjectView : public ObjectType
{
public:
    ObjectView(ObjectModel* model = 0, ObjectType* parent = 0)
        : ObjectType(parent), m_model(model)
    {}

    inline void setModel(ObjectModel* model) { m_model = model; }
    inline ObjectModel* model() const { return m_model; }

    Str displayModelData();
    void modifyModelData(Str& data);

    ObjectType* getRawModelData();

private:
    ObjectModel* m_model;
};

#endif // OBJECTVIEW_H

