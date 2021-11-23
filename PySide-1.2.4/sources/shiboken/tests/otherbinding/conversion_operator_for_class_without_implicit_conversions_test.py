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

'''Tests calling NoImplicitConversion using a ExtendsNoImplicitConversion parameter,
   being that the latter defines a new conversion operator for the former, and this one
   has no implicit conversions.'''

import unittest

from sample import NoImplicitConversion
from other import ExtendsNoImplicitConversion

class ConversionOperatorForClassWithoutImplicitConversionsTest(unittest.TestCase):
    '''Tests calling NoImplicitConversion constructor using a ExtendsNoImplicitConversion parameter.'''

    def testNoImplicitConversion(self):
        '''Basic test to see if the NoImplicitConversion is Ok.'''
        obj = NoImplicitConversion(123)
        # NoImplicitConversion.receivesNoImplicitConversionByValue(NoImplicitConversion)
        self.assertEqual(obj.objId(), NoImplicitConversion.receivesNoImplicitConversionByValue(obj))
        # NoImplicitConversion.receivesNoImplicitConversionByPointer(NoImplicitConversion*)
        self.assertEqual(obj.objId(), NoImplicitConversion.receivesNoImplicitConversionByPointer(obj))
        # NoImplicitConversion.receivesNoImplicitConversionByReference(NoImplicitConversion&)
        self.assertEqual(obj.objId(), NoImplicitConversion.receivesNoImplicitConversionByReference(obj))

    def testPassingExtendsNoImplicitConversionAsNoImplicitConversionByValue(self):
        '''Gives an ExtendsNoImplicitConversion object to a function expecting a NoImplicitConversion, passing by value.'''
        obj = ExtendsNoImplicitConversion(123)
        self.assertEqual(obj.objId(), NoImplicitConversion.receivesNoImplicitConversionByValue(obj))

    def testPassingExtendsNoImplicitConversionAsNoImplicitConversionByReference(self):
        '''Gives an ExtendsNoImplicitConversion object to a function expecting a NoImplicitConversion, passing by reference.'''
        obj = ExtendsNoImplicitConversion(123)
        self.assertEqual(obj.objId(), NoImplicitConversion.receivesNoImplicitConversionByReference(obj))

    def testPassingExtendsNoImplicitConversionAsNoImplicitConversionByPointer(self):
        '''Gives an ExtendsNoImplicitConversion object to a function expecting a NoImplicitConversion, passing by pointer.
           This should not be accepted, since pointers should not be converted.'''
        obj = ExtendsNoImplicitConversion(123)
        self.assertRaises(TypeError, NoImplicitConversion.receivesNoImplicitConversionByPointer, obj)


if __name__ == '__main__':
    unittest.main()

