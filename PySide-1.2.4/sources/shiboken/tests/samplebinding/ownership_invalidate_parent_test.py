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

'''Tests for invalidating a parent of other objects.'''

import unittest

from sample import ObjectType, BlackBox


class InvalidateParentTest(unittest.TestCase):
    '''Tests for invalidating a parent of other objects.'''

    def testInvalidateParent(self):
        '''Invalidate parent should invalidate children'''
        parent = ObjectType.create()
        child1 = ObjectType(parent)
        child1.setObjectName("child1")
        child2 = ObjectType.create()
        child2.setObjectName("child2")
        child2.setParent(parent)
        grandchild1 = ObjectType(child1)
        grandchild1.setObjectName("grandchild1")
        grandchild2 = ObjectType.create()
        grandchild2.setObjectName("grandchild2")
        grandchild2.setParent(child2)
        bbox = BlackBox()

        bbox.keepObjectType(parent) # Should invalidate the parent

        self.assertRaises(RuntimeError, parent.objectName)
        # some children still valid they are wrapper classes
        self.assertEqual(child1.objectName(), "child1")
        self.assertRaises(RuntimeError, child2.objectName)
        self.assertEqual(grandchild1.objectName(), "grandchild1")
        self.assertRaises(RuntimeError, grandchild2.objectName)

if __name__ == '__main__':
    unittest.main()

