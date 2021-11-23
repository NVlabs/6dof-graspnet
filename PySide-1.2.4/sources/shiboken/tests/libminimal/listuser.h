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

#ifndef LISTUSER_H
#define LISTUSER_H

#include <list>
#include "obj.h"
#include "val.h"
#include "minbool.h"

#include "libminimalmacros.h"

struct LIBMINIMAL_API ListUser
{
    virtual ~ListUser() {}

    // List of C++ primitive type items
    virtual std::list<int> createIntList(int num);
    std::list<int> callCreateIntList(int num) { return createIntList(num); }
    virtual int sumIntList(std::list<int> intList);
    int callSumIntList(std::list<int> intList) { return sumIntList(intList); }

    // List of C++ MinBool objects used as primitives in Python
    virtual std::list<MinBool> createMinBoolList(MinBool mb1, MinBool mb2);
    std::list<MinBool> callCreateMinBoolList(MinBool mb1, MinBool mb2) { return createMinBoolList(mb1, mb2); }
    virtual MinBool oredMinBoolList(std::list<MinBool> minBoolList);
    MinBool callOredMinBoolList(std::list<MinBool> minBoolList) { return oredMinBoolList(minBoolList); }

    // List of C++ value types
    virtual std::list<Val> createValList(int num);
    std::list<Val> callCreateValList(int num) { return createValList(num); }
    virtual int sumValList(std::list<Val> valList);
    int callSumValList(std::list<Val> valList) { return sumValList(valList); }

    // List of C++ object types
    virtual std::list<Obj*> createObjList(Obj* o1, Obj* o2);
    std::list<Obj*> callCreateObjList(Obj* o1, Obj* o2) { return createObjList(o1, o2); }
    virtual int sumObjList(std::list<Obj*> objList);
    int callSumObjList(std::list<Obj*> objList) { return sumObjList(objList); }

    // List of lists of C++ primitive type items
    virtual std::list<std::list<int> > createListOfIntLists(int num);
    std::list<std::list<int> > callCreateListOfIntLists(int num) { return createListOfIntLists(num); }
    virtual int sumListOfIntLists(std::list<std::list<int> > intListList);
    int callSumListOfIntLists(std::list<std::list<int> > intListList) { return sumListOfIntLists(intListList); }
};

#endif // LISTUSER_H

