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

'''Test cases for Collector class' shift operators.'''

import sys
import unittest

from sample import Collector, IntWrapper, ObjectType


class CollectorTest(unittest.TestCase):
    '''Test cases for Collector class' shift operators.'''

    def testLShiftOperatorSingleUse(self):
        '''Test case for using the Collector.__lshift__ operator just one time.'''
        collector = Collector()
        collector << 13
        self.assertEqual(collector.size(), 1)
        self.assertEqual(collector.items(), [13])

    def testLShiftOperatorMultipleUses(self):
        '''Test case for using the Collector.__lshift__ operator many times in the same line.'''
        collector = Collector()
        collector << 2 << 3 << 5 << 7 << 11
        self.assertEqual(collector.size(), 5)
        self.assertEqual(collector.items(), [2, 3, 5, 7, 11])

class CollectorExternalOperator(unittest.TestCase):
    '''Test cases for external operators of Collector'''

    def testLShiftExternal(self):
        '''Collector external operator'''
        collector = Collector()
        collector << IntWrapper(5)
        self.assertEqual(collector.size(), 1)
        self.assertEqual(collector.items(), [5])


class CollectorObjectType(unittest.TestCase):
    '''Test cases for Collector ObjectType'''

    def testBasic(self):
        '''Collector << ObjectType # greedy collector'''
        collector = Collector()
        obj = ObjectType()
        collector << obj
        self.assertEqual(collector.items()[0], obj.identifier())


if __name__ == '__main__':
    unittest.main()

