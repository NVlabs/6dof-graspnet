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
from minimal import Val


class ExtVal(Val):
    def __init__(self, valId):
        Val.__init__(self, valId)

    def passValueType(self, val):
        return ExtVal(val.valId() + 1)

    def passValueTypePointer(self, val):
        val.setValId(val.valId() + 1)
        return val

    def passValueTypeReference(self, val):
        val.setValId(val.valId() + 1)
        return val


class ValTest(unittest.TestCase):

    def testNormalMethod(self):
        valId = 123
        val = Val(valId)
        self.assertEqual(val.valId(), valId)

    def testPassValueType(self):
        val = Val(123)
        val1 = val.passValueType(val)
        self.assertNotEqual(val, val1)
        self.assertEqual(val1.valId(), 123)
        val2 = val.callPassValueType(val)
        self.assertNotEqual(val, val2)
        self.assertEqual(val2.valId(), 123)

    def testPassValueTypePointer(self):
        val = Val(0)
        self.assertEqual(val, val.passValueTypePointer(val))
        self.assertEqual(val, val.callPassValueTypePointer(val))

    def testPassValueTypeReference(self):
        val = Val(0)
        self.assertEqual(val, val.passValueTypeReference(val))
        self.assertEqual(val, val.callPassValueTypeReference(val))

    def testPassAndReceiveEnumValue(self):
        val = Val(0)
        self.assertEqual(val.oneOrTheOtherEnumValue(Val.One), Val.Other)
        self.assertEqual(val.oneOrTheOtherEnumValue(Val.Other), Val.One)

    def testPassValueTypeFromExtendedClass(self):
        val = ExtVal(0)
        val1 = val.passValueType(val)
        self.assertNotEqual(val, val1)
        self.assertEqual(val1.valId(), val.valId() + 1)
        val2 = val.callPassValueType(val)
        self.assertNotEqual(val, val2)
        self.assertEqual(val2.valId(), val.valId() + 1)

    def testPassValueTypePointerFromExtendedClass(self):
        val = ExtVal(0)
        self.assertEqual(val.valId(), 0)
        sameVal = val.passValueTypePointer(val)
        self.assertEqual(val, sameVal)
        self.assertEqual(sameVal.valId(), 1)
        sameVal = val.callPassValueTypePointer(val)
        self.assertEqual(val, sameVal)
        self.assertEqual(sameVal.valId(), 2)

    def testPassValueTypeReferenceFromExtendedClass(self):
        val = ExtVal(0)
        self.assertEqual(val.valId(), 0)
        sameVal = val.passValueTypeReference(val)
        self.assertEqual(val, sameVal)
        self.assertEqual(sameVal.valId(), 1)
        sameVal = val.callPassValueTypeReference(val)
        self.assertEqual(val, sameVal)
        self.assertEqual(sameVal.valId(), 2)


if __name__ == '__main__':
    unittest.main()

