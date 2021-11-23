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

'''Test case for a function that could receive a NULL pointer in a '[const] char*' parameter.'''

import unittest

from sample import countCharacters

class ReceiveNullCStringTest(unittest.TestCase):
    '''Test case for a function that could receive a NULL pointer in a '[const] char*' parameter.'''

    def testBasic(self):
        '''The test function should be working for the basic cases.'''
        a = ''
        b = 'abc'
        self.assertEqual(countCharacters(a), len(a))
        self.assertEqual(countCharacters(b), len(b))

    def testReceiveNull(self):
        '''The test function returns '-1' when receives a None value instead of a string.'''
        self.assertEqual(countCharacters(None), -1)

if __name__ == '__main__':
    unittest.main()

