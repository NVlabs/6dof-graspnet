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

'''The BlackBox class has cases of ownership transference between Python and C++.'''

import sys
import unittest

from sample import Point, BlackBox

class OwnershipInvalidateNonPolymorphicTest(unittest.TestCase):
    '''The BlackBox class has cases of ownership transference between Python and C++.'''

    def testOwnershipTransference(self):
        '''Ownership transference from Python to C++ and back again.'''
        p1 = Point(10, 20)
        bb = BlackBox()
        p1_ticket = bb.keepPoint(p1)
        self.assertRaises(RuntimeError, p1.x)
        p1_ret = bb.retrievePoint(p1_ticket)
        self.assertEqual(p1_ret, Point(10, 20))

if __name__ == '__main__':
    unittest.main()

