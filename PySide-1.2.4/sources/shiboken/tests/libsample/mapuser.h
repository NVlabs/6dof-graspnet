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

#ifndef MAPUSER_H
#define MAPUSER_H

#include <map>
#include <list>
#include <utility>
#include <string>
#include "complex.h"
#include "bytearray.h"

#include "libsamplemacros.h"

class LIBSAMPLE_API MapUser
{
public:
    MapUser() {}
    virtual ~MapUser() {}

    virtual std::map<std::string, std::pair<Complex, int> > createMap();
    std::map<std::string, std::pair<Complex, int> > callCreateMap();

    void showMap(std::map<std::string, int> mapping);

    inline void setMap(std::map<std::string, std::list<int> > map) { m_map = map; }
    inline std::map<std::string, std::list<int> > getMap() { return m_map; }

    // Compile test
    static void pointerToMap(std::map<std::string, std::string>* arg) {}
    static void referenceToMap(std::map<std::string, std::string>& arg) {}

    inline const std::map<int, ByteArray>& passMapIntValueType(const std::map<int, ByteArray>& arg) { return arg; }

private:
    std::map<std::string, std::list<int> > m_map;
};

#endif // MAPUSER_H
