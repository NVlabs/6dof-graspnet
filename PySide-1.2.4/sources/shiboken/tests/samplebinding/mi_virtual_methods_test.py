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

'''Test cases for virtual methods in multiple inheritance scenarios'''

import unittest

from sample import VirtualMethods, ObjectType, Event


class ImplementsNone(ObjectType, VirtualMethods):
    '''Implements no virtual methods'''

    def __init__(self):
        ObjectType.__init__(self)
        VirtualMethods.__init__(self)


class ImplementsBoth(ObjectType, VirtualMethods):
    '''Implements ObjectType.event and VirtualMethods.sum1'''

    def __init__(self):
        ObjectType.__init__(self)
        VirtualMethods.__init__(self)
        self.event_processed = False

    def event(self, event):
        self.event_processed = True
        return True

    def sum1(self, arg0, arg1, arg2):
        return (arg0 + arg1 + arg2) * 2


class CppVirtualTest(unittest.TestCase):
    '''Virtual method defined in c++ called from C++'''

    def testCpp(self):
        '''C++ calling C++ virtual method in multiple inheritance scenario'''
        obj = ImplementsNone()
        self.assert_(ObjectType.processEvent([obj], Event(Event.BASIC_EVENT)))
        self.assertRaises(AttributeError, getattr, obj, 'event_processed')

        self.assertEqual(obj.callSum0(1, 2, 3), 6)


class PyVirtualTest(unittest.TestCase):
    '''Virtual method reimplemented in python called from C++'''

    def testEvent(self):
        '''C++ calling Python reimplementation of virtual in multiple inheritance'''
        obj = ImplementsBoth()
        self.assert_(ObjectType.processEvent([obj], Event(Event.BASIC_EVENT)))
        self.assert_(obj.event_processed)

        self.assertEqual(obj.callSum1(1, 2, 3), 12)


if __name__ == '__main__':
    unittest.main()
