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
  main.cpp

  A simple example of how to access items from an existing model.
*/

#include <QtGui>

/*!
    Create a default directory model and, using the index-based interface to
    the model and some QLabel widgets, populate the window's layout with the
    names of objects in the directory.

    Note that we only want to read the filenames in the highest level of the
    directory, so we supply a default (invalid) QModelIndex to the model in
    order to indicate that we want top-level items.
*/

def main():
    app = QApplication(sys.argc, sys.argv)

    window = QWidget()
    layout = QVBoxLayout(window)
    title = QLabel("Some items from the directory model", window)
    title.setBackgroundRole(QPalette.Base)
    title.setMargin(8)
    layout.addWidget(title)

//! [0]
    model = QFileSystemModel()
    parentIndex = model.index(QDir.currentPath())
    numRows = model.rowCount(parentIndex)
//! [0]

//! [1]
    for row in range(numRows):
        index = model.index(row, 0, parentIndex)
//! [1]

//! [2]
        text = model.data(index, Qt.DisplayRole)
        // Display the text in a widget.
//! [2]

        label = QLabel(text, window)
        layout.addWidget(label)
//! [3]
//! [3]

    window.setWindowTitle("A simple model example")
    window.show()
    return app.exec_()
}
