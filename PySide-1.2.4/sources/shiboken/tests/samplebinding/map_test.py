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

'''Test cases for std::map container conversions'''

import unittest

from sample import MapUser

class ExtendedMapUser(MapUser):
    def __init__(self):
        MapUser.__init__(self)
        self.create_map_called = False

    def createMap(self):
        self.create_map_called = True
        return {'two' : (complex(2.2, 2.2), 2),
                'three' : (complex(3.3, 3.3), 3),
                'five' : (complex(5.5, 5.5), 5),
                'seven' : (complex(7.7, 7.7), 7)}

class MapConversionTest(unittest.TestCase):
    '''Test case for std::map container conversions'''

    def testReimplementedVirtualMethodCall(self):
        '''Test if a Python override of a virtual method is correctly called from C++.'''
        mu = ExtendedMapUser()
        map_ = mu.callCreateMap()
        self.assert_(mu.create_map_called)
        self.assertEqual(type(map_), dict)
        for key, value in map_.items():
            self.assertEqual(type(key), str)
            self.assertEqual(type(value[0]), complex)
            self.assertEqual(type(value[1]), int)

    def testConversionInBothDirections(self):
        '''Test converting a map from Python to C++ and back again.'''
        mu = MapUser()
        map_ = {'odds' : [2, 4, 6], 'evens' : [3, 5, 7], 'primes' : [3, 4, 6]}
        mu.setMap(map_)
        result = mu.getMap()
        self.assertEqual(result, map_)

    def testConversionMapIntKeyValueTypeValue(self):
        '''C++ signature: MapUser::passMapIntValueType(const std::map<int, const ByteArray>&)'''
        mu = MapUser()
        map_ = {0 : 'string'}
        result = mu.passMapIntValueType(map_)
        self.assertEqual(map_, result)

if __name__ == '__main__':
    unittest.main()
