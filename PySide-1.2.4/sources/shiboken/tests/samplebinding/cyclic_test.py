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
from sample import ObjectView
from sample import ObjectModel



class ObjTest(unittest.TestCase):

    def test_cyclic_dependency_withParent(self):
        """Create 2 objects with a cyclic dependency, so that they can
        only be removed by the garbage collector, and then invoke the
        garbage collector in a different thread.
        """
        import gc

        class CyclicChildObject(ObjectType):
            def __init__(self, parent):
                super(CyclicChildObject, self).__init__(parent)
                self._parent = parent

        class CyclicObject(ObjectType):
            def __init__(self):
                super(CyclicObject, self).__init__()
                CyclicChildObject(self)

        # turn off automatic garbage collection, to be able to trigger it
        # at the 'right' time
        gc.disable()
        alive = lambda :sum(isinstance(o, CyclicObject) for o in gc.get_objects() )

        #
        # first proof that the wizard is only destructed by the garbage
        # collector
        #
        cycle = CyclicObject()
        self.assertTrue(alive())
        del cycle
        self.assertTrue(alive())
        gc.collect()
        self.assertFalse(alive())

    def test_cyclic_dependency_withKeepRef(self):
        """Create 2 objects with a cyclic dependency, so that they can
        only be removed by the garbage collector, and then invoke the
        garbage collector in a different thread.
        """
        import gc

        class CyclicChildObject(ObjectView):
            def __init__(self, model):
                super(CyclicChildObject, self).__init__(None)
                self.setModel(model)

        class CyclicObject(ObjectModel):
            def __init__(self):
                super(CyclicObject, self).__init__()
                self._view = CyclicChildObject(self)

        # turn off automatic garbage collection, to be able to trigger it
        # at the 'right' time
        gc.disable()
        alive = lambda :sum(isinstance(o, CyclicObject) for o in gc.get_objects() )

        #
        # first proof that the wizard is only destructed by the garbage
        # collector
        #
        cycle = CyclicObject()
        self.assertTrue(alive())
        del cycle
        self.assertTrue(alive())
        gc.collect()
        self.assertFalse(alive())

if __name__ == '__main__':
    unittest.main()

