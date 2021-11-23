/############################################################################
##
## Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
## Contact: http://www.qt-project.org/legal
##
## $QT_BEGIN_LICENSE:LGPL$
## Commercial License Usage
## Licensees holding valid commercial Qt licenses may use this file in
## accordance with the commercial license agreement provided with the
## Software or, alternatively, in accordance with the terms contained in
## a written agreement between you and Digia.  For licensing terms and
## conditions see http://qt.digia.com/licensing.  For further information
## use the contact form at http://qt.digia.com/contact-us.
##
## GNU Lesser General Public License Usage
## Alternatively, this file may be used under the terms of the GNU Lesser
## General Public License version 2.1 as published by the Free Software
## Foundation and appearing in the file LICENSE.LGPL included in the
## packaging of this file.  Please review the following information to
## ensure the GNU Lesser General Public License version 2.1 requirements
## will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
##
## In addition, as a special exception, Digia gives you certain additional
## rights.  These rights are described in the Digia Qt LGPL Exception
## version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
##
## GNU General Public License Usage
## Alternatively, this file may be used under the terms of the GNU
## General Public License version 3.0 as published by the Free Software
## Foundation and appearing in the file LICENSE.GPL included in the
## packaging of this file.  Please review the following information to
## ensure the GNU General Public License version 3.0 requirements will be
## met: http://www.gnu.org/copyleft/gpl.html.
##
##
## $QT_END_LICENSE$
##
############################################################################/

from PySide.QtGui import *


DropArea.DropArea(QWidget *parent)
    : QLabel(parent)
{
    setMinimumSize(200, 200)
    setFrameStyle(QFrame.Sunken | QFrame::StyledPanel)
    setAlignment(Qt.AlignCenter)
    setAcceptDrops(True)
    setAutoFillBackground(True)
    clear()
}

void DropArea.dragEnterEvent(QDragEnterEvent *event)
{
    setText(tr("<drop content>"))
    setBackgroundRole(QPalette.Highlight)

    event.acceptProposedAction()
    emit changed(event.mimeData())
}

void DropArea.dragMoveEvent(QDragMoveEvent *event)
{
    event.acceptProposedAction()
}

void DropArea.dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event.mimeData()

    if (mimeData.hasImage()) {
        setPixmap(qvariant_cast<QPixmap>(mimeData.imageData()))
    } else if (mimeData.hasHtml()) {
        setText(mimeData.html())
        setTextFormat(Qt.RichText)
    } else if (mimeData.hasText()) {
        setText(mimeData.text())
        setTextFormat(Qt.PlainText)
    } else {
        setText(tr("Cannot display data"))
    }

    setBackgroundRole(QPalette.Dark)
    event.acceptProposedAction()
}

//![0]
def paste(self):
    clipboard = QApplication.clipboard()
    mimeData = clipboard.mimeData()

    if mimeData.hasImage():
        setPixmap(mimeData.imageData())
    elif mimeData.hasHtml():
        setText(mimeData.html())
        setTextFormat(Qt.RichText)
    elif (mimeData.hasText():
        setText(mimeData.text())
        setTextFormat(Qt.PlainText)
    else:
        setText(tr("Cannot display data"))
//![0]

    emit changed(mimeData)
    setBackgroundRole(QPalette.Dark)
    //event.acceptProposedAction()
}

void DropArea.dragLeaveEvent(QDragLeaveEvent *event)
{
    clear()
    event.accept()
}

void DropArea.clear()
{
    setText(tr("<drop content>"))
    setBackgroundRole(QPalette.Dark)

    emit changed()
}

QPixmap DropArea.extractPixmap(const QByteArray &data, const QString &format)
{
    QList<QByteArray> imageFormats = QImageReader.supportedImageFormats()
    QPixmap pixmap

    foreach (QByteArray imageFormat, imageFormats) {
        if (format.mid(6) == QString(imageFormat)) {
            pixmap.loadFromData(data, imageFormat)
            break
        }
    }
    return pixmap
}
