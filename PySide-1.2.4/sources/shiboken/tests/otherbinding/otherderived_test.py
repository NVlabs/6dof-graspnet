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

'''Test cases for OtherDerived class'''

import sys
import unittest

from sample import Abstract, Derived
from other import OtherDerived, Number

class Multiple(Derived, Number):
    def __init__(self):
        Derived.__init__(self, 42)
        Number.__init__(self, 42)

    def testCall(self):
        return True

class OtherDeviant(OtherDerived):
    def __init__(self):
        OtherDerived.__init__(self)
        self.pure_virtual_called = False
        self.unpure_virtual_called = False

    def pureVirtual(self):
        self.pure_virtual_called = True

    def unpureVirtual(self):
        self.unpure_virtual_called = True

    def className(self):
        return 'OtherDeviant'

class MultipleTest(unittest.TestCase):
    '''Test case for Multiple derived class'''

    def testConstructor(self):
        o = Multiple()
        self.assert_(isinstance(o, Multiple))
        self.assert_(isinstance(o, Number))
        self.assert_(isinstance(o, Derived))
        del o

    def testMethodCall(self):
        o = Multiple()
        self.assert_(o.id_(), 42)
        self.assert_(o.value(), 42)
        self.assert_(o.testCall())

class OtherDerivedTest(unittest.TestCase):
    '''Test case for OtherDerived class'''

    def testParentClassMethodsAvailability(self):
        '''Test if OtherDerived class really inherits its methods from parent.'''
        inherited_methods = set(['callPureVirtual', 'callUnpureVirtual',
                                 'id_', 'pureVirtual', 'unpureVirtual'])
        self.assert_(inherited_methods.issubset(dir(OtherDerived)))

    def testReimplementedPureVirtualMethodCall(self):
        '''Test if a Python override of a implemented pure virtual method is correctly called from C++.'''
        d = OtherDeviant()
        d.callPureVirtual()
        self.assert_(d.pure_virtual_called)

    def testReimplementedVirtualMethodCall(self):
        '''Test if a Python override of a reimplemented virtual method is correctly called from C++.'''
        d = OtherDeviant()
        d.callUnpureVirtual()
        self.assert_(d.unpure_virtual_called)

    def testVirtualMethodCallString(self):
        '''Test virtual method call returning string.'''
        d = OtherDerived()
        self.assertEqual(d.className(), 'OtherDerived')
        self.assertEqual(d.getClassName(), 'OtherDerived')

    def testReimplementedVirtualMethodCallReturningString(self):
        '''Test if a Python override of a reimplemented virtual method is correctly called from C++.'''
        d = OtherDeviant()
        self.assertEqual(d.className(), 'OtherDeviant')
        self.assertEqual(d.getClassName(), 'OtherDeviant')

    def testCallToMethodWithAbstractArgument(self):
        '''Call to method that expects an Abstract argument.'''
        objId = 123
        d = OtherDerived(objId)
        self.assertEqual(Abstract.getObjectId(d), objId)

if __name__ == '__main__':
    unittest.main()

