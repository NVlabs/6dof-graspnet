from __future__ import division
from sample import *
import unittest

class TestNewDivision(unittest.TestCase):

    def testIt(self):
        p = Point(4, 4)
        p2 = p/2
        self.assertEqual(p2, Point(2, 2))

if __name__ == "__main__":
    unittest.main()

