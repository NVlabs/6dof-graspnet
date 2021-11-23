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

import os
import sys
import shutil
import unittest

from py3kcompat import IS_PY3K

if IS_PY3K:
    from imp import reload

orig_path = os.path.join(os.path.dirname(__file__))
workdir = os.getcwd()
src = os.path.join(orig_path, 'test_module_template.py')
dst = os.path.join(workdir, 'test_module.py')
shutil.copyfile(src, dst)
sys.path.append(workdir)

class TestModuleReloading(unittest.TestCase):

    def testModuleReloading(self):
        '''Test module reloading with on-the-fly modifications.'''
        import test_module
        for i in range(3):
            oldObject = test_module.obj
            self.assertTrue(oldObject is test_module.obj)
            reload(test_module)
            self.assertFalse(oldObject is test_module.obj)

if __name__ == "__main__":
    unittest.main()
