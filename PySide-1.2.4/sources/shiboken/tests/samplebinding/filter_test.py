
import unittest

from sample import Data, Intersection, Union

class TestFilters(unittest.TestCase):

    def testAnd(self):

        f1 = Data(Data.Name, "joe")
        f2 = Union()

        inter = f1 & f2

        self.assertEqual(type(inter), Intersection)

if __name__ == '__main__':
    unittest.main()
