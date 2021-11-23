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

'''Test cases for StrList class that inherits from std::list<Str>.'''

import unittest

from sample import Str, StrList

class StrListTest(unittest.TestCase):
    '''Test cases for StrList class that inherits from std::list<Str>.'''

    def testStrListCtor_NoParams(self):
        '''StrList constructor receives no parameter.'''
        sl = StrList()
        self.assertEqual(len(sl), 0)
        self.assertEqual(sl.constructorUsed(), StrList.NoParamsCtor)

    def testStrListCtor_Str(self):
        '''StrList constructor receives a Str object.'''
        s = Str('Foo')
        sl = StrList(s)
        self.assertEqual(len(sl), 1)
        self.assertEqual(sl[0], s)
        self.assertEqual(sl.constructorUsed(), StrList.StrCtor)

    def testStrListCtor_PythonString(self):
        '''StrList constructor receives a Python string.'''
        s = 'Foo'
        sl = StrList(s)
        self.assertEqual(len(sl), 1)
        self.assertEqual(sl[0], s)
        self.assertEqual(sl.constructorUsed(), StrList.StrCtor)

    def testStrListCtor_StrList(self):
        '''StrList constructor receives a StrList object.'''
        sl1 = StrList(Str('Foo'))
        sl2 = StrList(sl1)
        #self.assertEqual(len(sl1), len(sl2))
        #self.assertEqual(sl1, sl2)
        self.assertEqual(sl2.constructorUsed(), StrList.CopyCtor)

    def testStrListCtor_ListOfStrs(self):
        '''StrList constructor receives a Python list of Str objects.'''
        strs = [Str('Foo'), Str('Bar')]
        sl = StrList(strs)
        self.assertEqual(len(sl), len(strs))
        self.assertEqual(sl, strs)
        self.assertEqual(sl.constructorUsed(), StrList.ListOfStrCtor)

    def testStrListCtor_MixedListOfStrsAndPythonStrings(self):
        '''StrList constructor receives a Python list of mixed Str objects and Python strings.'''
        strs = [Str('Foo'), 'Bar']
        sl = StrList(strs)
        self.assertEqual(len(sl), len(strs))
        self.assertEqual(sl, strs)
        self.assertEqual(sl.constructorUsed(), StrList.ListOfStrCtor)

    def testCompareStrListWithTupleOfStrs(self):
        '''Compares StrList with a Python tuple of Str objects.'''
        sl = StrList()
        sl.append(Str('Foo'))
        sl.append(Str('Bar'))
        self.assertEqual(len(sl), 2)
        self.assertEqual(sl, (Str('Foo'), Str('Bar')))

    def testCompareStrListWithTupleOfPythonStrings(self):
        '''Compares StrList with a Python tuple of Python strings.'''
        sl = StrList()
        sl.append(Str('Foo'))
        sl.append(Str('Bar'))
        self.assertEqual(len(sl), 2)
        self.assertEqual(sl, ('Foo', 'Bar'))

    def testCompareStrListWithTupleOfStrAndPythonString(self):
        '''Compares StrList with a Python tuple of mixed Str objects and Python strings.'''
        sl = StrList()
        sl.append(Str('Foo'))
        sl.append(Str('Bar'))
        self.assertEqual(len(sl), 2)
        self.assertEqual(sl, (Str('Foo'), 'Bar'))

if __name__ == '__main__':
    unittest.main()
