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

#include <QtGui>

int main()
{
    QWidget anyPaintDevice;
    {
        // PEN SNIPPET
        QPainter painter;
        QPen pen(Qt::red, 2);                 // red solid line, 2 pixels wide
        painter.begin(&anyPaintDevice);   // paint something
        painter.setPen(pen);              // set the red, wide pen
        painter.drawRect(40,30, 200,100); // draw a rectangle
        painter.setPen(Qt::blue);             // set blue pen, 0 pixel width
        painter.drawLine(40,30, 240,130); // draw a diagonal in rectangle
        painter.end();                    // painting done
    }

    {
        // BRUSH SNIPPET
        QPainter painter;
        QBrush brush(Qt::yellow);           // yellow solid pattern
        painter.begin(&anyPaintDevice);   // paint something
        painter.setBrush(brush);          // set the yellow brush
        painter.setPen(Qt::NoPen);        // do not draw outline
        painter.drawRect(40,30, 200,100); // draw filled rectangle
        painter.setBrush(Qt::NoBrush);    // do not fill
        painter.setPen(Qt::black);            // set black pen, 0 pixel width
        painter.drawRect(10,10, 30,20);   // draw rectangle outline
        painter.end();                    // painting done
    }

    // LINEAR
//! [0]
    linearGrad = QLinearGradient(QPointF(100, 100), QPointF(200, 200))
    linearGrad.setColorAt(0, Qt.black)
    linearGrad.setColorAt(1, Qt.white)
//! [0]

    // RADIAL
//! [1]
    radialGrad = QRadialGradient(QPointF(100, 100), 100)
    radialGrad.setColorAt(0, Qt.red)
    radialGrad.setColorAt(0.5, Qt.blue)
    radialGrad.setColorAt(1, Qt.green)
//! [1]
}
