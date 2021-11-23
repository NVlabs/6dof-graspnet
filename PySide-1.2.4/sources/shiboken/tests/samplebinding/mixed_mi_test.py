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

'''Test cases for multiple inheritance in mixed Python/C++ scenarios'''

import unittest

from sample import ObjectType


class Base(object):
    '''Base Python class'''

    def __init__(self):
        self.name = ''

    def pythonName(self):
        return self.name

    def setPythonName(self, name):
        self.name = name


class Child(Base, ObjectType):
    '''Dummy class with mixed parents'''

    def __init__(self):
        Base.__init__(self)
        ObjectType.__init__(self)


class MixedInheritanceTest(unittest.TestCase):

    def testMixed(self):
        '''Mixed Python/C++ multiple inheritance'''
        obj = Child()

        obj.setObjectName('aaa')
        self.assertEqual(obj.objectName(), 'aaa')

        obj.setPythonName('python')
        self.assertEqual(obj.pythonName(), 'python')


if __name__ == '__main__':
    unittest.main()


