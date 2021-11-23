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

'''Test cases for a class with a private destructor.'''

import gc
import sys
import unittest

import shiboken
from sample import PrivateDtor


class PrivateDtorTest(unittest.TestCase):
    '''Test case for PrivateDtor class'''

    def testPrivateDtorInstanciation(self):
        '''Test if instanciation of class with a private destructor raises an exception.'''
        self.assertRaises(TypeError, PrivateDtor)

    def testPrivateDtorInheritance(self):
        '''Test if inheriting from PrivateDtor raises an exception.'''
        def inherit():
            class Foo(PrivateDtor):
                pass
        self.assertRaises(TypeError, inherit)

    def testPrivateDtorInstanceMethod(self):
        '''Test if PrivateDtor.instance() method return the proper singleton.'''
        pd1 = PrivateDtor.instance()
        calls = pd1.instanceCalls()
        self.assertEqual(type(pd1), PrivateDtor)
        pd2 = PrivateDtor.instance()
        self.assertEqual(pd2, pd1)
        self.assertEqual(pd2.instanceCalls(), calls + 1)

    def testPrivateDtorRefCounting(self):
        '''Test refcounting of the singleton returned by PrivateDtor.instance().'''
        pd1 = PrivateDtor.instance()
        calls = pd1.instanceCalls()
        refcnt = sys.getrefcount(pd1)
        pd2 = PrivateDtor.instance()
        self.assertEqual(pd2.instanceCalls(), calls + 1)
        self.assertEqual(sys.getrefcount(pd2), sys.getrefcount(pd1))
        self.assertEqual(sys.getrefcount(pd2), refcnt + 1)
        del pd1
        self.assertEqual(sys.getrefcount(pd2), refcnt)
        del pd2
        gc.collect()
        pd3 = PrivateDtor.instance()
        self.assertEqual(type(pd3), PrivateDtor)
        self.assertEqual(pd3.instanceCalls(), calls + 2)
        self.assertEqual(sys.getrefcount(pd3), refcnt)

    def testClassDecref(self):
        # Bug was that class PyTypeObject wasn't decrefed when instance
        # was invalidated

        before = sys.getrefcount(PrivateDtor)

        for i in range(1000):
            obj = PrivateDtor.instance()
            shiboken.invalidate(obj)

        after = sys.getrefcount(PrivateDtor)

        self.assertLess(abs(before - after), 5)

if __name__ == '__main__':
    unittest.main()

