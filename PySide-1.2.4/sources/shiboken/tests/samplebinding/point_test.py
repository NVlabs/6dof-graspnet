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

'''Test cases for Point class'''

import sys
import unittest

from sample import Point
from py3kcompat import unicode

class PointTest(unittest.TestCase):
    '''Test case for Point class, including operator overloads.'''

    def testConstructor(self):
        '''Test Point class constructor.'''
        pt = Point(5.0, 2.3)
        self.assertEqual(pt.x(), 5.0)
        self.assertEqual(pt.y(), 2.3)

    def testPlusOperator(self):
        '''Test Point class + operator.'''
        pt1 = Point(5.0, 2.3)
        pt2 = Point(0.5, 3.2)
        self.assertEqual(pt1 + pt2, Point(5.0 + 0.5, 2.3 + 3.2))

    def testEqualOperator(self):
        '''Test Point class == operator.'''
        pt1 = Point(5.0, 2.3)
        pt2 = Point(5.0, 2.3)
        pt3 = Point(0.5, 3.2)
        self.assertTrue(pt1 == pt1)
        self.assertTrue(pt1 == pt2)
        self.assertFalse(pt1 == pt3)
        self.assertFalse(pt1 == object())

    def testNotEqualOperator(self):
        '''Test Point class != operator.'''
        pt1 = Point(5.0, 2.3)
        pt2 = Point(5.0, 2.3)
        self.assertRaises(NotImplementedError, pt1.__ne__, pt2)

    def testReturnNewCopy(self):
        '''Point returns a copy of itself.'''
        pt1 = Point(1.1, 2.3)
        pt2 = pt1.copy()
        self.assertEqual(pt1, pt2)
        pt2 += pt1
        self.assertFalse(pt1 == pt2)

    def testReturnConstPointer(self):
        '''Point returns a const pointer for itself.'''
        pt1 = Point(5.0, 2.3)
        refcount1 = sys.getrefcount(pt1)
        pt2 = pt1.getSelf()
        self.assertEqual(pt1, pt2)
        self.assertEqual(sys.getrefcount(pt1), refcount1 + 1)
        self.assertEqual(sys.getrefcount(pt1), sys.getrefcount(pt2))

    def testUintOverflow(self):
        pt1 = Point(0.0, 0.0)
        self.assertRaises(OverflowError, pt1.setXAsUint, 840835495615213080)
        self.assertEqual(pt1.x(), 0.0)

    def testAddedOperator(self):
        p = Point(0.0, 0.0)
        r = p - unicode('Hi')
        self.assertEqual(r, unicode('Hi'))

        # now the reverse op.
        r = unicode('Hi') - p
        self.assertEqual(r, unicode('Hi'))

    def testModifiedMethod(self):
        pt1 = Point(0.0, 0.0)
        pt2 = Point(10.0, 10.0)
        expected = Point((pt1.x() + pt2.x()) / 2.0, (pt1.y() + pt2.y()) / 2.0)
        self.assertEqual(pt1.midpoint(pt2), expected)

if __name__ == '__main__':
    unittest.main()
