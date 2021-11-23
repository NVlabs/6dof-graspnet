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

'''Test cases for overload sorting'''

import unittest

from sample import *

class Dummy(object):
    pass

class SimpleOverloadSorting(unittest.TestCase):

    def setUp(self):
        self.obj = SortedOverload()

    def testIntDouble(self):
        '''Overloads with int and double'''
        self.assertEqual(self.obj.overload(3), "int")
        self.assertEqual(self.obj.overload(3.14), "double")

    def testImplicitConvert(self):
        '''Overloads with implicit convertible types'''
        self.assertEqual(self.obj.overload(ImplicitTarget()), "ImplicitTarget")
        self.assertEqual(self.obj.overload(ImplicitBase()), "ImplicitBase")

    def testContainer(self):
        '''Overloads with containers arguments'''
        self.assertEqual(self.obj.overload([ImplicitBase()]), "list(ImplicitBase)")

    def testPyObject(self):
        '''Overloads with PyObject args'''
        self.assertEqual(self.obj.overload(Dummy()), "PyObject")

    def testImplicitOnly(self):
        '''Passing an implicit convertible object to an overload'''
        self.assert_(self.obj.implicit_overload(ImplicitTarget()))

    def testPyObjectSort(self):
        self.assertEqual(self.obj.pyObjOverload(1, 2), "int,int")
        self.assertEqual(self.obj.pyObjOverload(object(), 2), "PyObject,int")


class DeepOverloadSorting(unittest.TestCase):

    def setUp(self):
        self.obj = SortedOverload()

    def testPyObject(self):
        '''Deep Overload - (int, PyObject *)'''
        self.assertEqual(self.obj.overloadDeep(1, Dummy()), "PyObject")

    def testImplicit(self):
        '''Deep Overload - (int, ImplicitBase *)'''
        self.assertEqual(self.obj.overloadDeep(1, ImplicitBase()), "ImplicitBase")

class EnumOverIntSorting(unittest.TestCase):
    def testEnumOverInt(self):
        ic = ImplicitConv(ImplicitConv.CtorTwo)
        self.assertEquals(ic.ctorEnum(), ImplicitConv.CtorTwo)

if __name__ == '__main__':
    unittest.main()
