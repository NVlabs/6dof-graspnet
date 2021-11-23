import shiboken
import unittest
from sample import *

class MultipleInherited (ObjectType, Point):
    def __init__(self):
        ObjectType.__init__(self)
        Point.__init__(self)

class TestShiboken(unittest.TestCase):
    def testIsValid(self):
        self.assertTrue(shiboken.isValid(object()))
        self.assertTrue(shiboken.isValid(None))

        bb = BlackBox()
        item = ObjectType()
        ticket = bb.keepObjectType(item)
        bb.disposeObjectType(ticket)
        self.assertFalse(shiboken.isValid(item))

    def testWrapInstance(self):
        addr = ObjectType.createObjectType()
        obj = shiboken.wrapInstance(addr, ObjectType)
        self.assertFalse(shiboken.createdByPython(obj))
        obj.setObjectName("obj")
        self.assertEqual(obj.objectName(), "obj")
        self.assertEqual(addr, obj.identifier())
        self.assertFalse(shiboken.createdByPython(obj))

        # avoid mem leak =]
        bb = BlackBox()
        self.assertTrue(shiboken.createdByPython(bb))
        bb.disposeObjectType(bb.keepObjectType(obj))

    def testIsOwnedByPython(self):
        obj = ObjectType()
        self.assertTrue(shiboken.ownedByPython(obj))
        p = ObjectType()
        obj.setParent(p)
        self.assertFalse(shiboken.ownedByPython(obj))

    def testDump(self):
        """Just check if dump doesn't crash on certain use cases"""
        p = ObjectType()
        obj = ObjectType(p)
        obj2 = ObjectType(obj)
        obj3 = ObjectType(obj)
        self.assertEqual(shiboken.dump(None), "Ordinary Python type.")
        shiboken.dump(obj)

        model = ObjectModel(p)
        v = ObjectView(model, p)
        shiboken.dump(v)

        m = MultipleInherited()
        shiboken.dump(m)
        self.assertEqual(len(shiboken.getCppPointer(m)), 2)

        # Don't crash even after deleting an object
        shiboken.invalidate(obj)
        shiboken.dump(obj)  # deleted
        shiboken.dump(p)    # child deleted
        shiboken.dump(obj2) # parent deleted

    def testDelete(self):
        obj = ObjectType()
        child = ObjectType(obj)
        self.assertTrue(shiboken.isValid(obj))
        self.assertTrue(shiboken.isValid(child))
        # Note: this test doesn't assure that the object dtor was really called
        shiboken.delete(obj)
        self.assertFalse(shiboken.isValid(obj))
        self.assertFalse(shiboken.isValid(child))

    def testVersionAttr(self):
        self.assertEqual(type(shiboken.__version__), str)
        self.assertTrue(len(shiboken.__version__) >= 5)
        self.assertEqual(type(shiboken.__version_info__), tuple)
        self.assertEqual(len(shiboken.__version_info__), 5)

if __name__ == '__main__':
    unittest.main()
