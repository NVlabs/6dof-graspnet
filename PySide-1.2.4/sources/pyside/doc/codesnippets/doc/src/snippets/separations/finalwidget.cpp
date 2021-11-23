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

/*
finalwidget.cpp

A widget to display an image and a label containing a description.
*/

#include <QtGui>
#include "finalwidget.h"

FinalWidget::FinalWidget(QWidget *parent, const QString &name,
                         const QSize &labelSize)
    : QFrame(parent)
{
    hasImage = false;
    imageLabel = new QLabel;
    imageLabel->setFrameShadow(QFrame::Sunken);
    imageLabel->setFrameShape(QFrame::StyledPanel);
    imageLabel->setMinimumSize(labelSize);
    nameLabel = new QLabel(name);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(imageLabel, 1);
    layout->addWidget(nameLabel, 0);
    setLayout(layout);
}

/*!
    If the mouse moves far enough when the left mouse button is held down,
    start a drag and drop operation.
*/

void FinalWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (!(event->buttons() & Qt::LeftButton))
        return;
    if ((event->pos() - dragStartPosition).manhattanLength()
         < QApplication::startDragDistance())
        return;
    if (!hasImage)
        return;

    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData;

//! [0]
    output = QByteArray()
    outputBuffer = QBuffer(output)
    outputBuffer.open(QIODevice.WriteOnly)
    imageLabel.pixmap().toImage().save(outputBuffer, "PNG")
    mimeData.setData("image/png", output)
//! [0]
/*
//! [1]
    mimeData.setImageData(QVariant(imageLabel.pixmap()))
//! [1]
*/
    drag.setMimeData(mimeData)
    drag.setPixmap(imageLabel.pixmap().scaled(64, 64, Qt.KeepAspectRatio))
//! [2]
    drag.setHotSpot(QPoint(drag.pixmap().width()/2,
                           drag.pixmap().height()))
//! [2]

    drag->start();
}

/*!
    Check for left mouse button presses in order to enable drag and drop.
*/

void FinalWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        dragStartPosition = event->pos();
}

const QPixmap* FinalWidget::pixmap() const
{
    return imageLabel->pixmap();
}

void FinalWidget::setPixmap(const QPixmap &pixmap)
{
    imageLabel->setPixmap(pixmap);
    hasImage = true;
}
