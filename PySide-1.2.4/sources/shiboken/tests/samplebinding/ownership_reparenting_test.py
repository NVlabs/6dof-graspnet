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

'''Tests for object reparenting.'''

import unittest
import sys

from sample import ObjectType

class ExtObjectType(ObjectType):
    def __init__(self):
        ObjectType.__init__(self)


class ReparentingTest(unittest.TestCase):
    '''Tests for object reparenting.'''

    def testReparentedObjectTypeIdentity(self):
        '''Reparent children from one parent to another.'''
        object_list = []
        old_parent = ObjectType()
        new_parent = ObjectType()
        for i in range(3):
            obj = ObjectType()
            object_list.append(obj)
            obj.setParent(old_parent)
        for obj in object_list:
            obj.setParent(new_parent)
        for child in new_parent.children():
            self.assert_(child in object_list)

    def testReparentWithTheSameParent(self):
        '''Set the same parent twice to check if the ref continue the same'''
        obj = ObjectType()
        parent = ObjectType()
        self.assertEqual(sys.getrefcount(obj), 2)
        obj.setParent(parent)
        self.assertEqual(sys.getrefcount(obj), 3)
        obj.setParent(parent)
        self.assertEqual(sys.getrefcount(obj), 3)

    def testReparentedExtObjectType(self):
        '''Reparent children from one extended parent to another.'''
        object_list = []
        old_parent = ExtObjectType()
        new_parent = ExtObjectType()
        for i in range(3):
            obj = ExtObjectType()
            object_list.append(obj)
            obj.setParent(old_parent)
        for obj in object_list:
            obj.setParent(new_parent)
        for orig, child in zip(object_list, new_parent.children()):
            self.assertEqual(type(orig), type(child))

    def testReparentedObjectTypeIdentityWithParentsCreatedInCpp(self):
        '''Reparent children from one parent to another, both created in C++.'''
        object_list = []
        old_parent = ObjectType.create()
        new_parent = ObjectType.create()
        for i in range(3):
            obj = ObjectType()
            object_list.append(obj)
            obj.setParent(old_parent)
        for obj in object_list:
            obj.setParent(new_parent)
        for child in new_parent.children():
            self.assert_(child in object_list)

    def testReparentedObjectTypeIdentityWithChildrenCreatedInCpp(self):
        '''Reparent children created in C++ from one parent to another.'''
        object_list = []
        old_parent = ObjectType()
        new_parent = ObjectType()
        for i in range(3):
            obj = ObjectType.create()
            object_list.append(obj)
            obj.setParent(old_parent)
        for obj in object_list:
            obj.setParent(new_parent)
        for child in new_parent.children():
            self.assert_(child in object_list)

    def testReparentedObjectTypeIdentityWithParentsAndChildrenCreatedInCpp(self):
        '''Reparent children from one parent to another. Parents and children are created in C++.'''
        object_list = []
        old_parent = ObjectType.create()
        new_parent = ObjectType.create()
        for i in range(3):
            obj = ObjectType.create()
            object_list.append(obj)
            obj.setParent(old_parent)
        for obj in object_list:
            obj.setParent(new_parent)
        for child in new_parent.children():
            self.assert_(child in object_list)


if __name__ == '__main__':
    unittest.main()

