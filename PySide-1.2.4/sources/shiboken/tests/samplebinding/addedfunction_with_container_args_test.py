#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2013 Kitware, Inc.
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

'''Test cases for added functions with nested and multi-argument container types.'''

import unittest
from sample import sum2d, sumproduct

class TestAddedFunctionsWithContainerArgs(unittest.TestCase):
    '''Tests added functions with nested and multi-argument container types.'''

    def testNestedContainerType(self):
        '''Test added function with single-argument containers.'''
        values = [[1,2],[3,4,5],[6]]
        self.assertEqual(sum2d(values), 21)

    def testMultiArgContainerType(self):
        '''Test added function with a two-argument container.'''
        values = [(1,2),(3,4),(5,6)]
        self.assertEqual(sumproduct(values), 44)

if __name__ == '__main__':
    unittest.main()
