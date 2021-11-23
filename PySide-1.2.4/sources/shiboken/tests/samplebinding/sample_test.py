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

'''Test cases for libsample bindings module'''

import unittest

import sample

class ModuleTest(unittest.TestCase):
    '''Test case for module and global functions'''

    def testModuleMembers(self):
        '''Test availability of classes, global functions and other members on binding'''
        expected_members = set(['Abstract', 'Derived', 'Point',
                                'ListUser', 'PairUser', 'MapUser',
                                'gimmeComplexList', 'gimmeDouble', 'gimmeInt',
                                'makeCString', 'multiplyPair', 'returnCString',
                                'SampleNamespace', 'transmuteComplexIntoPoint',
                                'transmutePointIntoComplex', 'sumComplexPair',
                                'FirstThing', 'SecondThing', 'ThirdThing',
                                'GlobalEnum', 'NoThing'])
        self.assertTrue(expected_members.issubset(dir(sample)))

    def testAbstractPrintFormatEnum(self):
        '''Test availability of PrintFormat enum from Abstract class'''
        enum_members = set(['PrintFormat', 'Short', 'Verbose',
                            'OnlyId', 'ClassNameAndId'])
        self.assertTrue(enum_members.issubset(dir(sample.Abstract)))

    def testSampleNamespaceOptionEnum(self):
        '''Test availability of Option enum from SampleNamespace namespace'''
        enum_members = set(['Option', 'None_', 'RandomNumber', 'UnixTime'])
        self.assertTrue(enum_members.issubset(dir(sample.SampleNamespace)))

    def testAddedFunctionAtModuleLevel(self):
        '''Calls function added to module from type system description.'''
        str1 = 'Foo'
        self.assertEqual(sample.multiplyString(str1, 3), str1 * 3)
        self.assertEqual(sample.multiplyString(str1, 0), str1 * 0)

    def testAddedFunctionWithVarargs(self):
        '''Calls function that receives varargs added to module from type system description.'''
        self.assertEqual(sample.countVarargs(1), 0)
        self.assertEqual(sample.countVarargs(1, 2), 1)
        self.assertEqual(sample.countVarargs(1, 2, 3, 'a', 'b', 4, (5, 6)), 6)

if __name__ == '__main__':
    unittest.main()

