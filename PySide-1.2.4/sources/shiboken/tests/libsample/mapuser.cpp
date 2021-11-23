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

#include <iostream>
#include "mapuser.h"

using namespace std;

std::map<std::string, std::pair<Complex, int> >
MapUser::callCreateMap()
{
    return createMap();
}


std::map<std::string, std::pair<Complex, int> >
MapUser::createMap()
{
    std::map<std::string, std::pair<Complex, int> > retval;

    std::pair<std::string, std::pair<Complex, int> >
            item0("zero", std::pair<Complex, int>(Complex(1.2, 3.4), 2));
    retval.insert(item0);

    std::pair<std::string, std::pair<Complex, int> >
            item1("one", std::pair<Complex, int>(Complex(5.6, 7.8), 3));
    retval.insert(item1);

    std::pair<std::string, std::pair<Complex, int> >
            item2("two", std::pair<Complex, int>(Complex(9.1, 2.3), 5));
    retval.insert(item2);

    return retval;
}

void
MapUser::showMap(std::map<std::string, int> mapping)
{
    std::map<std::string, int>::iterator it;
    cout << __FUNCTION__ << endl;
    for (it = mapping.begin() ; it != mapping.end(); it++)
        cout << (*it).first << " => " << (*it).second << endl;
}

