######################################
#
# Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
# Contact: http://www.qt-project.org/legal
#
# This file is part of the example classes of the Qt Toolkit.
#
# $QT_BEGIN_LICENSE:LGPL$
# Commercial License Usage
# Licensees holding valid commercial Qt licenses may use this file in
# accordance with the commercial license agreement provided with the
# Software or, alternatively, in accordance with the terms contained in
# a written agreement between you and Digia.  For licensing terms and
# conditions see http://qt.digia.com/licensing.  For further information
# use the contact form at http://qt.digia.com/contact-us.
#
# GNU Lesser General Public License Usage
# Alternatively, this file may be used under the terms of the GNU Lesser
# General Public License version 2.1 as published by the Free Software
# Foundation and appearing in the file LICENSE.LGPL included in the
# packaging of this file.  Please review the following information to
# ensure the GNU Lesser General Public License version 2.1 requirements
# will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
#
# In addition, as a special exception, Digia gives you certain additional
# rights.  These rights are described in the Digia Qt LGPL Exception
# version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
#
# GNU General Public License Usage
# Alternatively, this file may be used under the terms of the GNU
# General Public License version 3.0 as published by the Free Software
# Foundation and appearing in the file LICENSE.GPL included in the
# packaging of this file.  Please review the following information to
# ensure the GNU General Public License version 3.0 requirements will be
# met: http://www.gnu.org/copyleft/gpl.html.
#
#
# $QT_END_LICENSE$
#
######################################

from PySide.QtGui import *
from PySide.QtSql import *

def initializeModel(model):
//! [0]
    model.setTable("employee")
//! [0]

    model.setEditStrategy(QSqlTableModel.OnManualSubmit)
//! [1]
    model.setRelation(2, QSqlRelation("city", "id", "name"))
//! [1] //! [2]
    model.setRelation(3, QSqlRelation("country", "id", "name"))
//! [2]

//! [3]
    model.setHeaderData(0, Qt.Horizontal, QObject::tr("ID"))
    model.setHeaderData(1, Qt.Horizontal, QObject::tr("Name"))
    model.setHeaderData(2, Qt.Horizontal, QObject::tr("City"))
    model.setHeaderData(3, Qt.Horizontal, QObject::tr("Country"))
//! [3]

    model.select()


def createView(title, model):
//! [4]
    view =  QTableView()
    view.setModel(model)
    view.setItemDelegate(QSqlRelationalDelegate(view))
//! [4]
    view.setWindowTitle(title)
    return view


def createRelationalTables():
    query = QSqlQuery()
    query.exec_("create table employee(id int primary key, name varchar(20), city int, country int)")
    query.exec_("insert into employee values(1, 'Espen', 5000, 47)")
    query.exec_("insert into employee values(2, 'Harald', 80000, 49)")
    query.exec_("insert into employee values(3, 'Sam', 100, 1)")

    query.exec_("create table city(id int, name varchar(20))")
    query.exec_("insert into city values(100, 'San Jose')")
    query.exec_("insert into city values(5000, 'Oslo')")
    query.exec_("insert into city values(80000, 'Munich')")

    query.exec_("create table country(id int, name varchar(20))")
    query.exec_("insert into country values(1, 'USA')")
    query.exec_("insert into country values(47, 'Norway')")
    query.exec_("insert into country values(49, 'Germany')")


def main():

    app = QApplication([])
    if !createConnection():
        return 1

    createRelationalTables()

    model = QSqlRelationalTableModel()

    initializeModel(model)

    view = createView(QObject.tr("Relational Table Model"), model)
    view.show()

    return app.exec_()

