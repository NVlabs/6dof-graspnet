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

'''Test case for objects that keep references to other object without owning them (e.g. model/view relationships).'''

import unittest
from sys import getrefcount

from sample import ObjectModel, ObjectView

class TestKeepReference(unittest.TestCase):
    '''Test case for objects that keep references to other object without owning them (e.g. model/view relationships).'''

    def testReferenceCounting(self):
        '''Tests reference count of model-like object referred by view-like objects.'''
        model1 = ObjectModel()
        refcount1 = getrefcount(model1)
        view1 = ObjectView()
        view1.setModel(model1)
        self.assertEqual(getrefcount(view1.model()), refcount1 + 1)

        view2 = ObjectView()
        view2.setModel(model1)
        self.assertEqual(getrefcount(view2.model()), refcount1 + 2)

        model2 = ObjectModel()
        view2.setModel(model2)
        self.assertEqual(getrefcount(view1.model()), refcount1 + 1)

    def testReferenceCountingWhenDeletingReferrer(self):
        '''Tests reference count of model-like object referred by deceased view-like object.'''
        model = ObjectModel()
        refcount1 = getrefcount(model)
        view = ObjectView()
        view.setModel(model)
        self.assertEqual(getrefcount(view.model()), refcount1 + 1)

        del view
        self.assertEqual(getrefcount(model), refcount1)

    def testReferreedObjectSurvivalAfterContextEnd(self):
        '''Model-like object assigned to a view-like object must survive after get out of context.'''
        def createModelAndSetToView(view):
            model = ObjectModel()
            model.setObjectName('created model')
            view.setModel(model)
        view = ObjectView()
        createModelAndSetToView(view)
        model = view.model()

if __name__ == '__main__':
    unittest.main()

