
//![newstuff]
    fi = QFileInfo("c:/temp/foo") => fi.absoluteFilePath() => "C:/temp/foo"
//![newstuff]

//! [0]
info1 = QFileInfo("/home/bob/bin/untabify")
info1.isSymLink()           # returns true
info1.absoluteFilePath()    # returns "/home/bob/bin/untabify"
info1.size()                # returns 56201
info1.symLinkTarget()       # returns "/opt/pretty++/bin/untabify"

info2 = QFileInfo(info1.symLinkTarget())
info1.isSymLink()           # returns False
info1.absoluteFilePath()    # returns "/opt/pretty++/bin/untabify"
info1.size()                # returns 56201

//! [0]


//! [1]
info1 = QFileInfo("C:\\Documents and Settings\\Bob\\untabify.lnk")
info1.isSymLink()           # returns True
info1.absoluteFilePath()    # returns "C:/Documents and Settings/Bob/untabify.lnk"
info1.size()                # returns 743
info1.symLinkTarget()       # returns "C:/Pretty++/untabify"

info2 = QFileInfo(info1.symLinkTarget())
info1.isSymLink()           # returns False
info1.absoluteFilePath()    # returns "C:/Pretty++/untabify"
info1.size()                # returns 63942
//! [1]


//! [2]
absolute = "/local/bin"
relative = "local/bin"
absFile = QFileInfo(absolute)
relFile = QFileInfo(relative)

QDir.setCurrent(QDir.rootPath())
# absFile and relFile now point to the same file

QDir.setCurrent("/tmp")
# absFile now points to "/local/bin",
# while relFile points to "/tmp/local/bin"
//! [2]


//! [3]
fi = QFileInfo("/tmp/archive.tar.gz")
name = fi.fileName()                    # name = "archive.tar.gz"
//! [3]


//! [4]
fi = QFileInfo("/Applications/Safari.app")
bundle = fi.bundleName()                # name = "Safari"
//! [4]


//! [5]
fi = QFileInfo("/tmp/archive.tar.gz")
base = fi.baseName()                    # base = "archive"
//! [5]


//! [6]
fi = QFileInfo("/tmp/archive.tar.gz")
base = fi.completeBaseName()            # base = "archive.tar"
//! [6]


//! [7]
fi = QFileInfo("/tmp/archive.tar.gz")
ext = fi.completeSuffix()               # ext = "tar.gz"
//! [7]


//! [8]
fi = QFileInfo("/tmp/archive.tar.gz")
ext = fi.suffix();                      # ext = "gz"
//! [8]


//! [9]
info = QFileInfo(fileName)
if info.isSymLink():
    fileName = info.symLinkTarget()
//! [9]


//! [10]
fi = QFileInfo("/tmp/archive.tar.gz")
if fi.permission(QFile.WriteUser | QFile.ReadGroup):
    print "I can change the file; my group can read the file"
if fi.permission(QFile.WriteGroup | QFile.WriteOther):
    print "The group or others can change the file"
//! [10]
