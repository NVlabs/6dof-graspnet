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
from sample import Point

class ExtPoint(Point): pass

class TestWritableClassDict(unittest.TestCase):
    def testSetattrOnClass(self):
        setattr(Point, 'foo', 123)
        self.assertEqual(Point.foo, 123)
        pt = Point()
        self.assertEqual(pt.foo, 123)

    def testSetattrOnInheritingClass(self):
        setattr(Point, 'bar', 321)
        self.assertEqual(Point.bar, 321)
        self.assertEqual(ExtPoint.bar, 321)
        pt = ExtPoint()
        self.assertEqual(pt.bar, 321)

if __name__ == '__main__':
    unittest.main()
