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

from sample import SizeF

class NumericalTypedefTest(unittest.TestCase):

    def testNumericalTypedefExact(self):
        width, height = (1.1, 2.2)
        size = SizeF(width, height)
        self.assertEqual(size.width(), width)
        self.assertEqual(size.height(), height)

    def testNumericalTypedefConvertible(self):
        width, height = (1, 2)
        size = SizeF(width, height)
        self.assertEqual(size.width(), float(width))
        self.assertEqual(size.height(), float(height))

    def testNumericalTypedefOfUnsignedShort(self):
        self.assertEqual(SizeF.passTypedefOfUnsignedShort(123), 123)
        self.assertEqual(SizeF.passTypedefOfUnsignedShort(321), 321)
        self.assertNotEqual(SizeF.passTypedefOfUnsignedShort(123), 0)

if __name__ == '__main__':
    unittest.main()
