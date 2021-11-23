
import unittest

from PySide.QtGui import QApplication, QPixmap

import image_rc

class TestRccImage(unittest.TestCase):
    def testImage(self):
        app = QApplication([])
        image = QPixmap(":image")
        self.assertFalse(image.isNull())

if __name__ == '__main__':
    unittest.main()
