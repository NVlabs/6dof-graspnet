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

#include "window.h"

MyWidget::MyWidget(QWidget *parent)
    : QWidget(parent)
{
    QLabel *textLabel = new QLabel(tr("Data:"), this);
    dataLabel = new QLabel(this);
    dataLabel->setFixedSize(200, 200);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(textLabel);
    layout->addWidget(dataLabel);

    setAcceptDrops(true);
    setWindowTitle(tr("Drop Events"));
}

//! [0]
void MyWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasText() || event->mimeData()->hasImage())
        event->acceptProposedAction();
}
//! [0]

//! [1]
void MyWidget::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasText())
        dataLabel->setText(event->mimeData()->text());
    else if (event->mimeData()->hasImage()) {
        QVariant imageData = event->mimeData()->imageData();
        dataLabel->setPixmap(qvariant_cast<QPixmap>(imageData));
    }
    event->acceptProposedAction();
}
//! [1]

//! [2]
void MyWidget::mousePressEvent(QMouseEvent *event)
{
//! [2]
    QString text = dataLabel->text();
    QPixmap iconPixmap(32, 32);
    iconPixmap.fill(qRgba(255, 0, 0, 127));
    QImage image(100, 100, QImage::Format_RGB32);
    image.fill(qRgb(0, 0, 255));

//! [3]
    if (event->button() == Qt::LeftButton) {

        QDrag *drag = new QDrag(this);
        QMimeData *mimeData = new QMimeData;

        mimeData->setText(text);
        mimeData->setImageData(image);
        drag->setMimeData(mimeData);
        drag->setPixmap(iconPixmap);

        Qt::DropAction dropAction = drag->exec();
//! [3]
        // ...
//! [4]
        event->accept();
    }
//! [4]
    else if (event->button() == Qt::MidButton) {

        QDrag *drag = new QDrag(this);
        QMimeData *mimeData = new QMimeData;

        mimeData->setImageData(image);
        drag->setMimeData(mimeData);
        drag->setPixmap(iconPixmap);

        Qt::DropAction dropAction = drag->exec();
        // ...
        event->accept();
    }
//! [5]
}
//! [5]
