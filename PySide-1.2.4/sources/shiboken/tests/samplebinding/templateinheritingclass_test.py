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
from sample import Photon

'''This tests classes that inherit from template classes,
simulating a situation found in Qt's phonon module.'''

class TemplateInheritingClassTest(unittest.TestCase):
    def testClassBasics(self):
        self.assertEqual(Photon.ValueIdentity.classType(), Photon.IdentityType)
        self.assertEqual(Photon.ValueDuplicator.classType(), Photon.DuplicatorType)

    def testInstanceBasics(self):
        value = 123
        samer = Photon.ValueIdentity(value)
        self.assertEqual(samer.multiplicator(), 1)
        doubler = Photon.ValueDuplicator(value)
        self.assertEqual(doubler.multiplicator(), 2)
        self.assertEqual(samer.value(), doubler.value())
        self.assertEqual(samer.calculate() * 2, doubler.calculate())

    def testPassToFunctionAsPointer(self):
        obj = Photon.ValueDuplicator(123)
        self.assertEqual(Photon.callCalculateForValueDuplicatorPointer(obj), obj.calculate())

    def testPassToFunctionAsReference(self):
        obj = Photon.ValueDuplicator(321)
        self.assertEqual(Photon.callCalculateForValueDuplicatorReference(obj), obj.calculate())

    def testPassToMethodAsValue(self):
        value1, value2 = 123, 321
        one = Photon.ValueIdentity(value1)
        other = Photon.ValueIdentity(value2)
        self.assertEqual(one.sumValueUsingPointer(other), value1 + value2)

    def testPassToMethodAsReference(self):
        value1, value2 = 123, 321
        one = Photon.ValueDuplicator(value1)
        other = Photon.ValueDuplicator(value2)
        self.assertEqual(one.sumValueUsingReference(other), value1 + value2)

    def testPassPointerThrough(self):
        obj1 = Photon.ValueIdentity(123)
        self.assertEqual(obj1, obj1.passPointerThrough(obj1))
        obj2 = Photon.ValueDuplicator(321)
        self.assertEqual(obj2, obj2.passPointerThrough(obj2))
        self.assertRaises(TypeError, obj1.passPointerThrough, obj2)

if __name__ == '__main__':
    unittest.main()
