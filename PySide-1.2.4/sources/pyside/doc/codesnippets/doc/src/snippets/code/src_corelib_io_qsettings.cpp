//! [0]
settings = QSettings("MySoft", "Star Runner")
color = QColor(settings.value("DataPump/bgcolor"))
//! [0]


//! [1]
settings = QSettings("MySoft", "Star Runner")
color = palette().background().color()
settings.setValue("DataPump/bgcolor", color)
//! [1]


//! [2]
settings = QSettings("/home/petra/misc/myapp.ini",
                     QSettings.IniFormat)
//! [2]


//! [3]
settings = QSettings("/Users/petra/misc/myapp.plist",
                     QSettings.NativeFormat)
//! [3]


//! [4]
settings = QSettings("HKEY_CURRENT_USER\\Software\\Microsoft\\Office",
                     QSettings.NativeFormat)
//! [4]


//! [5]
settings.setValue("11.0/Outlook/Security/DontTrustInstalledFiles", 0)
//! [5]


//! [6]
settings.setValue("HKEY_CURRENT_USER\\MySoft\\Star Runner\\Galaxy", "Milkyway")
settings.setValue("HKEY_CURRENT_USER\\MySoft\\Star Runner\\Galaxy\\Sun", "OurStar")
settings.value("HKEY_CURRENT_USER\\MySoft\\Star Runner\\Galaxy\\Default")   # returns "Milkyway"
//! [6]


//! [7]
#ifdef Q_WS_MAC
    settings = QSettings("grenoullelogique.fr", "Squash")
#else
    settings = QSettings("Grenoulle Logique", "Squash")
#endif
//! [7]


//! [8]
pos = @Point(100 100)
//! [8]


//! [9]
windir = C:\Windows
//! [9]


//! [10]
settings = QSettings("Moose Tech", "Facturo-Pro")
//! [10]


//! [11]
settings = QSettings("Moose Soft", "Facturo-Pro")
//! [11]


//! [12]
QCoreApplication.setOrganizationName("Moose Soft")
QCoreApplication.setApplicationName("Facturo-Pro")
settings = QSettings()
//! [12]


//! [13]
settings.beginGroup("mainwindow")
settings.setValue("size", win.size())
settings.setValue("fullScreen", win.isFullScreen())
settings.endGroup()

settings.beginGroup("outputpanel")
settings.setValue("visible", panel.isVisible())
settings.endGroup()
//! [13]


//! [14]
settings.beginGroup("alpha")
# settings.group() == "alpha"

settings.beginGroup("beta")
# settings.group() == "alpha/beta"

settings.endGroup()
# settings.group() == "alpha"

settings.endGroup()
# settings.group() == ""
//! [14]


//! [15]
class Login:
    userName = ''
    password = ''

    logins = []
    ...

    settings = QSettings()
    size = settings.beginReadArray("logins")
    for i in range(size):
        settings.setArrayIndex(i)
        login = Login()
        login.userName = settings.value("userName")
        login.password = settings.value("password")
        logins.append(login)

    settings.endArray()
//! [15]


//! [16]
class Login:
    userName = ''
    password = ''

    logins = []
    ...

    settings = QSettings()
    settings.beginWriteArray("logins")
    for i in range(logins.size()):
        settings.setArrayIndex(i)
        settings.setValue("userName", list.at(i).userName)
        settings.setValue("password", list.at(i).password)

    settings.endArray()
//! [16]


//! [17]
settings = QSettings()
settings.setValue("fridge/color", Qt.white)
settings.setValue("fridge/size", QSize(32, 96))
settings.setValue("sofa", True)
settings.setValue("tv", False)

keys = settings.allKeys();
# keys: ["fridge/color", "fridge/size", "sofa", "tv"]
//! [17]


//! [18]
settings.beginGroup("fridge")
keys = settings.allKeys()
# keys: ["color", "size"]
//! [18]


//! [19]
settings = QSettings()
settings.setValue("fridge/color", Qt.white)
settings.setValue("fridge/size", QSize(32, 96))
settings.setValue("sofa", True)
settings.setValue("tv", False)

keys = settings.childKeys()
# keys: ["sofa", "tv"]
//! [19]


//! [20]
settings.beginGroup("fridge")
keys = settings.childKeys()
# keys: ["color", "size"]
//! [20]


//! [21]
settings = QSettings()
settings.setValue("fridge/color", Qt.white)
settings.setValue("fridge/size", QSize(32, 96));
settings.setValue("sofa", True)
settings.setValue("tv", False)

groups = settings.childGroups()
# group: ["fridge"]
//! [21]


//! [22]
settings.beginGroup("fridge")
groups = settings.childGroups()
# groups: []
//! [22]


//! [23]
settings = QSettings()
settings.setValue("interval", 30)
settings.value("interval")      # returns 30

settings.setValue("interval", 6.55)
settings.value("interval")  # returns 6.55
//! [23]


//! [24]
settings = QSettings()
settings.setValue("ape")
settings.setValue("monkey", 1)
settings.setValue("monkey/sea", 2)
settings.setValue("monkey/doe", 4)

settings.remove("monkey")
keys = settings.allKeys()
# keys: ["ape"]
//! [24]


//! [25]
settings = QSettings()
settings.setValue("ape")
settings.setValue("monkey", 1)
settings.setValue("monkey/sea", 2)
settings.setValue("monkey/doe", 4)

settings.beginGroup("monkey")
settings.remove("")
settings.endGroup()

keys = settings.allKeys()
# keys: ["ape"]
//! [25]


//! [26]
settings = QSettings()
settings.setValue("animal/snake", 58)
settings.value("animal/snake", 1024)   # returns 58
settings.value("animal/zebra", 1024)   # returns 1024
settings.value("animal/zebra")         # returns 0
//! [26]


//! [27]
# @arg device QIODevice
# @arg map QSettings.SettingsMap
# @return bool
def myReadFunc(device, map):
//! [27]


//! [28]
# @arg device QIODevice
# @arg map QSettings.SettingsMap
# @return bool
def myWriteFunc(device, map)
//! [28]


//! [29]
# @arg device QIODevice
# @arg map QSettings.SettingsMap
# @return bool
def readXmlFile(device, map):
def writeXmlFile(device, map):

def main():
    XmlFormat = QSettings::registerFormat("xml", readXmlFile, writeXmlFile)
    settings = QSettings(XmlFormat, QSettings.UserSettings,
                         "MySoft", "Star Runner")
    ...
//! [29]
