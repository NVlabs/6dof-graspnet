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

'''Test cases for a class with only a private constructor.'''

import gc
import sys
import unittest

from sample import PrivateCtor


class PrivateCtorTest(unittest.TestCase):
    '''Test case for PrivateCtor class'''

    def testPrivateCtorInstanciation(self):
        '''Test if instanciation of class with a private constructor raises an exception.'''
        self.assertRaises(TypeError, PrivateCtor)

    def testPrivateCtorInheritance(self):
        '''Test if inheriting from PrivateCtor raises an exception.'''
        def inherit():
            class Foo(PrivateCtor):
                pass
        self.assertRaises(TypeError, inherit)

    def testPrivateCtorInstanceMethod(self):
        '''Test if PrivateCtor.instance() method return the proper singleton.'''
        pd1 = PrivateCtor.instance()
        calls = pd1.instanceCalls()
        self.assertEqual(type(pd1), PrivateCtor)
        pd2 = PrivateCtor.instance()
        self.assertEqual(pd2, pd1)
        self.assertEqual(pd2.instanceCalls(), calls + 1)

    def testPrivateCtorRefCounting(self):
        '''Test refcounting of the singleton returned by PrivateCtor.instance().'''
        pd1 = PrivateCtor.instance()
        calls = pd1.instanceCalls()
        refcnt = sys.getrefcount(pd1)
        pd2 = PrivateCtor.instance()
        self.assertEqual(pd2.instanceCalls(), calls + 1)
        self.assertEqual(sys.getrefcount(pd2), sys.getrefcount(pd1))
        self.assertEqual(sys.getrefcount(pd2), refcnt + 1)
        del pd1
        self.assertEqual(sys.getrefcount(pd2), refcnt)
        del pd2
        gc.collect()
        pd3 = PrivateCtor.instance()
        self.assertEqual(type(pd3), PrivateCtor)
        self.assertEqual(pd3.instanceCalls(), calls + 2)
        self.assertEqual(sys.getrefcount(pd3), refcnt)

if __name__ == '__main__':
    unittest.main()

