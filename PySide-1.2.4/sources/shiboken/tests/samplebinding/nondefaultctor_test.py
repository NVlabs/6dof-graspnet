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

'''Test cases for ...'''

import sys
import unittest

from sample import NonDefaultCtor

class DerivedNonDefaultCtor (NonDefaultCtor):
    def returnMyselfVirtual(self):
        return NonDefaultCtor(self.value()+1)

class AnotherDerivedNonDefaultCtor (NonDefaultCtor):
    def __init__(self, some_string):
        pass

class NonDefaultCtorTest(unittest.TestCase):

    def testNonDefaultCtor(self):
        c = NonDefaultCtor(2)
        # these functions returns NonDefaultCtor by value, so a PyObjecy  is created every time
        self.assertNotEqual(c.returnMyself(), c)
        self.assertEqual(c.returnMyself().value(), 2)
        self.assertNotEqual(c.returnMyself(3), c)
        self.assertEqual(c.returnMyself(3).value(), 2)
        self.assertNotEqual(c.returnMyself(4, c), c)
        self.assertEqual(c.returnMyself(4, c).value(), 2)

    def testVirtuals(self):
        c = DerivedNonDefaultCtor(3)
        # these functions returns NonDefaultCtor by value, so a PyObjecy  is created every time
        self.assertNotEqual(c.returnMyselfVirtual(), c)
        self.assertEqual(c.returnMyselfVirtual().value(), 4)
        self.assertEqual(c.callReturnMyselfVirtual().value(), 4)

    def testCtorOverload(self):
        c = AnotherDerivedNonDefaultCtor("testing")

if __name__ == '__main__':
    unittest.main()

