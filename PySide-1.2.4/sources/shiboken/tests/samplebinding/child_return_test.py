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

'''The BlackBox class has cases of ownership transference between C++ and Python.'''

import sys
import unittest

from sample import *

class ReturnOfChildTest(unittest.TestCase):
    '''The BlackBox class has cases of ownership transference between C++ and Python.'''

    def testKillParentKeepingChild(self):
        '''Ownership transference from Python to C++ and back again.'''
        o1 = ObjectType.createWithChild()
        child = o1.children()[0]
        del o1
        self.assertRaises(RuntimeError, child.objectName)

    def testKillParentKeepingChild2(self):
        '''Ownership transference from Python to C++ and back again.'''
        o1 = ObjectType.createWithChild()
        child = o1.findChild("child")
        del o1
        self.assertRaises(RuntimeError, child.objectName)

if __name__ == '__main__':
    unittest.main()

