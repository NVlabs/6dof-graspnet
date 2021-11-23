/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui>

#include "xbelreader.h"

//! [0]
XbelReader::XbelReader(QTreeWidget *treeWidget)
    : treeWidget(treeWidget)
{
    QStyle *style = treeWidget->style();

    folderIcon.addPixmap(style->standardPixmap(QStyle::SP_DirClosedIcon),
                         QIcon::Normal, QIcon::Off);
    folderIcon.addPixmap(style->standardPixmap(QStyle::SP_DirOpenIcon),
                         QIcon::Normal, QIcon::On);
    bookmarkIcon.addPixmap(style->standardPixmap(QStyle::SP_FileIcon));
}
//! [0]

//! [1]
def read(self, device):
    self.setDevice(device)

    while not atEnd():
        readNext()

        if isStartElement():
            if self.name() == "xbel" and self.attributes().value("version") == "1.0":
                self.readXBEL()
            else:
                self.raiseError(QObject.tr("The file is not an XBEL version 1.0 file."));

    return not self.error();
//! [1]

//! [2]
void XbelReader::readUnknownElement()
{
    Q_ASSERT(isStartElement());

    while (!atEnd()) {
        readNext();

        if (isEndElement())
            break;

        if (isStartElement())
            readUnknownElement();
    }
}
//! [2]

//! [3]
void XbelReader::readXBEL()
{
    Q_ASSERT(isStartElement() && name() == "xbel");

    while (!atEnd()) {
        readNext();

        if (isEndElement())
            break;

        if (isStartElement()) {
            if (name() == "folder")
                readFolder(0);
            else if (name() == "bookmark")
                readBookmark(0);
            else if (name() == "separator")
                readSeparator(0);
            else
                readUnknownElement();
        }
    }
}
//! [3]

//! [4]
void XbelReader::readTitle(QTreeWidgetItem *item)
{
    Q_ASSERT(isStartElement() && name() == "title");

    QString title = readElementText();
    item->setText(0, title);
}
//! [4]

//! [5]
void XbelReader::readSeparator(QTreeWidgetItem *item)
{
    QTreeWidgetItem *separator = createChildItem(item);
    separator->setFlags(item->flags() & ~Qt::ItemIsSelectable);
    separator->setText(0, QString(30, 0xB7));
    readElementText();
}
//! [5]

void XbelReader::readFolder(QTreeWidgetItem *item)
{
    Q_ASSERT(isStartElement() && name() == "folder");

    QTreeWidgetItem *folder = createChildItem(item);
    bool folded = (attributes().value("folded") != "no");
    treeWidget->setItemExpanded(folder, !folded);

    while (!atEnd()) {
        readNext();

        if (isEndElement())
            break;

        if (isStartElement()) {
            if (name() == "title")
                readTitle(folder);
            else if (name() == "folder")
                readFolder(folder);
            else if (name() == "bookmark")
                readBookmark(folder);
            else if (name() == "separator")
                readSeparator(folder);
            else
                readUnknownElement();
        }
    }
}

void XbelReader::readBookmark(QTreeWidgetItem *item)
{
    Q_ASSERT(isStartElement() && name() == "bookmark");

    QTreeWidgetItem *bookmark = createChildItem(item);
    bookmark->setFlags(bookmark->flags() | Qt::ItemIsEditable);
    bookmark->setIcon(0, bookmarkIcon);
    bookmark->setText(0, QObject::tr("Unknown title"));
    bookmark->setText(1, attributes().value("href").toString());
    while (!atEnd()) {
        readNext();

        if (isEndElement())
            break;

        if (isStartElement()) {
            if (name() == "title")
                readTitle(bookmark);
            else
                readUnknownElement();
        }
    }
}

QTreeWidgetItem *XbelReader::createChildItem(QTreeWidgetItem *item)
{
    QTreeWidgetItem *childItem;
    if (item) {
        childItem = new QTreeWidgetItem(item);
    } else {
        childItem = new QTreeWidgetItem(treeWidget);
    }
    childItem->setData(0, Qt::UserRole, name().toString());
    return childItem;
}
