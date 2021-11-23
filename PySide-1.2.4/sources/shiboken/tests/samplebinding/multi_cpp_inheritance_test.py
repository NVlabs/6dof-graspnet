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

'''Test cases for multiple inheritance'''

import sys
import unittest

from sample import *

class SimpleUseCase(ObjectType, Str):
    def __init__(self, name):
        ObjectType.__init__(self)
        Str.__init__(self, name)

class SimpleUseCaseReverse(Str, ObjectType):
    def __init__(self, name):
        ObjectType.__init__(self)
        Str.__init__(self, name)

class SimpleUseCase2(SimpleUseCase):
    def __init__(self, name):
        SimpleUseCase.__init__(self, name)

class ComplexUseCase(SimpleUseCase2, Point):
    def __init__(self, name):
        SimpleUseCase2.__init__(self, name)
        Point.__init__(self)

class ComplexUseCaseReverse(Point, SimpleUseCase2):
    def __init__(self, name):
        SimpleUseCase2.__init__(self, name)
        Point.__init__(self)

class MultipleCppDerivedTest(unittest.TestCase):
    def testInstanciation(self):
        s = SimpleUseCase("Hi")
        self.assertEqual(s, "Hi")
        s.setObjectName(s)
        self.assertEqual(s.objectName(), "Hi")

    def testInstanciation2(self):
        s = SimpleUseCase2("Hi")
        self.assertEqual(s, "Hi")
        s.setObjectName(s)
        self.assertEqual(s.objectName(), "Hi")

    def testComplexInstanciation(self):
        c = ComplexUseCase("Hi")
        self.assertEqual(c, "Hi")
        c.setObjectName(c)
        self.assertEqual(c.objectName(), "Hi")
        c.setX(2);
        self.assertEqual(c.x(), 2)

class MultipleCppDerivedReverseTest(unittest.TestCase):
    def testInstanciation(self):
        s = SimpleUseCaseReverse("Hi")
        self.assertEqual(s, "Hi")
        s.setObjectName(s)
        self.assertEqual(s.objectName(), "Hi")

    def testInstanciation2(self):
        s = SimpleUseCase2("Hi")
        self.assertEqual(s, "Hi")
        s.setObjectName(s)
        self.assertEqual(s.objectName(), "Hi")

    def testComplexInstanciation(self):
        c = ComplexUseCaseReverse("Hi")
        c.setObjectName(c)
        self.assertEqual(c.objectName(), "Hi")
        c.setX(2);
        self.assertEqual(c, Point(2, 0))

if __name__ == '__main__':
    unittest.main()
