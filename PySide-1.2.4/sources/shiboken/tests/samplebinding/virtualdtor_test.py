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

'''Test cases for virtual destructor.'''

import sys
import unittest

from sample import VirtualDtor

class ExtendedVirtualDtor(VirtualDtor):
    def __init__(self):
        VirtualDtor.__init__(self)

class VirtualDtorTest(unittest.TestCase):
    '''Test case for virtual destructor.'''

    def setUp(self):
        VirtualDtor.resetDtorCounter()

    def testVirtualDtor(self):
        '''Original virtual destructor is being called.'''
        dtor_called = VirtualDtor.dtorCalled()
        for i in range(1, 10):
            vd = VirtualDtor()
            del vd
            self.assertEqual(VirtualDtor.dtorCalled(), dtor_called + i)

    def testVirtualDtorOnCppCreatedObject(self):
        '''Original virtual destructor is being called for a C++ created object.'''
        dtor_called = VirtualDtor.dtorCalled()
        for i in range(1, 10):
            vd = VirtualDtor.create()
            del vd
            self.assertEqual(VirtualDtor.dtorCalled(), dtor_called + i)

    def testDtorOnDerivedClass(self):
        '''Original virtual destructor is being called for a derived class.'''
        dtor_called = ExtendedVirtualDtor.dtorCalled()
        for i in range(1, 10):
            evd = ExtendedVirtualDtor()
            del evd
            self.assertEqual(ExtendedVirtualDtor.dtorCalled(), dtor_called + i)


if __name__ == '__main__':
    unittest.main()

