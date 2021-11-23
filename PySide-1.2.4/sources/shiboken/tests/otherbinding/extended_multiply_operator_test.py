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

'''Test cases for libsample's Point multiply operator defined in libother module.'''

import unittest

from sample import Point
from other import Number

class PointOperationsWithNumber(unittest.TestCase):
    '''Test cases for libsample's Point multiply operator defined in libother module.'''

    def testPointTimesInt(self):
        '''sample.Point * int'''
        pt1 = Point(2, 7)
        num = 3
        pt2 = Point(pt1.x() * num, pt1.y() * num)
        self.assertEqual(pt1 * num, pt2)

    def testIntTimesPoint(self):
        '''int * sample.Point'''
        pt1 = Point(2, 7)
        num = 3
        pt2 = Point(pt1.x() * num, pt1.y() * num)
        self.assertEqual(num * pt1, pt2)

    def testPointTimesNumber(self):
        '''sample.Point * other.Number'''
        pt = Point(2, 7)
        num = Number(11)
        self.assertEqual(pt * num, pt * 11)

if __name__ == '__main__':
    unittest.main()

