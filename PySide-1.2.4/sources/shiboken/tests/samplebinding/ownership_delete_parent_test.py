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

'''Tests for destroying the parent'''

import sys
import unittest

from sample import ObjectType


class DeleteParentTest(unittest.TestCase):
    '''Test case for deleting a parent object'''

    def testParentDestructor(self):
        '''Delete parent object should invalidate child'''
        parent = ObjectType()
        child = ObjectType()
        child.setParent(parent)

        refcount_before = sys.getrefcount(child)

        del parent
        self.assertRaises(RuntimeError, child.objectName)
        self.assertEqual(sys.getrefcount(child), refcount_before-1)

    def testParentDestructorMultipleChildren(self):
        '''Delete parent object should invalidate all children'''
        parent = ObjectType()
        children = [ObjectType() for _ in range(10)]

        for child in children:
            child.setParent(parent)

        del parent
        for i, child in enumerate(children):
            self.assertRaises(RuntimeError, child.objectName)
            self.assertEqual(sys.getrefcount(child), 4)

    def testRecursiveParentDelete(self):
        '''Delete parent should invalidate grandchildren'''
        parent = ObjectType()
        child = ObjectType(parent)
        grandchild = ObjectType(child)

        del parent
        self.assertRaises(RuntimeError, child.objectName)
        self.assertEqual(sys.getrefcount(child), 2)
        self.assertRaises(RuntimeError, grandchild.objectName)
        self.assertEqual(sys.getrefcount(grandchild), 2)


if __name__ == '__main__':
    unittest.main()
