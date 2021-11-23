#!/usr/bin/env python

'''Tests for using Shiboken-based bindings with python threads'''

import unittest
import threading
import sample
import time

import logging

#logging.basicConfig(level=logging.DEBUG)

from random import random

class Producer(threading.Thread):
    '''Producer thread'''

    def __init__(self, bucket, max_runs, *args):
        #Constructor. Receives the bucket
        super(Producer, self).__init__(*args)
        self.runs = 0
        self.bucket = bucket
        self.max_runs = max_runs
        self.production_list = []

    def run(self):
        while self.runs < self.max_runs:
            value = int(random()*10) % 10
            self.bucket.push(value)
            self.production_list.append(value)
            logging.debug('PRODUCER - pushed %d' % value)
            self.runs += 1
            #self.msleep(5)
            time.sleep(0.01)


class Consumer(threading.Thread):
    '''Consumer thread'''
    def __init__(self, bucket, max_runs, *args):
        #Constructor. Receives the bucket
        super(Consumer, self).__init__(*args)
        self.runs = 0
        self.bucket = bucket
        self.max_runs = max_runs
        self.consumption_list = []

    def run(self):
        while self.runs < self.max_runs:
            if not self.bucket.empty():
                value = self.bucket.pop()
                self.consumption_list.append(value)
                logging.debug('CONSUMER - got %d' % value)
                self.runs += 1
            else:
                logging.debug('CONSUMER - empty bucket')
            time.sleep(0.01)

class ProducerConsumer(unittest.TestCase):
    '''Basic test case for producer-consumer QThread'''

    def finishCb(self):
        #Quits the application
        self.app.exit(0)

    def testProdCon(self):
        #QThread producer-consumer example
        bucket = sample.Bucket()
        prod = Producer(bucket, 10)
        cons = Consumer(bucket, 10)

        prod.start()
        cons.start()

        #QObject.connect(prod, SIGNAL('finished()'), self.finishCb)
        #QObject.connect(cons, SIGNAL('finished()'), self.finishCb)

        prod.join()
        cons.join()

        self.assertEqual(prod.production_list, cons.consumption_list)





if __name__ == '__main__':
    unittest.main()
