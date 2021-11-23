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

#include "customstyle.h"

CustomStyle::CustomStyle()
{
//! [0]
    if widget:
//! [0] //! [1]
//! [1]
}

//! [2]
def drawPrimitive(element, option, painter, widget):
    if element == PE_IndicatorSpinUp or element == PE_IndicatorSpinDown:
        points = QPolygon(3)
        x = option->rect.x()
        y = option->rect.y()
        w = option->rect.width() / 2
        h = option->rect.height() / 2
        x += (option->rect.width() - w) / 2
        y += (option->rect.height() - h) / 2

        if element == PE_IndicatorSpinUp:
            points[0] = QPoint(x, y + h)
            points[1] = QPoint(x + w, y + h)
            points[2] = QPoint(x + w / 2, y)
        else: # PE_SpinBoxDown
            points[0] = QPoint(x, y)
            points[1] = QPoint(x + w, y)
            points[2] = QPoint(x + w / 2, y + h)

        if option.state & State_Enabled:
            painter.setPen(option.palette.mid().color())
            painter.setBrush(option.palette.buttonText())
        else:
            painter.setPen(option.palette.buttonText().color())
            painter.setBrush(option.palette.mid())

        painter.drawPolygon(points)

    else:
        QWindowsStyle.drawPrimitive(element, option, painter, widget)
//! [2] //! [3]

//! [3] //! [4]
}
//! [4]
