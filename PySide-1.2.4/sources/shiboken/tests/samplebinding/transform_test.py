#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2013 Kitware, Inc.
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

'''Test cases for argument modification with more than nine arguments.'''

import unittest

from sample import Point, applyHomogeneousTransform

class TransformTest(unittest.TestCase):
    '''Test cases for modifying a function with > 9 arguments.'''

    def testTransform_ValidMatrix(self):
        '''Transform applies successfully.'''
        p = Point(3, 4)
        r = applyHomogeneousTransform(p, 0, 1, 0, -1, 0, 0, 0, 0, 1)
        self.assertTrue(type(r) is Point)
        self.assertEqual(r.x(), 4)
        self.assertEqual(r.y(), -3)

    def testTransform_InvalidMatrix(self):
        '''Transform does not apply successfully.'''
        p = Point(3, 4)
        r = applyHomogeneousTransform(p, 1, 0, 0, 0, 1, 0, 0, 0, 0)
        self.assertTrue(r is None)

if __name__ == '__main__':
    unittest.main()
