#!/usr/bin/env python

import time
import threading
import unittest
from random import random

from sample import ObjectType, Event


class Producer(ObjectType):

    def __init__(self):
        ObjectType.__init__(self)
        self.data = None
        self.read = False

    def event(self, event):
        self.data = random()

        while not self.read:
            time.sleep(0.01)

        return True


class Collector(threading.Thread):

    def __init__(self, objects):
        threading.Thread.__init__(self)
        self.max_runs = len(objects)
        self.objects = objects
        self.data = []

    def run(self):
        i = 0
        while i < self.max_runs:
            if self.objects[i].data is not None:
                self.data.append(self.objects[i].data)
                self.objects[i].read = True
                i += 1
            time.sleep(0.01)


class TestEventLoopWithThread(unittest.TestCase):
    '''Communication between a python thread and an simple
    event loop in C++'''

    def testBasic(self):
        '''Allowing threads and calling virtuals from C++'''
        number = 10
        objs = [Producer() for x in range(number)]
        thread = Collector(objs)

        thread.start()

        evaluated = ObjectType.processEvent(objs,
                                        Event(Event.BASIC_EVENT))

        thread.join()

        producer_data = [x.data for x in objs]
        self.assertEqual(evaluated, number)
        self.assertEqual(producer_data, thread.data)


if __name__ == '__main__':
    unittest.main()
