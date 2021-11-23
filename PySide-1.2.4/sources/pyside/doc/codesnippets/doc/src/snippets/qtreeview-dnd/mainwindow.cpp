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

#include "mainwindow.h"
#include "dragdropmodel.h"

MainWindow::MainWindow()
{
    QMenu *fileMenu = new QMenu(tr("&File"));

    QAction *quitAction = fileMenu->addAction(tr("E&xit"));
    quitAction->setShortcut(tr("Ctrl+Q"));

    menuBar()->addMenu(fileMenu);

//  For convenient quoting:
    QTreeView *treeView = new QTreeView(this);
    treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    treeView->setDragEnabled(true);
    treeView->setAcceptDrops(true);
    treeView->setDropIndicatorShown(true);

    this->treeView = treeView;

    connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));

    setupItems();

    setCentralWidget(treeView);
    setWindowTitle(tr("Tree View"));
}

void MainWindow::setupItems()
{
    QStringList items;
    items << tr("Widgets\tUser interface objects used to create GUI applications.")
          << tr("  QWidget\tThe basic building block for all other widgets.")
          << tr("  QDialog\tThe base class for dialog windows.")
          << tr("Tools\tUtilities and applications for Qt developers.")
          << tr("  Qt Designer\tA GUI form designer for Qt applications.")
          << tr("  Qt Assistant\tA documentation browser for Qt documentation.");

    DragDropModel *model = new DragDropModel(items, this);
    QModelIndex index = model->index(0, 0, QModelIndex());
    model->insertRows(2, 3, index);
    index = model->index(0, 0, QModelIndex());
    index = model->index(2, 0, index);
    model->setData(index, QVariant("QFrame"));
    model->removeRows(3, 2, index.parent());
    treeView->setModel(model);
}
