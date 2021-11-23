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
#include "model.h"

/*!
    The main window constructor creates and populates a model with values
    from a string list then displays the contents of the model using a
    QListView widget.
*/

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QMenu *fileMenu = new QMenu(tr("&File"));

    QAction *quitAction = fileMenu->addAction(tr("E&xit"));
    quitAction->setShortcut(tr("Ctrl+Q"));

    QMenu *itemsMenu = new QMenu(tr("&Items"));

    insertAction = itemsMenu->addAction(tr("&Insert Item"));
    removeAction = itemsMenu->addAction(tr("&Remove Item"));

    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(itemsMenu);

    QStringList numbers;
    numbers << tr("One") << tr("Two") << tr("Three") << tr("Four") << tr("Five")
            << tr("Six") << tr("Seven") << tr("Eight") << tr("Nine") << tr("Ten");

    model = new StringListModel(numbers);
    QListView *view = new QListView(this);
    view->setModel(model);

    selectionModel = view->selectionModel();

    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(insertAction, SIGNAL(triggered()), this, SLOT(insertItem()));
    connect(removeAction, SIGNAL(triggered()), this, SLOT(removeItem()));
    connect(selectionModel,
            SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
            this, SLOT(updateMenus(const QModelIndex &)));
    
    setCentralWidget(view);
    setWindowTitle("View onto a string list model");
}

void MainWindow::insertItem()
{
    if (!selectionModel->currentIndex().isValid())
        return;

    QString itemText = QInputDialog::getText(this, tr("Insert Item"),
        tr("Input text for the new item:"));

    if (itemText.isNull())
        return;

    int row = selectionModel->currentIndex().row();

    if (model->insertRows(row, 1))
        model->setData(model->index(row, 0), itemText, Qt::EditRole);
}

void MainWindow::removeItem()
{
    if (!selectionModel->currentIndex().isValid())
        return;

    int row = selectionModel->currentIndex().row();

    model->removeRows(row, 1);
}

void MainWindow::updateMenus(const QModelIndex &currentIndex)
{
    insertAction->setEnabled(currentIndex.isValid());
    removeAction->setEnabled(currentIndex.isValid());
}
