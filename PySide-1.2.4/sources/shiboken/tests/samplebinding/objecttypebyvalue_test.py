from sample import *
import unittest

class ObjectTypeByValueTest (unittest.TestCase):
    def testIt(self):
        factory = ObjectTypeByValue()
        obj = factory.returnSomeKindOfMe()
        # This should crash!
        obj.prop.protectedValueTypeProperty.setX(1.0)
        # just to make sure it will segfault
        obj.prop.protectedValueTypeProperty.setY(2.0)

if __name__ == "__main__":
    unittest.main()
