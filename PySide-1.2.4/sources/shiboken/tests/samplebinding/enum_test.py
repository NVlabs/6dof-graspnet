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

'''Test cases for Python representation of C++ enums.'''

import os
import sys
import unittest

import sample
from sample import SampleNamespace, ObjectType, Event
from py3kcompat import IS_PY3K, b

def createTempFile():
    if sys.version_info >= (2, 6):
        import tempfile
        return tempfile.SpooledTemporaryFile(mode='rw')
    else:
        return os.tmpfile()

class EnumTest(unittest.TestCase):
    '''Test case for Python representation of C++ enums.'''

    def testEnumRepr(self):
        enum = SampleNamespace.Option(1)
        self.assertEqual(eval(repr(enum)), enum)

        enum = SampleNamespace.Option(999)
        self.assertEqual(eval(repr(enum)), enum)

    def testHashability(self):
        self.assertEqual(hash(SampleNamespace.TwoIn), hash(SampleNamespace.TwoOut))
        self.assertNotEqual(hash(SampleNamespace.TwoIn), hash(SampleNamespace.OneIn))

    def testEnumValuesInsideEnum(self):
        '''Enum values should be accessible inside the enum as well as outside.'''
        for value_name in SampleNamespace.Option.values:
            enum_item1 = getattr(SampleNamespace.Option, value_name)
            enum_item2 = getattr(SampleNamespace, value_name)
            self.assertEqual(enum_item1, enum_item2)

    def testPassingIntegerOnEnumArgument(self):
        '''Tries to use an integer in place of an enum argument.'''
        self.assertRaises(TypeError, SampleNamespace.getNumber, 1)

    def testBuildingEnumFromIntegerValue(self):
        '''Tries to build the proper enum using an integer.'''
        SampleNamespace.getNumber(SampleNamespace.Option(1))

    def testBuildingEnumWithDefaultValue(self):
        '''Enum constructor with default value'''
        enum = SampleNamespace.Option()
        self.assertEqual(enum, SampleNamespace.None_)

    def testEnumConversionToAndFromPython(self):
        '''Conversion of enum objects from Python to C++ back again.'''
        enumout = SampleNamespace.enumInEnumOut(SampleNamespace.TwoIn)
        self.assertTrue(enumout, SampleNamespace.TwoOut)
        self.assertEqual(repr(enumout), repr(SampleNamespace.TwoOut))

    def testEnumConstructorWithTooManyParameters(self):
        '''Calling the constructor of non-extensible enum with the wrong number of parameters.'''
        self.assertRaises(TypeError, SampleNamespace.InValue, 13, 14)

    def testEnumConstructorWithNonNumberParameter(self):
        '''Calling the constructor of non-extensible enum with a string.'''
        self.assertRaises(TypeError, SampleNamespace.InValue, '1')

    def testEnumItemAsDefaultValueToIntArgument(self):
        '''Calls function with an enum item as default value to an int argument.'''
        self.assertEqual(SampleNamespace.enumItemAsDefaultValueToIntArgument(), SampleNamespace.ZeroIn)
        self.assertEqual(SampleNamespace.enumItemAsDefaultValueToIntArgument(SampleNamespace.ZeroOut), SampleNamespace.ZeroOut)
        self.assertEqual(SampleNamespace.enumItemAsDefaultValueToIntArgument(123), 123)

    def testAnonymousGlobalEnums(self):
        '''Checks availability of anonymous global enum items.'''
        self.assertEqual(sample.AnonymousGlobalEnum_Value0, 0)
        self.assertEqual(sample.AnonymousGlobalEnum_Value1, 1)

    def testAnonymousClassEnums(self):
        '''Checks availability of anonymous class enum items.'''
        self.assertEqual(SampleNamespace.AnonymousClassEnum_Value0, 0)
        self.assertEqual(SampleNamespace.AnonymousClassEnum_Value1, 1)

    def testEnumTpPrintImplementation(self):
        '''Without SbkEnum.tp_print 'print' returns the enum represented as an int.'''
        tmpfile = createTempFile()
        if IS_PY3K:
            from py3k import printToFile
            printToFile(tmpfile, Event.ANY_EVENT)
        else:
            sys.stdout = tmpfile
            print(Event.ANY_EVENT)
            sys.stdout = sys.__stdout__
        tmpfile.seek(0)
        text = tmpfile.read().strip()
        tmpfile.close()
        self.assertEqual(text, str(Event.ANY_EVENT))
        self.assertEqual(text, repr(Event.ANY_EVENT))

    def testEnumArgumentWithDefaultValue(self):
        '''Option enumArgumentWithDefaultValue(Option opt = UnixTime);'''
        self.assertEqual(SampleNamespace.enumArgumentWithDefaultValue(), SampleNamespace.UnixTime)
        self.assertEqual(SampleNamespace.enumArgumentWithDefaultValue(SampleNamespace.RandomNumber), SampleNamespace.RandomNumber)


class MyEvent(Event):
    def __init__(self):
        Event.__init__(self, Event.EventType(999))

class OutOfBoundsTest(unittest.TestCase):
    def testValue(self):
        e = MyEvent()
        self.assertEqual(repr(e.eventType()), 'sample.Event.EventType(999)')

    def testNoneName(self):
        e = MyEvent()
        t = e.eventType()
        self.assertEqual(t.name, None)

class EnumOverloadTest(unittest.TestCase):
    '''Test case for overloads involving enums'''

    def testWithInt(self):
        '''Overload with Enums and ints with default value'''
        o = ObjectType()

        self.assertEqual(o.callWithEnum('', Event.ANY_EVENT, 9), 81)
        self.assertEqual(o.callWithEnum('', 9), 9)

class EnumOperators(unittest.TestCase):
    '''Test case for operations on enums'''

    def testInequalitySameObject(self):
        self.assertFalse(Event.ANY_EVENT != Event.ANY_EVENT)


if __name__ == '__main__':
    unittest.main()

