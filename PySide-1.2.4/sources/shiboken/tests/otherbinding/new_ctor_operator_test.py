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

'''Tests calling Str constructor using a Number parameter, being that number defines a cast operator to Str.'''

import unittest

from sample import Str
from other import Number

class NewCtorOperatorTest(unittest.TestCase):
    '''Tests calling Str constructor using a Number parameter, being that number defines a cast operator to Str.'''

    def testNumber(self):
        '''Basic test to see if the Number class is Ok.'''
        value = 123
        num = Number(value)
        self.assertEqual(num.value(), value)

    def testStrCtorWithNumberArgument(self):
        '''Try to build a Str from 'sample' module with a Number argument from 'other' module.'''
        value = 123
        num = Number(value)
        string = Str(num)

if __name__ == '__main__':
    unittest.main()

