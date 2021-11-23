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
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

''' Test case for a class that holds a unknown handle object.
    Test case for BUG #1105.
'''

import unittest

from sample import HandleHolder

class HandleHolderTest(unittest.TestCase):
    def testCreation(self):
        holder = HandleHolder(HandleHolder.createHandle())
        holder2 = HandleHolder(HandleHolder.createHandle())
        self.assertEqual(holder.compare(holder2), False)

    def testTransfer(self):
        holder = HandleHolder()
        holder2 = HandleHolder(holder.handle())
        self.assertTrue(holder.compare(holder2))

    def testUseDefinedType(self):
        holder = HandleHolder(8)
        holder2 = HandleHolder(holder.handle2())
        self.assertTrue(holder.compare2(holder2))

if __name__ == '__main__':
    unittest.main()
