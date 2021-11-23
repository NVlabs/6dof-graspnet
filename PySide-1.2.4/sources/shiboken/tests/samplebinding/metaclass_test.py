from sample import *
import unittest

class MetaA(type):
    pass

class A(object):
    __metaclass__ = MetaA

MetaB = type(Point)
B = Point

class MetaC(MetaA, MetaB):
    pass
class C(A, B):
    __metaclass__ = MetaC

class D(C):
    pass

class TestMetaClass(unittest.TestCase):
    def testIt(self):
        w1 = C() # works
        w1.setX(1)
        w1.setY(2)

        w2 = D() # should work!
        w2.setX(3)
        w2.setY(4)

        w3 = w1 + w2
        self.assertEqual(w3.x(), 4)
        self.assertEqual(w3.y(), 6)
