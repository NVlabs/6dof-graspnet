import os
import unittest
from PySide.QtCore import QFile
import example_rc

class TestRccSimple(unittest.TestCase):

    def setUp(self):
        self.testdir = os.path.dirname(__file__)

    def testSimple(self):
        handle = QFile(":words.txt")
        handle.open(QFile.ReadOnly)
        original = open(os.path.join(self.testdir, "words.txt"), "r")
        self.assertEqual(handle.readLine(), original.readline())

    def testAlias(self):
        handle = QFile(":jack")
        handle.open(QFile.ReadOnly)
        original = open(os.path.join(self.testdir, "shining.txt"), "r")
        self.assertEqual(handle.readLine(), original.readline())

    def testHuge(self):
        handle = QFile(":manychars.txt")
        handle.open(QFile.ReadOnly)
        original = open(os.path.join(self.testdir, "manychars.txt"), "r")
        self.assertEqual(handle.readLine(), original.readline())


if __name__ == '__main__':
    unittest.main()
