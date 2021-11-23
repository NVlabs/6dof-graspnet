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

'''Test cases for <add-function> with const char* as argument'''

import unittest

from sample import Color, Pen, SampleNamespace

class TestPen(unittest.TestCase):
    '''Simple test case for Pen.'''

    def testPenWithEmptyConstructor(self):
        pen = Pen()
        self.assertEqual(pen.ctorType(), Pen.EmptyCtor)

    def testPenWithEnumConstructor(self):
        pen = Pen(SampleNamespace.RandomNumber)
        self.assertEqual(pen.ctorType(), Pen.EnumCtor)

    def testPenWithColorConstructor(self):
        pen = Pen(Color())
        self.assertEqual(pen.ctorType(), Pen.ColorCtor)

    def testPenWithCopyConstructor(self):
        pen = Pen(Pen())
        self.assertEqual(pen.ctorType(), Pen.CopyCtor)

    def testPenWithIntConvertedToColor(self):
        pen = Pen(1)
        self.assertEqual(pen.ctorType(), Pen.ColorCtor)

if __name__ == '__main__':
    unittest.main()

