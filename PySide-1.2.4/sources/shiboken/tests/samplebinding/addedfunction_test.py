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

'''Test cases for added functions.'''

import unittest
from sample import SampleNamespace, ObjectType, Point

class TestAddedFunctionsWithSimilarTypes(unittest.TestCase):
    '''Adds new signatures very similar to already existing ones.'''

    def testValueTypeReferenceAndValue(self):
        '''In C++ we have "function(const ValueType&, double)",
        in Python we add "function(ValueType)".'''
        point = Point(10, 20)
        multiplier = 4.0
        control = (point.x() + point.y()) * multiplier
        self.assertEqual(SampleNamespace.passReferenceToValueType(point, multiplier), control)
        control = point.x() + point.y()
        self.assertEqual(SampleNamespace.passReferenceToValueType(point), control)

    def testObjectTypeReferenceAndPointer(self):
        '''In C++ we have "function(const ObjectType&, int)",
        in Python we add "function(ValueType)".'''
        obj = ObjectType()
        obj.setObjectName('sbrubbles')
        multiplier = 3.0
        control = len(obj.objectName()) * multiplier
        self.assertEqual(SampleNamespace.passReferenceToObjectType(obj, multiplier), control)
        control = len(obj.objectName())
        self.assertEqual(SampleNamespace.passReferenceToObjectType(obj), control)

if __name__ == '__main__':
    unittest.main()
