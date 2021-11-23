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

'''Test cases for virtual methods.'''

import types
import unittest
from py3kcompat import IS_PY3K

from sample import VirtualMethods, SimpleFile, Point

def MethodTypeCompat(func, instance):
    if IS_PY3K:
        return types.MethodType(func, instance)
    else:
        return types.MethodType(func, instance, type(instance))

class Duck(VirtualMethods):
    def __init__(self):
        VirtualMethods.__init__(self)

class Monkey(SimpleFile):
    def __init__(self, filename):
        SimpleFile.__init__(self, filename)

class DuckPunchingTest(unittest.TestCase):
    '''Test case for duck punching (aka "monkey patching").'''

    def setUp(self):
        self.multiplier = 2.0
        self.duck_method_called = False
        self.call_counter = 0

    def testMonkeyPatchOnVirtualMethod(self):
        '''Injects new 'virtualMethod0' on a VirtualMethods instance and makes C++ call it.'''
        vm = VirtualMethods()
        pt, val, cpx, b = Point(1.1, 2.2), 4, complex(3.3, 4.4), True

        result1 = vm.virtualMethod0(pt, val, cpx, b)
        result2 = vm.callVirtualMethod0(pt, val, cpx, b)
        self.assertEqual(result1, result2)
        self.assertEqual(result1, VirtualMethods.virtualMethod0(vm, pt, val, cpx, b))

        def myVirtualMethod0(obj, pt, val, cpx, b):
            self.duck_method_called = True
            return VirtualMethods.virtualMethod0(obj, pt, val, cpx, b) * self.multiplier
        vm.virtualMethod0 = MethodTypeCompat(myVirtualMethod0, vm)

        result1 = vm.callVirtualMethod0(pt, val, cpx, b)
        self.assert_(self.duck_method_called)

        result2 = vm.virtualMethod0(pt, val, cpx, b)
        self.assertEqual(result1, result2)
        self.assertEqual(result1, VirtualMethods.virtualMethod0(vm, pt, val, cpx, b) * self.multiplier)

        # This is done to decrease the refcount of the vm object
        # allowing the object wrapper to be deleted before the
        # BindingManager. This is useful when compiling Shiboken
        # for debug, since the BindingManager destructor has an
        # assert that checks if the wrapper mapper is empty.
        vm.virtualMethod0 = None

    def testMonkeyPatchOnVirtualMethodWithInheritance(self):
        '''Injects new 'virtualMethod0' on an object that inherits from VirtualMethods and makes C++ call it.'''
        duck = Duck()
        pt, val, cpx, b = Point(1.1, 2.2), 4, complex(3.3, 4.4), True

        result1 = duck.virtualMethod0(pt, val, cpx, b)
        result2 = duck.callVirtualMethod0(pt, val, cpx, b)
        self.assertEqual(result1, result2)
        self.assertEqual(result1, VirtualMethods.virtualMethod0(duck, pt, val, cpx, b))

        def myVirtualMethod0(obj, pt, val, cpx, b):
            self.duck_method_called = True
            return VirtualMethods.virtualMethod0(obj, pt, val, cpx, b) * self.multiplier
        duck.virtualMethod0 = MethodTypeCompat(myVirtualMethod0, duck)

        result1 = duck.callVirtualMethod0(pt, val, cpx, b)
        self.assert_(self.duck_method_called)

        result2 = duck.virtualMethod0(pt, val, cpx, b)
        self.assertEqual(result1, result2)
        self.assertEqual(result1, VirtualMethods.virtualMethod0(duck, pt, val, cpx, b) * self.multiplier)

        duck.virtualMethod0 = None

    def testMonkeyPatchOnMethodWithStaticAndNonStaticOverloads(self):
        '''Injects new 'exists' on a SimpleFile instance and makes C++ call it.'''
        simplefile = SimpleFile('foobar')

        # Static 'exists'
        simplefile.exists('sbrubbles')
        self.assertFalse(self.duck_method_called)
        # Non-static 'exists'
        simplefile.exists()
        self.assertFalse(self.duck_method_called)

        def myExists(obj):
            self.duck_method_called = True
            return False
        simplefile.exists = MethodTypeCompat(myExists, simplefile)

        # Static 'exists' was overridden by the monkey patch, which accepts 0 arguments
        self.assertRaises(TypeError, simplefile.exists, 'sbrubbles')
        # Monkey patched exists
        simplefile.exists()
        self.assert_(self.duck_method_called)

        simplefile.exists = None

    def testMonkeyPatchOnMethodWithStaticAndNonStaticOverloadsWithInheritance(self):
        '''Injects new 'exists' on an object that inherits from SimpleFile and makes C++ call it.'''
        monkey = Monkey('foobar')

        # Static 'exists'
        monkey.exists('sbrubbles')
        self.assertFalse(self.duck_method_called)
        # Non-static 'exists'
        monkey.exists()
        self.assertFalse(self.duck_method_called)

        def myExists(obj):
            self.duck_method_called = True
            return False
        monkey.exists = MethodTypeCompat(myExists, monkey)

        # Static 'exists' was overridden by the monkey patch, which accepts 0 arguments
        self.assertRaises(TypeError, monkey.exists, 'sbrubbles')
        # Monkey patched exists
        monkey.exists()
        self.assert_(self.duck_method_called)

        monkey.exists = None

    def testForInfiniteRecursion(self):
        def myVirtualMethod0(obj, pt, val, cpx, b):
            self.call_counter += 1
            return VirtualMethods.virtualMethod0(obj, pt, val, cpx, b)
        vm = VirtualMethods()
        vm.virtualMethod0 = MethodTypeCompat(myVirtualMethod0, vm)
        pt, val, cpx, b = Point(1.1, 2.2), 4, complex(3.3, 4.4), True
        vm.virtualMethod0(pt, val, cpx, b)
        self.assertEqual(self.call_counter, 1)


if __name__ == '__main__':
    unittest.main()

