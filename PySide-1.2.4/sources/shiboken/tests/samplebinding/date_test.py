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

'''Test cases for python conversions types '''

import sys
import unittest
from datetime import date

from sample import SbkDate

class DateConversionTest(unittest.TestCase):

    def testConstructorWithDateObject(self):
        pyDate = date(2010, 12, 12)
        cDate = SbkDate(pyDate)
        self.assert_(cDate.day(), pyDate.day)
        self.assert_(cDate.month(), pyDate.month)
        self.assert_(cDate.year(), pyDate.year)

    def testToPythonFunction(self):
        cDate = SbkDate(2010, 12, 12)
        pyDate = cDate.toPython()
        self.assert_(cDate.day(), pyDate.day)
        self.assert_(cDate.month(), pyDate.month)
        self.assert_(cDate.year(), pyDate.year)

if __name__ == '__main__':
    unittest.main()

