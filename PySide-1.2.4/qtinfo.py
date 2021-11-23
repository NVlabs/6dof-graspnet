import sys
import subprocess
from distutils.spawn import find_executable

class QtInfo(object):
    def __init__(self, qmake_path=None):
        if qmake_path:
            self._qmake_path = qmake_path
        else:
            self._qmake_path = find_executable("qmake")

    def getQMakePath(self):
        return self._qmake_path

    def getVersion(self):
        return self.getProperty("QT_VERSION")

    def getBinsPath(self):
        return self.getProperty("QT_INSTALL_BINS")

    def getLibsPath(self):
        return self.getProperty("QT_INSTALL_LIBS")

    def getPluginsPath(self):
        return self.getProperty("QT_INSTALL_PLUGINS")

    def getImportsPath(self):
        return self.getProperty("QT_INSTALL_IMPORTS")

    def getTranslationsPath(self):
        return self.getProperty("QT_INSTALL_TRANSLATIONS")

    def getHeadersPath(self):
        return self.getProperty("QT_INSTALL_HEADERS")

    def getProperty(self, prop_name):
        cmd = [self._qmake_path, "-query", prop_name]
        proc = subprocess.Popen(cmd, stdout = subprocess.PIPE, shell=False)
        prop = proc.communicate()[0]
        proc.wait()
        if proc.returncode != 0:
            return None
        if sys.version_info >= (3,):
            return str(prop, 'ascii').strip()
        return prop.strip()

    version = property(getVersion)
    bins_dir = property(getBinsPath)
    libs_dir = property(getLibsPath)
    plugins_dir = property(getPluginsPath)
    qmake_path = property(getQMakePath)
    imports_dir = property(getImportsPath)
    translations_dir = property(getTranslationsPath)
    headers_dir = property(getHeadersPath)
