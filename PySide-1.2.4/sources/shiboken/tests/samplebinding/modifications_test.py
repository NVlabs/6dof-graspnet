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

'''Test cases for method modifications performed as described on type system. '''

import unittest

from sample import Modifications, Point, ByteArray

class ExtModifications(Modifications):
    def __init__(self):
        Modifications.__init__(self)
        self.multiplier = 3.0
        self.increment = 10.0

    def name(self):
        return 'ExtModifications'

    def differenceOfPointCoordinates(self, point):
        ok, res = Modifications.differenceOfPointCoordinates(self, point)
        return ok, res * self.multiplier + self.increment


class ModificationsTest(unittest.TestCase):
    '''Test cases for method modifications performed as described on type system. '''

    def setUp(self):
        self.mods = Modifications()

    def tearDown(self):
        del self.mods

    def testClassMembersAvailability(self):
        '''Test if Modified class really have the expected members.'''
        expected_members = set(['OverloadedModFunc', 'OverloadedNone',
                                'Overloaded_ibiP', 'Overloaded_ibib',
                                'Overloaded_ibid', 'Overloaded_ibii',
                                'calculateArea', 'doublePlus', 'increment',
                                'multiplyPointCoordsPlusValue', 'name',
                                'pointToPair', 'overloaded', 'power',
                                'timesTen'])
        self.assert_(expected_members.issubset(dir(Modifications)))

    def testRenamedMethodAvailability(self):
        '''Test if Modification class really have renamed the 'className' virtual method to 'name'.'''
        self.assert_('className' not in dir(Modifications))
        self.assert_('name' in dir(Modifications))

    def testReimplementationOfRenamedVirtualMethod(self):
        '''Test if class inheriting from Modification class have the reimplementation of renamed virtual method called.'''
        em = ExtModifications()
        self.assertEqual(self.mods.name(), 'Modifications')
        self.assertEqual(em.name(), 'ExtModifications')

    def testRegularMethodRenaming(self):
        '''Test if Modifications::cppMultiply was correctly renamed to calculateArea.'''
        self.assert_('cppMultiply' not in dir(Modifications))
        self.assert_('calculateArea' in dir(Modifications))
        self.assertEqual(self.mods.calculateArea(3, 6), 3 * 6)

    def testRegularMethodRemoval(self):
        '''Test if 'Modifications::exclusiveCppStuff' was removed from Python bindings.'''
        self.assert_('exclusiveCppStuff' not in dir(Modifications))

    def testArgumentRemoval(self):
        '''Test if second argument of Modifications::doublePlus(int, int) was removed.'''
        self.assertRaises(TypeError, self.mods.doublePlus, 3, 7)
        self.assertEqual(self.mods.doublePlus(7), 14)

    def testDefaultValueRemoval(self):
        '''Test if default value was removed from first argument of Modifications::increment(int).'''
        self.assertRaises(TypeError, self.mods.increment)
        self.assertEqual(self.mods.increment(7), 8)

    def testDefaultValueReplacement(self):
        '''Test if default values for both arguments of Modifications::power(int, int) were modified.'''
        # original default values: int power(int base = 1, int exponent = 0);
        self.assertNotEqual(self.mods.power(4), 1)
        # modified default values: int power(int base = 2, int exponent = 1);
        self.assertEqual(self.mods.power(), 2)
        self.assertEqual(self.mods.power(3), 3)
        self.assertEqual(self.mods.power(5, 3), 5**3)

    def testSetNewDefaultValue(self):
        '''Test if default value was correctly set to 10 for first argument of Modifications::timesTen(int).'''
        self.assertEqual(self.mods.timesTen(7), 70)
        self.assertEqual(self.mods.timesTen(), 100)

    def testArgumentRemovalAndReturnTypeModificationWithTypesystemTemplates1(self):
        '''Test modifications to method signature and return value using type system templates (case 1).'''
        result, ok = self.mods.pointToPair(Point(2, 5))
        self.assertEqual(type(ok), bool)
        self.assertEqual(type(result), tuple)
        self.assertEqual(len(result), 2)
        self.assertEqual(type(result[0]), float)
        self.assertEqual(type(result[1]), float)
        self.assertEqual(result[0], 2.0)
        self.assertEqual(result[1], 5.0)

    def testArgumentRemovalAndReturnTypeModificationWithTypesystemTemplates2(self):
        '''Test modifications to method signature and return value using type system templates (case 2).'''
        result, ok = self.mods.multiplyPointCoordsPlusValue(Point(2, 5), 4.1)
        self.assertEqual(type(ok), bool)
        self.assertEqual(type(result), float)
        self.assertEqual(result, 14.1)

    def testOverloadedMethodModifications(self):
        '''Tests modifications to an overloaded method'''
        # overloaded(int, bool[removed], int, double)
        self.assertEqual(self.mods.overloaded(1, 2, 3.1), Modifications.Overloaded_ibid)
        # overloaded(int, bool, int[removed,default=321], int)
        self.assertEqual(self.mods.overloaded(1, True, 2), Modifications.Overloaded_ibii)
        # the others weren't modified
        self.assertEqual(self.mods.overloaded(1, True, 2, False), Modifications.Overloaded_ibib)
        self.assertEqual(self.mods.overloaded(1, False, 2, Point(3, 4)), Modifications.Overloaded_ibiP)
        self.assertRaises(TypeError, self.mods.overloaded, 1, True, Point(2, 3), Point(4, 5))
        self.assertEqual(self.mods.over(1, True, Point(2, 3), Point(4, 5)), Modifications.Overloaded_ibPP)

    def testPointArrayModification(self):
        points = (Point(1, 1), Point(2, 2))
        summedPoint = Point(1, 1) + Point(2, 2)
        self.assertEqual(self.mods.sumPointArray(points), summedPoint)

    def testTypeSystemVariableReplacementInFunctionModification(self):
        ba = ByteArray('12345')
        self.assertEqual(self.mods.getSize(ba), len(ba))
        self.assertEqual(self.mods.getSize(ba, 20), 20)

    def testNoNulPointerTag(self):
        point = Point(12, 34)
        self.assertEqual(self.mods.sumPointCoordinates(point), 12 + 34)
        self.assertRaises(TypeError, self.mods.sumPointCoordinates, None)

    def testNonConversionRuleForArgumentWithDefaultValue(self):
        status, obj = self.mods.nonConversionRuleForArgumentWithDefaultValue()
        self.assert_(status)
        self.assertEqual(obj, self.mods.getObject())
        self.assertEqual(obj.objectName(), 'MyObject')

    def testInjectCodeWithConversionVariableForUserPrimitive(self):
        self.assertTrue(Modifications.invertBoolean(False))
        self.assertFalse(Modifications.invertBoolean(True))

    def testConversionRuleForReturnType(self):
        x, y = 11, 2
        diff = float(abs(x - y))
        point = Point(x, y)

        ok, res = self.mods.differenceOfPointCoordinates(point)
        self.assertTrue(isinstance(ok, bool))
        self.assertTrue(isinstance(res, float))
        self.assertEqual(res, diff)

        ok, res = self.mods.callDifferenceOfPointCoordinates(point)
        self.assertTrue(isinstance(ok, bool))
        self.assertTrue(isinstance(res, float))
        self.assertEqual(res, diff)

        ok, res = self.mods.differenceOfPointCoordinates(None)
        self.assertTrue(isinstance(ok, bool))
        self.assertTrue(isinstance(res, float))
        self.assertEqual(res, 0.0)

        ok, res = self.mods.callDifferenceOfPointCoordinates(None)
        self.assertTrue(isinstance(ok, bool))
        self.assertTrue(isinstance(res, float))
        self.assertEqual(res, 0.0)

    def testConversionRuleForReturnTypeOnExtendedClass(self):
        x, y = 11, 2
        diff = float(abs(x - y))
        point = Point(x, y)
        em = ExtModifications()

        ok, res = em.differenceOfPointCoordinates(point)
        self.assertTrue(isinstance(ok, bool))
        self.assertTrue(isinstance(res, float))
        self.assertEqual(res, diff * em.multiplier + em.increment)

        ok, res = em.callDifferenceOfPointCoordinates(point)
        self.assertTrue(isinstance(ok, bool))
        self.assertTrue(isinstance(res, float))
        self.assertEqual(res, diff * em.multiplier + em.increment)

        ok, res = em.differenceOfPointCoordinates(None)
        self.assertTrue(isinstance(ok, bool))
        self.assertTrue(isinstance(res, float))
        self.assertEqual(res, em.increment)

        ok, res = em.callDifferenceOfPointCoordinates(None)
        self.assertTrue(isinstance(ok, bool))
        self.assertTrue(isinstance(res, float))
        self.assertEqual(res, em.increment)

if __name__ == '__main__':
    unittest.main()
