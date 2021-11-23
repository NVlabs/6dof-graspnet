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

import unittest

from sample import ObjectType

class NamedArgsTest(unittest.TestCase):

    def testOneArgument(self):
        p = ObjectType()
        o = ObjectType(parent=p)
        self.assertEqual(o.parent(), p)

    def testMoreArguments(self):
        o = ObjectType()

        o.setObjectSplittedName("", prefix="pys", suffix="ide")
        self.assertEqual(o.objectName(), "pyside")

        o.setObjectSplittedName("", suffix="ide", prefix="pys")
        self.assertEqual(o.objectName(), "pyside")

        o.setObjectNameWithSize(name="pyside", size=6)
        self.assertEqual(o.objectName(), "pyside")

        o.setObjectNameWithSize(size=6, name="pyside")
        self.assertEqual(o.objectName(), "pyside")


    def testUseDefaultValues(self):
        o = ObjectType()

        o.setObjectNameWithSize(size=3)
        self.assertEqual(o.objectName(), "<un") # use name='unknown' default argument

        o.setObjectSplittedName("")
        self.assertEqual(o.objectName(), "<unknown>") # user prefix='<unk' and suffix='nown>'



if __name__ == '__main__':
    unittest.main()

