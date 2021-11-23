import sys

try:
    from distutils import sysconfig
    if bool(sysconfig.get_config_var('Py_DEBUG')):
        sys.exit(0)
    import numpy
except:
    sys.exit(0)

import unittest
from sample import PointF

class TestNumpyTypes(unittest.TestCase):

    def testNumpyConverted(self):
        x, y = (0.1, 0.2)
        p = PointF(float(numpy.float32(x)), float(numpy.float32(y)))
        self.assertAlmostEqual(p.x(), x)
        self.assertAlmostEqual(p.y(), y)

    def testNumpyAsIs(self):
        x, y = (0.1, 0.2)
        p = PointF(numpy.float32(x), numpy.float32(y))
        self.assertAlmostEqual(p.x(), x)
        self.assertAlmostEqual(p.y(), y)

if __name__ == "__main__":
    unittest.main()

