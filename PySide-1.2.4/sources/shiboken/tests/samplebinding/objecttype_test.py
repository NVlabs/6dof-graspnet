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

'''Tests ObjectType class of object-type with privates copy constructor and = operator.'''

import unittest
import sys

from sample import ObjectType, Str
import shiboken


class ObjectTypeTest(unittest.TestCase):
    '''Test cases  ObjectType class of object-type with privates copy constructor and = operator.'''

    def testObjectTypeSetObjectNameWithStrVariable(self):
        '''ObjectType.setObjectName with Str variable as argument.'''
        s = Str('object name')
        o = ObjectType()
        o.setObjectName(s)
        self.assertEqual(str(o.objectName()), str(s))

    def testObjectTypeSetObjectNameWithStrInstantiation(self):
        '''ObjectType.setObjectName with Str instantiation as argument.'''
        s = 'object name'
        o = ObjectType()
        o.setObjectName(Str(s))
        self.assertEqual(str(o.objectName()), s)

    def testObjectTypeSetObjectNameWithPythonString(self):
        '''ObjectType.setObjectName with Python string as argument.'''
        o = ObjectType()
        o.setObjectName('object name')
        self.assertEqual(str(o.objectName()), 'object name')

    def testNullOverload(self):
        o = ObjectType()
        o.setObject(None)
        self.assertEqual(o.callId(), 0)
        o.setNullObject(None)
        self.assertEqual(o.callId(), 1)

    def testParentFromCpp(self):
        o = ObjectType()
        self.assertEqual(sys.getrefcount(o), 2)
        o.getCppParent().setObjectName('parent')
        self.assertEqual(sys.getrefcount(o), 3)
        o.getCppParent().setObjectName('parent')
        self.assertEqual(sys.getrefcount(o), 3)
        o.getCppParent().setObjectName('parent')
        self.assertEqual(sys.getrefcount(o), 3)
        o.getCppParent().setObjectName('parent')
        self.assertEqual(sys.getrefcount(o), 3)
        o.getCppParent().setObjectName('parent')
        self.assertEqual(sys.getrefcount(o), 3)
        o.destroyCppParent()
        self.assertEqual(sys.getrefcount(o), 2)

    def testNextInFocusChainCycle(self):
        parent = ObjectType()
        child = ObjectType(parent)
        next_focus = child.nextInFocusChain()

        shiboken.invalidate(parent)

    def testNextInFocusChainCycleList(self):
        '''As above but in for a list of objects'''
        parents = []
        children = []
        focus_chains = []
        for i in range(10):
            parent = ObjectType()
            child = ObjectType(parent)
            next_focus = child.nextInFocusChain()
            parents.append(parent)
            children.append(child)
            focus_chains.append(next_focus)

        shiboken.invalidate(parents)

    def testClassDecref(self):
        # Bug was that class PyTypeObject wasn't decrefed when instance died
        before = sys.getrefcount(ObjectType)

        for i in range(1000):
            obj = ObjectType()
            shiboken.delete(obj)

        after = sys.getrefcount(ObjectType)

        self.assertLess(abs(before - after), 5)

if __name__ == '__main__':
    unittest.main()
