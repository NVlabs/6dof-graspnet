import unittest
from sample import *

class TestLackOfDereferenceOperators (unittest.TestCase):
    def testIf(self):
        r = Reference()
        self.assertFalse(hasattr(r, "__mul__"))

if __name__ == '__main__':
    unittest.main()
