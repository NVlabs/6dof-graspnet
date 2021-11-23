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

'''Test cases for deep copy of objects'''

import copy
import unittest

try:
    import cPickle as pickle
except ImportError:
    import pickle


from sample import Point


class SimpleCopy(unittest.TestCase):
    '''Simple copy of objects'''

    def testCopy(self):
        point = Point(0.1, 2.4)
        new_point = copy.copy(point)

        self.assert_(point is not new_point)
        self.assertEqual(point, new_point)


class DeepCopy(unittest.TestCase):
    '''Deep copy with shiboken objects'''

    def testDeepCopy(self):
        '''Deep copy of value types'''
        point = Point(3.1, 4.2)
        new_point = copy.deepcopy([point])[0]

        self.assert_(point is not new_point)
        self.assertEqual(point, new_point)


class PicklingTest(unittest.TestCase):
    '''Support pickling'''

    def testSimple(self):
        '''Simple pickling and unpickling'''

        point = Point(10.2, 43.5)

        data = pickle.dumps(point)
        new_point = pickle.loads(data)

        self.assertEqual(point, new_point)
        self.assert_(point is not new_point)


if __name__ == '__main__':
    unittest.main()

