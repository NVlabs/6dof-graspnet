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

'''Test cases for overloads involving static and non-static versions of a method.'''

import os
import unittest

from sample import SimpleFile

class SimpleFile2 (SimpleFile):
    def exists(self):
        return "Mooo"

class SimpleFile3 (SimpleFile):
    pass

class SimpleFile4 (SimpleFile):
    exists = 5

class StaticNonStaticMethodsTest(unittest.TestCase):
    '''Test cases for overloads involving static and non-static versions of a method.'''

    def setUp(self):
        filename = 'simplefile%d.txt' % os.getpid()
        self.existing_filename = os.path.join(os.path.curdir, filename)
        self.delete_file = False
        if not os.path.exists(self.existing_filename):
            f = open(self.existing_filename, 'w')
            for line in range(10):
                f.write('sbrubbles\n')
            f.close()
            self.delete_file = True

        self.non_existing_filename = os.path.join(os.path.curdir, 'inexistingfile.txt')
        i = 0
        while os.path.exists(self.non_existing_filename):
            i += 1
            filename = 'inexistingfile-%d.txt' % i
            self.non_existing_filename = os.path.join(os.path.curdir, filename)

    def tearDown(self):
        if self.delete_file:
            os.remove(self.existing_filename)

    def testCallingStaticMethodWithClass(self):
        '''Call static method using class.'''
        self.assert_(SimpleFile.exists(self.existing_filename))
        self.assertFalse(SimpleFile.exists(self.non_existing_filename))

    def testCallingStaticMethodWithInstance(self):
        '''Call static method using instance of class.'''
        f = SimpleFile(self.non_existing_filename)
        self.assert_(f.exists(self.existing_filename))
        self.assertFalse(f.exists(self.non_existing_filename))

    def testCallingInstanceMethod(self):
        '''Call instance method.'''
        f1 = SimpleFile(self.non_existing_filename)
        self.assertFalse(f1.exists())
        f2 = SimpleFile(self.existing_filename)
        self.assert_(f2.exists())

    def testOverridingStaticNonStaticMethod(self):
        f = SimpleFile2(self.existing_filename)
        self.assertEqual(f.exists(), "Mooo")

        f = SimpleFile3(self.existing_filename)
        self.assertTrue(f.exists())

        f = SimpleFile4(self.existing_filename)
        self.assertEqual(f.exists, 5)

    def testDuckPunchingStaticNonStaticMethod(self):
        f = SimpleFile(self.existing_filename)
        f.exists = lambda : "Meee"
        self.assertEqual(f.exists(), "Meee")

if __name__ == '__main__':
    unittest.main()

