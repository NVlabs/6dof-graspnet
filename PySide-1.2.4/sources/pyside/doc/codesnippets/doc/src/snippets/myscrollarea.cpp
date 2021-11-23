/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the documentation of the Qt Toolkit.
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

from PySide.QtCore import *
from PySide.QtGui import *
import sys

class MyScrollArea (QAbstractScrollArea):
    widget = None

    def __init__(widget):
        self.setWidget(widget);

    def setWidget(w):
        self.widget = w;
        self.widget.setParent(viewport());
        if not self.widget.testAttribute(Qt.WA_Resized):
            self.widget.resize(widget.sizeHint())

        self.verticalScrollBar().setValue(0)
        self.verticalScrollBar().setValue(0)

        self.updateArea()


    def updateWidgetPosition():
//! [0]
        hvalue = self.horizontalScrollBar().value()
        vvalue = self.verticalScrollBar().value()
        topLeft = self.viewport().rect().topLeft()

        self.widget.move(topLeft.x() - hvalue, topLeft.y() - vvalue)
//! [0]


    def scrollContentsBy(dx, dy):
        self.updateWidgetPosition()

    def updateArea():
//! [1]
        areaSize = viewport().size()
        widgetSize = widget.size()

        self.verticalScrollBar().setPageStep(widgetSize.height())
        self.horizontalScrollBar().setPageStep(widgetSize.width())
        self.verticalScrollBar().setRange(0, widgetSize.height() - areaSize.height())
        self.horizontalScrollBar().setRange(0, widgetSize.width() - areaSize.width())
        self.updateWidgetPosition()
//! [1]

    def resizeEvent(QResizeEvent):
        self.updateArea()

app = QApplication(sys.argv)
pixmap = QPixmap("mypixmap.png")
label = QLabel()
label.setPixmap(pixmap)
area = MyScrollArea(label)
area.resize(300, 300)
area.show()

area.setWidget(label)

app.exec_();

