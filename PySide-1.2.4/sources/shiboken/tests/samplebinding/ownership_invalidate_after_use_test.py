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

'''Ownership tests for cases of invalidation of Python wrapper after use.'''

import sys
import unittest

from sample import ObjectType, ObjectTypeDerived, Event


class ExtObjectType(ObjectType):
    def __init__(self):
        ObjectType.__init__(self)
        self.type_of_last_event = None
        self.last_event = None
    def event(self, event):
        self.last_event = event
        self.type_of_last_event = event.eventType()
        return True

class MyObjectType (ObjectType):
    def __init__(self):
        super(MyObjectType, self).__init__()
        self.fail = False

    def event(self, ev):
        self.callInvalidateEvent(ev)
        try:
            ev.eventType()
        except:
            self.fail = True
            raise
        return True

    def invalidateEvent(self, ev):
        pass

class ExtObjectTypeDerived(ObjectTypeDerived):
    def __init__(self):
        ObjectTypeDerived.__init__(self)
        self.type_of_last_event = None
        self.last_event = None
    def event(self, event):
        self.last_event = event
        self.type_of_last_event = event.eventType()
        return True

class OwnershipInvalidateAfterUseTest(unittest.TestCase):
    '''Ownership tests for cases of invalidation of Python wrapper after use.'''

    def testInvalidateAfterUse(self):
        '''In ObjectType.event(Event*) the wrapper object created for Event must me marked as invalid after the method is called.'''
        eot = ExtObjectType()
        eot.causeEvent(Event.SOME_EVENT)
        self.assertEqual(eot.type_of_last_event, Event.SOME_EVENT)
        self.assertRaises(RuntimeError, eot.last_event.eventType)

    def testObjectInvalidatedAfterUseAsParameter(self):
        '''Tries to use wrapper invalidated after use as a parameter to another method.'''
        eot = ExtObjectType()
        ot = ObjectType()
        eot.causeEvent(Event.ANY_EVENT)
        self.assertEqual(eot.type_of_last_event, Event.ANY_EVENT)
        self.assertRaises(RuntimeError, ot.event, eot.last_event)

    def testit(self):
        obj = MyObjectType()
        obj.causeEvent(Event.BASIC_EVENT)
        self.assertFalse(obj.fail)

    def testInvalidateAfterUseInDerived(self):
        '''Invalidate was failing in a derived C++ class that also inherited
        other base classes'''
        eot = ExtObjectTypeDerived()
        eot.causeEvent(Event.SOME_EVENT)
        self.assertEqual(eot.type_of_last_event, Event.SOME_EVENT)
        self.assertRaises(RuntimeError, eot.last_event.eventType)

if __name__ == '__main__':
    unittest.main()

