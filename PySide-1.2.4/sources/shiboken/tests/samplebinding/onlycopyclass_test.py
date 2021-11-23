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

import unittest
from sample import OnlyCopy, FriendOfOnlyCopy

class ClassWithOnlyCopyCtorTest(unittest.TestCase):
    def testGetOne(self):
        obj = FriendOfOnlyCopy.createOnlyCopy(123)
        self.assertEqual(type(obj), OnlyCopy)
        self.assertEqual(obj.value(), 123)

    def testGetMany(self):
        objs = FriendOfOnlyCopy.createListOfOnlyCopy(3)
        self.assertEqual(type(objs), list)
        self.assertEqual(len(objs), 3)
        for value, obj in enumerate(objs):
            self.assertEqual(obj.value(), value)

    def testPassAsValue(self):
        obj = FriendOfOnlyCopy.createOnlyCopy(123)
        self.assertEqual(obj.value(), OnlyCopy.getValue(obj))

    def testPassAsReference(self):
        obj = FriendOfOnlyCopy.createOnlyCopy(123)
        self.assertEqual(obj.value(), OnlyCopy.getValueFromReference(obj))

if __name__ == '__main__':
    unittest.main()
