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

'''Test cases for Derived class'''

import sys
import unittest

import sample
from sample import Abstract, Derived, OverloadedFuncEnum

class Deviant(Derived):
    def __init__(self):
        Derived.__init__(self)
        self.pure_virtual_called = False
        self.unpure_virtual_called = False

    def pureVirtual(self):
        self.pure_virtual_called = True

    def unpureVirtual(self):
        self.unpure_virtual_called = True

    def className(self):
        return 'Deviant'

class DerivedTest(unittest.TestCase):
    '''Test case for Derived class'''

    def testParentClassMethodsAvailability(self):
        '''Test if Derived class really inherits its methods from parent.'''
        inherited_methods = set(['callPureVirtual', 'callUnpureVirtual',
                                 'id_', 'pureVirtual', 'unpureVirtual'])
        self.assert_(inherited_methods.issubset(dir(Derived)))

    def testOverloadedMethodCall(self):
        '''Test if the correct overloaded method is being called.'''
        derived = Derived()

        result = derived.overloaded(1, 2)
        self.assertEqual(type(result), OverloadedFuncEnum)
        self.assertEqual(result, sample.OverloadedFunc_ii)

        result = derived.overloaded(3)
        self.assertEqual(type(result), OverloadedFuncEnum)
        self.assertEqual(result, sample.OverloadedFunc_ii)

        result = derived.overloaded(4.4)
        self.assertEqual(type(result), OverloadedFuncEnum)
        self.assertEqual(result, sample.OverloadedFunc_d)

    def testOtherOverloadedMethodCall(self):
        '''Another test to check overloaded method calling, just to double check.'''
        derived = Derived()

        result = derived.otherOverloaded(1, 2, True, 3.3)
        self.assertEqual(type(result), Derived.OtherOverloadedFuncEnum)
        self.assertEqual(result, sample.Derived.OtherOverloadedFunc_iibd)

        result = derived.otherOverloaded(1, 2.2)
        self.assertEqual(type(result), Derived.OtherOverloadedFuncEnum)
        self.assertEqual(result, Derived.OtherOverloadedFunc_id)

    def testOverloadedMethodCallWithDifferentNumericTypes(self):
        '''Test if the correct overloaded method accepts a different numeric type as argument.'''
        derived = Derived()
        result = derived.overloaded(1.1, 2.2)
        self.assertEqual(type(result), OverloadedFuncEnum)
        self.assertEqual(result, sample.OverloadedFunc_ii)

    def testOverloadedMethodCallWithWrongNumberOfArguments(self):
        '''Test if a call to an overloaded method with the wrong number of arguments raises an exception.'''
        derived = Derived()
        self.assertRaises(TypeError, derived.otherOverloaded, 1, 2, True)

    def testReimplementedPureVirtualMethodCall(self):
        '''Test if a Python override of a implemented pure virtual method is correctly called from C++.'''
        d = Deviant()
        d.callPureVirtual()
        self.assert_(d.pure_virtual_called)

    def testReimplementedVirtualMethodCall(self):
        '''Test if a Python override of a reimplemented virtual method is correctly called from C++.'''
        d = Deviant()
        d.callUnpureVirtual()
        self.assert_(d.unpure_virtual_called)

    def testVirtualMethodCallString(self):
        '''Test virtual method call returning string.'''
        d = Derived()
        self.assertEqual(d.className(), 'Derived')
        self.assertEqual(d.getClassName(), 'Derived')

    def testReimplementedVirtualMethodCallReturningString(self):
        '''Test if a Python override of a reimplemented virtual method is correctly called from C++.'''
        d = Deviant()
        self.assertEqual(d.className(), 'Deviant')
        self.assertEqual(d.getClassName(), 'Deviant')

    def testSingleArgument(self):
        '''Test singleArgument call.'''
        d = Derived()
        self.assert_(d.singleArgument(False))
        self.assert_(not d.singleArgument(True))

    def testMethodCallWithDefaultValue(self):
        '''Test method call with default value.'''
        d = Derived()
        self.assertEqual(d.defaultValue(3), 3.1)
        self.assertEqual(d.defaultValue(), 0.1)

    def testCallToMethodWithAbstractArgument(self):
        '''Call to method that expects an Abstract argument.'''
        objId = 123
        d = Derived(objId)
        self.assertEqual(Abstract.getObjectId(d), objId)

    def testObjectCreationWithParentType(self):
        '''Derived class creates an instance of itself in C++ and returns it as a pointer to its ancestor Abstract.'''
        obj = Derived.createObject()
        self.assertEqual(type(obj), Derived)

if __name__ == '__main__':
    unittest.main()

