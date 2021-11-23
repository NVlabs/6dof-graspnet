import unittest
from sample import *
from other import *
import shiboken

class TestHashFuncs (unittest.TestCase):

    def testIt(self):
        obj1 = HandleHolder()
        obj2 = HandleHolder()

        hash1 = hash(obj1)
        hash2 = hash(obj2)
        self.assertNotEqual(hash1, hash2)

        # Now invalidate the object and test its hash.  It shouldn't segfault.
        shiboken.invalidate(obj1)

        hash1_2 = hash(obj1)
        self.assertEqual(hash1_2, hash1)



if __name__ == '__main__':
    unittest.main()
