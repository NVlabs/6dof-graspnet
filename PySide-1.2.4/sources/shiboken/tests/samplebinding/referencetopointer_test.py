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

'''Test cases for a reference to pointer argument type.'''

import unittest

from sample import VirtualMethods, Str

class ExtendedVirtualMethods(VirtualMethods):
    def __init__(self):
        VirtualMethods.__init__(self)
        self.prefix = 'Ext'

    def createStr(self, text):
        ext_text = text
        if text is not None:
            ext_text = self.prefix + text
        return VirtualMethods.createStr(self, ext_text)


class ReferenceToPointerTest(unittest.TestCase):
    '''Test cases for a reference to pointer argument type.'''

    def testSimpleCallWithNone(self):
        '''Simple call to createStr method with a None argument.'''
        obj = VirtualMethods()
        ok, string = obj.createStr(None)
        self.assertFalse(ok)
        self.assertEqual(string, None)

    def testSimpleCallWithString(self):
        '''Simple call to createStr method with a Python string argument.'''
        obj = VirtualMethods()
        ok, string = obj.createStr('foo')
        self.assert_(ok)
        self.assertEqual(string, Str('foo'))

    def testCallNonReimplementedMethodWithNone(self):
        '''Calls createStr method from C++ with a None argument.'''
        obj = VirtualMethods()
        ok, string = obj.callCreateStr(None)
        self.assertFalse(ok)
        self.assertEqual(string, None)

    def testCallNonReimplementedMethodWithString(self):
        '''Calls createStr method from C++ with a Python string argument.'''
        obj = VirtualMethods()
        ok, string = obj.callCreateStr('foo')
        self.assertTrue(ok)
        self.assertEqual(string, Str('foo'))

    def testCallReimplementedMethodWithNone(self):
        '''Calls reimplemented createStr method from C++ with a None argument.'''
        obj = ExtendedVirtualMethods()
        ok, string = obj.callCreateStr(None)
        self.assertFalse(ok)
        self.assertEqual(string, None)

    def testCallReimplementedMethodWithString(self):
        '''Calls reimplemented createStr method from C++ with a Python string argument.'''
        obj = ExtendedVirtualMethods()
        ok, string = obj.callCreateStr('foo')
        self.assert_(ok)
        self.assertEqual(string, Str(obj.prefix + 'foo'))

if __name__ == '__main__':
    unittest.main()

