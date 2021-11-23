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

'''Test cases for OddBool user's primitive type conversion.'''

import unittest

from sample import OddBoolUser

class DerivedOddBoolUser (OddBoolUser):
    def returnMyselfVirtual(self):
        return OddBoolUser()
    pass

class OddBoolTest(unittest.TestCase):

    def testOddBoolUser(self):
        obuTrue = OddBoolUser()
        obuFalse = OddBoolUser()
        obuTrue.setOddBool(True)
        self.assertEqual(obuFalse.oddBool(), False)
        self.assertEqual(obuTrue.oddBool(), True)
        self.assertEqual(obuTrue.callInvertedOddBool(), False)

        self.assertEqual(obuTrue.oddBool() == True, True)
        self.assertEqual(False == obuFalse.oddBool(), True)
        self.assertEqual(obuTrue.oddBool() == obuFalse.oddBool(), False)

        self.assertEqual(obuFalse.oddBool() != True, True)
        self.assertEqual(True != obuFalse.oddBool(), True)
        self.assertEqual(obuTrue.oddBool() != obuFalse.oddBool(), True)

    def testVirtuals(self):
        dobu = DerivedOddBoolUser()
        self.assertEqual(dobu.invertedOddBool(), True)

    def testImplicitConversionWithUsersPrimitiveType(self):
        obu = OddBoolUser(True)
        self.assertTrue(obu.oddBool())
        obu = OddBoolUser(False)
        self.assertFalse(obu.oddBool())
        cpx = complex(1.0, 0.0)
        obu = OddBoolUser(cpx)
        self.assertTrue(obu.oddBool())
        cpx = complex(0.0, 0.0)
        obu = OddBoolUser(cpx)
        self.assertFalse(obu.oddBool())

if __name__ == '__main__':
    unittest.main()
