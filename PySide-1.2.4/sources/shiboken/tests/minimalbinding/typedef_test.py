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
from minimal import *
from py3kcompat import IS_PY3K

try:
    import numpy as np
except ImportError, e:
    np = None


if IS_PY3K:
    import functools
    reduce = functools.reduce


class TypedefTest(unittest.TestCase):

    def setUp(self):
        self.the_size = 8

    def test_arrayFuncInt(self):
        none = ()
        full = range(self.the_size)
        self.assertTrue(arrayFuncInt(none), "None is empty, arrayFuncInt should return true")
        self.assertFalse(arrayFuncInt(full), "Full is NOT empty, arrayFuncInt should return false")

        self.assertTrue(arrayFuncInt(np.array(none)), "None is empty, arrayFuncInt should return true")
        self.assertFalse(arrayFuncInt(np.array(full)), "Full is NOT empty, arrayFuncInt should return false")

    def test_arrayFuncIntTypedef(self):
        none = ()
        full = (1, 2, 3)
        self.assertTrue(arrayFuncIntTypedef(none), "None is empty, arrayFuncIntTypedef should return true")
        self.assertFalse(arrayFuncIntTypedef(full), "Full is NOT empty, arrayFuncIntTypedef should return false")

        self.assertTrue(arrayFuncIntTypedef(np.array(none)), "None is empty, arrayFuncIntTypedef should return true")
        self.assertFalse(arrayFuncIntTypedef(np.array(full)), "Full is NOT empty, arrayFuncIntTypedef should return false")

    def test_arrayFuncIntReturn(self):
        none = arrayFuncIntReturn(0)
        full = arrayFuncIntReturn(self.the_size)
        self.assertTrue((len(none) == 0), "none should be empty")
        self.assertTrue((len(full) == self.the_size), "full should have " + str(self.the_size) + " elements")

    def test_arrayFuncIntReturnTypedef(self):
        none = arrayFuncIntReturnTypedef(0)
        full = arrayFuncIntReturnTypedef(self.the_size)
        self.assertTrue((len(none) == 0), "none should be empty")
        self.assertTrue((len(full) == self.the_size), "full should have " + str(self.the_size) + " elements")

    def test_arrayFunc(self):
        none = ()
        full = range(self.the_size)
        self.assertTrue(arrayFunc(none), "None is empty, arrayFunc should return true")
        self.assertFalse(arrayFunc(full), "Full is NOT empty, arrayFunc should return false")

        self.assertTrue(arrayFunc(np.array(none)), "None is empty, arrayFunc should return true")
        self.assertFalse(arrayFunc(np.array(full)), "Full is NOT empty, arrayFunc should return false")

    def test_arrayFuncTypedef(self):
        none = ()
        full = (1, 2, 3)
        self.assertTrue(arrayFuncTypedef(none), "None is empty, arrayFuncTypedef should return true")
        self.assertFalse(arrayFuncTypedef(full), "Full is NOT empty, arrayFuncTypedef should return false")

        self.assertTrue(arrayFuncTypedef(np.array(none)), "None is empty, arrayFuncTypedef should return true")
        self.assertFalse(arrayFuncTypedef(np.array(full)), "Full is NOT empty, arrayFuncTypedef should return false")

    def test_arrayFuncReturn(self):
        none = arrayFuncReturn(0)
        full = arrayFuncReturn(self.the_size)
        self.assertTrue((len(none) == 0), "none should be empty")
        self.assertTrue((len(full) == self.the_size), "full should have " + str(self.the_size) + " elements")

    def test_arrayFuncReturnTypedef(self):
        none = arrayFuncReturnTypedef(0)
        full = arrayFuncReturnTypedef(self.the_size)
        self.assertTrue((len(none) == 0), "none should be empty")
        self.assertTrue((len(full) == self.the_size), "full should have " + str(self.the_size) + " elements")


if __name__ == '__main__':
    if np != None:
        unittest.main()
