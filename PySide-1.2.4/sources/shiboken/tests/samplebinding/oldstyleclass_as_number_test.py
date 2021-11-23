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
import sample
from py3kcompat import IS_PY3K

class OldStyle:
    pass

class NewStyle(object):
    pass

class OldStyleNumber:
    def __init__(self, value):
        self.value = value
    def __trunc__(self):
        return self.value

class NewStyleNumber(object):
    def __init__(self, value):
        self.value = value
    def __int__(self):
        return int(self.value)
    def __trunc__(self):
        return self.value

class TestOldStyleClassAsNumber(unittest.TestCase):

    def testBasic(self):
        '''For the sake of calibration...'''
        self.assertEqual(sample.acceptInt(123), 123)

    def testOldStyleClassPassedAsInteger(self):
        '''Old-style classes aren't numbers and shouldn't be accepted.'''
        obj = OldStyle()
        self.assertRaises(TypeError, sample.acceptInt, obj)

    def testNewStyleClassPassedAsInteger(self):
        '''New-style classes aren't numbers and shouldn't be accepted.'''
        obj = NewStyle()
        self.assertRaises(TypeError, sample.acceptInt, obj)

    def testOldStyleClassWithNumberProtocol(self):
        obj = OldStyleNumber(123)
        self.assertEqual(sample.acceptInt(obj), obj.value)

    def testNewStyleClassWithNumberProtocol(self):
        obj = NewStyleNumber(123)
        self.assertEqual(sample.acceptInt(obj), obj.value)

if __name__ == "__main__" and not IS_PY3K:
    unittest.main()

