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

'''Test cases for std::map container conversions'''

import unittest
from sample import *
from py3kcompat import IS_PY3K

if IS_PY3K:
    TYPE_STR = "class"
else:
    TYPE_STR = "type"

class TestEnumUnderNamespace(unittest.TestCase):
    def testInvisibleNamespace(self):
        o1 = EnumOnNamespace.Option1
        self.assertEquals(o1, 1)

class TestClassesUnderNamespace(unittest.TestCase):
    def testIt(self):
        c1 = SampleNamespace.SomeClass()
        e1 = SampleNamespace.SomeClass.ProtectedEnum()
        c2 = SampleNamespace.SomeClass.SomeInnerClass()
        e2 = SampleNamespace.SomeClass.SomeInnerClass.ProtectedEnum()
        c3 = SampleNamespace.SomeClass.SomeInnerClass.OkThisIsRecursiveEnough()
        e3 = SampleNamespace.SomeClass.SomeInnerClass.OkThisIsRecursiveEnough.NiceEnum()

    def testFunctionAddedOnNamespace(self):
        res = SampleNamespace.ImInsideANamespace(2, 2)
        self.assertEquals(res, 4)

    def testTpNames(self):
        self.assertEquals(str(SampleNamespace.SomeClass), "<%s 'sample.SampleNamespace.SomeClass'>"%TYPE_STR)
        self.assertEquals(str(SampleNamespace.SomeClass.ProtectedEnum), "<%s 'sample.SampleNamespace.SomeClass.ProtectedEnum'>"%TYPE_STR)
        self.assertEquals(str(SampleNamespace.SomeClass.SomeInnerClass.ProtectedEnum), "<%s 'sample.SampleNamespace.SomeClass.SomeInnerClass.ProtectedEnum'>"%TYPE_STR)
        self.assertEquals(str(SampleNamespace.SomeClass.SomeInnerClass.OkThisIsRecursiveEnough), "<%s 'sample.SampleNamespace.SomeClass.SomeInnerClass.OkThisIsRecursiveEnough'>"%TYPE_STR)
        self.assertEquals(str(SampleNamespace.SomeClass.SomeInnerClass.OkThisIsRecursiveEnough.NiceEnum), "<%s 'sample.SampleNamespace.SomeClass.SomeInnerClass.OkThisIsRecursiveEnough.NiceEnum'>"%TYPE_STR)

if __name__ == '__main__':
    unittest.main()
