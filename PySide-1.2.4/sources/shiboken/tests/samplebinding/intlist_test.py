#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# This file is part of the Shiboken Python Bindings Generator project.
#
# Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
#
# Contact: PySide team <contact@pyside.org>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public License
# version 2.1 as published by the Free Software Foundation. Please
# review the following information to ensure the GNU Lesser General
# Public License version 2.1 requirements will be met:
# http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
# #
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
# 02110-1301 USA

import unittest

from sample import IntList

class IntListTest(unittest.TestCase):

    def testAutoFunctionsToBaseList(self):
        lst = IntList()
        self.assertEqual(len(lst), 0)
        lst.append(10)
        self.assertEqual(lst[0], 10)
        lst.append(20)
        self.assertEqual(lst[1], 20)
        lst.append(30)
        self.assertEqual(lst[2], 30)
        lst[1] = 25
        self.assertEqual(lst[0], 10)
        self.assertEqual(lst[1], 25)
        self.assertEqual(lst[2], 30)
        self.assertEqual(len(lst), 3)

    def testIntListCtor_NoParams(self):
        '''IntList constructor receives no parameter.'''
        il = IntList()
        self.assertEqual(len(il), 0)
        self.assertEqual(il.constructorUsed(), IntList.NoParamsCtor)

    def testIntListCtor_int(self):
        '''IntList constructor receives an integer.'''
        value = 123
        il = IntList(value)
        self.assertEqual(len(il), 1)
        self.assertEqual(il[0], value)
        self.assertEqual(il.constructorUsed(), IntList.IntCtor)

    def testIntListCtor_IntList(self):
        '''IntList constructor receives an IntList object.'''
        il1 = IntList(123)
        il2 = IntList(il1)
        self.assertEqual(len(il1), len(il2))
        for i in range(len(il1)):
            self.assertEqual(il1[i], il2[i])
        self.assertEqual(il2.constructorUsed(), IntList.CopyCtor)

    def testIntListCtor_ListOfInts(self):
        '''IntList constructor receives an integer list.'''
        ints = [123, 456]
        il = IntList(ints)
        self.assertEqual(len(il), len(ints))
        for i in range(len(il)):
            self.assertEqual(il[i], ints[i])
        self.assertEqual(il.constructorUsed(), IntList.ListOfIntCtor)

    def testIntListAttributeTypeCheck(self):
        '''Attribute values to IntList.'''
        il = IntList([0, 1, 2])
        self.assertEqual(len(il), 3)
        il[0] = 123
        self.assertEqual(len(il), 3)
        self.assertEqual(il[0], 123)
        il[1] = 432.1
        self.assertEqual(len(il), 3)
        self.assertEqual(il[1], int(432.1))
        self.assertRaises(TypeError, il.__setitem__, 2, '78')

if __name__ == '__main__':
    unittest.main()
