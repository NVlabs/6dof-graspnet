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

'''Test weakref support'''

import weakref
import unittest

from sample import ObjectType, PrivateDtor


class WeakrefBasicTest(unittest.TestCase):
    '''Simple test case of using a weakref'''

    def setUp(self):
        self.called = False

    def cb(self, *args):
        self.called = True

    def testBasic(self):
        '''ObjectType weakref'''
        obj = ObjectType()
        ref = weakref.ref(obj, self.cb)
        del obj
        self.assert_(self.called)

    def testPrivateDtor(self):
        '''PrivateDtor weakref'''
        obj = PrivateDtor.instance()
        ref = weakref.ref(obj, self.cb)
        del obj
        self.assert_(self.called)


if __name__ == '__main__':
    unittest.main()
