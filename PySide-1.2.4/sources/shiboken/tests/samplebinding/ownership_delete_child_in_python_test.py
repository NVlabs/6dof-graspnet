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

'''Tests for deleting a child object in python'''

import unittest
import random
import string

from sample import ObjectType
from py3kcompat import IS_PY3K

if IS_PY3K:
    string.letters = string.ascii_letters


class DeleteChildInPython(unittest.TestCase):
    '''Test case for deleting (unref) a child in python'''

    def testDeleteChild(self):
        '''Delete child in python should not invalidate child'''
        parent = ObjectType()
        child = ObjectType(parent)
        name = ''.join(random.sample(string.letters, 5))
        child.setObjectName(name)

        del child
        new_child = parent.children()[0]
        self.assertEqual(new_child.objectName(), name)

if __name__ == '__main__':
    unittest.main()
