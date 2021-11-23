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

'''Test case for a class that holds a void pointer.'''

import unittest

from sample import VoidHolder, Point

class VoidHolderTest(unittest.TestCase):
    '''Test case for void pointer manipulation.'''

    def testGetVoidPointerFromCppAndPutsOnVoidHolder(self):
        '''Passes a void pointer created in C++ and to kept by VoidHolder.'''
        voidptr = VoidHolder.gimmeMeSomeVoidPointer()
        voidholder = VoidHolder(voidptr)
        self.assertEquals(voidptr, voidholder.voidPointer())

    def testPutRandomObjectInsideVoidHolder(self):
        '''Passes a C++ pointer for an object created in Python to be kept by VoidHolder.'''
        obj = Point(1, 2)
        voidholder = VoidHolder(obj)
        self.assertEquals(obj, voidholder.voidPointer())

    def testGetNoneObjectFromVoidHolder(self):
        '''A VoidHolder created without parameters returns a NULL pointer
           that should be converted to a Python None.'''
        voidholder = VoidHolder()
        self.assertEquals(voidholder.voidPointer(), None)

    def testPutPythonObjectInsideVoidHolder(self):
        '''Passes a native Python object to be kept by VoidHolder.'''
        obj = 'Foo'
        voidholder = VoidHolder(obj)
        self.assertEquals(obj, voidholder.voidPointer())



if __name__ == '__main__':
    unittest.main()

