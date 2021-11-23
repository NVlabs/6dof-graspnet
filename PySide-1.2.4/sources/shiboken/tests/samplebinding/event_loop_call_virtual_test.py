#!/usr/bin/env python


'''Simple event loop dispatcher test.'''

import time
import unittest
from random import random

from sample import ObjectType, Event


class NoOverride(ObjectType):

    pass


class Override(ObjectType):

    def __init__(self):
        ObjectType.__init__(self)
        self.called = False

    def event(self, event):
        self.called = True
        return True


class TestEventLoop(unittest.TestCase):

    def testEventLoop(self):
        '''Calling virtuals in a event loop'''
        objs = [ObjectType(), NoOverride(), Override()]

        evaluated = ObjectType.processEvent(objs,
                                        Event(Event.BASIC_EVENT))

        self.assertEqual(evaluated, 3)
        self.assert_(objs[2].called)


if __name__ == '__main__':
    unittest.main()
