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

'''Tests for invalidating a C++ created child that was already on the care of a parent.'''

import unittest

from sample import ObjectType, BlackBox


class InvalidateChildTest(unittest.TestCase):
    '''Tests for invalidating a C++ created child that was already on the care of a parent.'''

    def testInvalidateChild(self):
        '''Invalidating method call should remove child from the care of a parent if it has one.'''
        parent = ObjectType()
        child1 = ObjectType(parent)
        child1.setObjectName('child1')
        child2 = ObjectType.create()
        child2.setParent(parent)
        child2.setObjectName('child2')

        self.assertEqual(parent.children(), [child1, child2])

        bbox = BlackBox()

        # This method steals ownership from Python to C++.
        bbox.keepObjectType(child1)
        self.assertEqual(parent.children(), [child2])

        bbox.keepObjectType(child2)
        self.assertEqual(parent.children(), [])

        del parent

        self.assertEqual(child1.objectName(), 'child1')
        self.assertRaises(RuntimeError, child2.objectName)

if __name__ == '__main__':
    unittest.main()

