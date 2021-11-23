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

'''Test case for overflowing C++ numeric types.'''

import sys
import unittest

from sample import *

class OverflowTest(unittest.TestCase):
    '''Test case for overflowing C++ numeric types.'''

    def testUnsignedInt(self):
        '''C++ function receives an unsigned int argument and raise OverflowError if the value is negative.'''
        val = 100
        self.assertEqual(doubleUnsignedInt(val), 2 * val)
        val *= -1
        self.assertRaises(OverflowError, doubleUnsignedInt, val)

    def testLongLong(self):
        '''C++ function receives an long long argument and raise OverflowError if the value is negative.'''
        val = 100
        self.assertEqual(doubleLongLong(val), 2 * val)
        val = long(100)
        self.assertEqual(doubleLongLong(val), 2 * val)
        val = (2 << 64) + 1
        self.assertRaises(OverflowError, doubleLongLong, val)

    def testUnsignedLongLong(self):
        '''C++ function receives an unsigned long long argument and raise OverflowError if the value is negative.'''
        val = 100
        self.assertEqual(doubleUnsignedLongLong(val), 2 * val)
        val = long(100)
        self.assertEqual(doubleUnsignedLongLong(val), 2 * val)
        val *= -1
        self.assertRaises(OverflowError, doubleUnsignedLongLong, val)

    def testOverflow(self):
        '''Calls function with unsigned int parameter using an overflowing value.'''
        self.assertRaises(OverflowError, doubleUnsignedInt, 42415335332353253)
        doubleUnsignedInt(0xdeadbeef)

    def testShortOverflow(self):
        '''Calls function with short parameter using an overflowing value.'''
        doubleShort(-3)
        self.assertRaises(OverflowError, doubleShort, 0xFFFF*-1)
        self.assertRaises(OverflowError, doubleShort, 0xFFFF + 1)

    def testOverflowOnCtor(self):
        '''Calls object ctor with int parameter using overflowing values.'''
        self.assertRaises(OverflowError, Point, 42415335332353253, 42415335332353253)

if __name__ == '__main__':
    unittest.main()

