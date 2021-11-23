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

'''Test cases for operator overloads on Size class'''

import unittest

from sample import Size

class PointTest(unittest.TestCase):
    '''Test case for Size class, including operator overloads.'''

    def testConstructor(self):
        '''Test Size class constructor.'''
        width, height = (5.0, 2.3)
        size = Size(width, height)
        self.assertEqual(size.width(), width)
        self.assertEqual(size.height(), height)
        self.assertEqual(size.calculateArea(), width * height)

    def testCopyConstructor(self):
        '''Test Size class copy constructor.'''
        width, height = (5.0, 2.3)
        s1 = Size(width, height)
        s2 = Size(s1)
        self.assertFalse(s1 is s2)
        self.assertEqual(s1, s2)

    def testPlusOperator(self):
        '''Test Size class + operator.'''
        s1 = Size(5.0, 2.3)
        s2 = Size(0.5, 3.2)
        self.assertEqual(s1 + s2, Size(5.0 + 0.5, 2.3 + 3.2))

    def testEqualOperator(self):
        '''Test Size class == operator.'''
        s1 = Size(5.0, 2.3)
        s2 = Size(5.0, 2.3)
        s3 = Size(0.5, 3.2)
        self.assertTrue(s1 == s1)
        self.assertTrue(s1 == s2)
        self.assertFalse(s1 == s3)

    def testNotEqualOperator(self):
        '''Test Size class != operator.'''
        s1 = Size(5.0, 2.3)
        s2 = Size(5.0, 2.3)
        s3 = Size(0.5, 3.2)
        self.assertFalse(s1 != s1)
        self.assertFalse(s1 != s2)
        self.assertTrue(s1 != s3)

    def testMinorEqualOperator(self):
        '''Test Size class <= operator.'''
        s1 = Size(5.0, 2.3)
        s2 = Size(5.0, 2.3)
        s3 = Size(0.5, 3.2)
        self.assertTrue(s1 <= s1)
        self.assertTrue(s1 <= s2)
        self.assertTrue(s3 <= s1)
        self.assertFalse(s1 <= s3)

    def testMinorOperator(self):
        '''Test Size class < operator.'''
        s1 = Size(5.0, 2.3)
        s2 = Size(0.5, 3.2)
        self.assertFalse(s1 < s1)
        self.assertFalse(s1 < s2)
        self.assertTrue(s2 < s1)

    def testMajorEqualOperator(self):
        '''Test Size class >= operator.'''
        s1 = Size(5.0, 2.3)
        s2 = Size(5.0, 2.3)
        s3 = Size(0.5, 3.2)
        self.assertTrue(s1 >= s1)
        self.assertTrue(s1 >= s2)
        self.assertTrue(s1 >= s3)
        self.assertFalse(s3 >= s1)

    def testMajorOperator(self):
        '''Test Size class > operator.'''
        s1 = Size(5.0, 2.3)
        s2 = Size(0.5, 3.2)
        self.assertFalse(s1 > s1)
        self.assertTrue(s1 > s2)
        self.assertFalse(s2 > s1)

if __name__ == '__main__':
    unittest.main()

