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

'''Test cases for a class that holds an arbitraty pointer and is modified to hold an PyObject.'''

import sys
import unittest

from sample import PointerHolder

class TestPointerHolder(unittest.TestCase):
    '''Test cases for a class that holds an arbitraty pointer and is modified to hold an PyObject.'''

    def testStoringAndRetrievingPointer(self):
        ph = PointerHolder('Hello')
        self.assertEqual(ph.pointer(), 'Hello')
        a = (1, 2, 3)
        ph = PointerHolder(a)
        self.assertEqual(ph.pointer(), a)

    def testReferenceCounting(self):
        '''Test reference counting when retrieving data with PointerHolder.pointer().'''
        a = (1, 2, 3)
        refcnt = sys.getrefcount(a)
        ph = PointerHolder(a)
        ptr = ph.pointer()
        self.assertEqual(sys.getrefcount(a), refcnt + 1)

if __name__ == '__main__':
    unittest.main()

