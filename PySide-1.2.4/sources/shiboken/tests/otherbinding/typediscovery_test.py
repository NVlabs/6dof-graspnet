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

'''Test cases for type discovery'''

import unittest

from sample import Abstract, Base1, Derived, MDerived1, MDerived3, SonOfMDerived1
from other import OtherMultipleDerived

class TypeDiscoveryTest(unittest.TestCase):

    def testPureVirtualsOfImpossibleTypeDiscovery(self):
        a = Derived.triggerImpossibleTypeDiscovery()
        self.assertEqual(type(a), Abstract)
        # call some pure virtual method
        a.pureVirtual()

    def testAnotherImpossibleTypeDiscovery(self):
        a = Derived.triggerAnotherImpossibleTypeDiscovery()
        self.assertEqual(type(a), Derived)

    def testMultipleInheritance(self):
        obj = OtherMultipleDerived.createObject("Base1");
        self.assertEqual(type(obj), Base1)
        obj = OtherMultipleDerived.createObject("MDerived1");
        self.assertEqual(type(obj), MDerived1)
        obj = OtherMultipleDerived.createObject("SonOfMDerived1");
        self.assertEqual(type(obj), SonOfMDerived1)
        obj = OtherMultipleDerived.createObject("MDerived3");
        self.assertEqual(type(obj), MDerived3)
        obj = OtherMultipleDerived.createObject("OtherMultipleDerived");
        self.assertEqual(type(obj), OtherMultipleDerived)

if __name__ == '__main__':
    unittest.main()
