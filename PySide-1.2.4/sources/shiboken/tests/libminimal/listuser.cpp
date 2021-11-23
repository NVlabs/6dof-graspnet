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

#include <numeric>
#include <cstdlib>
#include "listuser.h"

std::list<int>
ListUser::createIntList(int num)
{
    std::list<int> retval;
    for (int i = 0; i < num; ++i)
        retval.push_back(i);
    return retval;
}

int
ListUser::sumIntList(std::list<int> intList)
{
    int total = 0;
    for (std::list<int>::iterator iter = intList.begin(); iter != intList.end(); iter++)
        total += *iter;
    return total;
}

std::list<MinBool>
ListUser::createMinBoolList(MinBool mb1, MinBool mb2)
{
    std::list<MinBool> retval;
    retval.push_back(mb1);
    retval.push_back(mb2);
    return retval;
}

MinBool
ListUser::oredMinBoolList(std::list<MinBool> minBoolList)
{
    MinBool result(false);
    for (std::list<MinBool>::iterator iter = minBoolList.begin(); iter != minBoolList.end(); iter++)
        result |= *iter;
    return result;
}

std::list<Val>
ListUser::createValList(int num)
{
    std::list<Val> retval;
    for (int i = 0; i < num; ++i)
        retval.push_back(Val(i));
    return retval;
}

int
ListUser::sumValList(std::list<Val> valList)
{
    int total = 0;
    for (std::list<Val>::iterator iter = valList.begin(); iter != valList.end(); iter++)
        total += iter->valId();
    return total;
}

std::list<Obj*>
ListUser::createObjList(Obj* o1, Obj* o2)
{
    std::list<Obj*> retval;
    retval.push_back(o1);
    retval.push_back(o2);
    return retval;
}

int
ListUser::sumObjList(std::list<Obj*> objList)
{
    int total = 0;
    for (std::list<Obj*>::iterator iter = objList.begin(); iter != objList.end(); iter++)
        total += (*iter)->objId();
    return total;
}

std::list<std::list<int> >
ListUser::createListOfIntLists(int num)
{
    std::list<std::list<int> > retval;
    for (int i = 0; i < num; ++i)
        retval.push_back(createIntList(num));
    return retval;
}

int
ListUser::sumListOfIntLists(std::list<std::list<int> > intListList)
{
    int total = 0;
    for (std::list<std::list<int> >::iterator it0 = intListList.begin(); it0 != intListList.end(); it0++) {
        for (std::list<int>::iterator it1 = (*it0).begin(); it1 != (*it0).end(); it1++)
            total += *it1;
    }
    return total;
}

