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
#include <QTranslator>
#include <QLocale>
#include <QLibraryInfo>

//! [0] //! [1]
def createIntroPage(self):
    page = QWizardPage()
    page.setTitle("Introduction")

    label = QLabel("This wizard will help you register your copy of Super Product Two.")
    label.setWordWrap(True)

    layout = QVBoxLayout()
    layout.addWidget(label)
    page.setLayout(layout)

    return page
//! [0]

//! [2]
QWizardPage *createRegistrationPage()
//! [1] //! [3]

//! [3]
    QWizardPage *page = new QWizardPage;
    page->setTitle("Registration");
    page->setSubTitle("Please fill both fields.");

    QLabel *nameLabel = new QLabel("Name:");
    QLineEdit *nameLineEdit = new QLineEdit;

    QLabel *emailLabel = new QLabel("Email address:");
    QLineEdit *emailLineEdit = new QLineEdit;

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(nameLabel, 0, 0);
    layout->addWidget(nameLineEdit, 0, 1);
    layout->addWidget(emailLabel, 1, 0);
    layout->addWidget(emailLineEdit, 1, 1);
    page->setLayout(layout);

    return page;
//! [4]

//! [2] //! [4]

//! [5] //! [6]
def createConclusionPage(self):
//! [5] //! [7]

//! [7]
    QWizardPage *page = new QWizardPage;
    page->setTitle("Conclusion");

    QLabel *label = new QLabel("You are now successfully registered. Have a "
                               "nice day!");
    label->setWordWrap(true);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(label);
    page->setLayout(layout);

    return page;
//! [8]

//! [6] //! [8]

//! [9] //! [10]

//! [9] //! [11]
def main():
    app = QApplication(sys.argv)

    translatorFileName = "qt_"
    translatorFileName += QLocale.system().name()
    translator = QTranslator(app)
    if translator.load(translatorFileName, QLibraryInfo.location(QLibraryInfo.TranslationsPath)):
        app.installTranslator(translator)

    wizard = QWizard()
    wizard.addPage(createIntroPage())
    wizard.addPage(createRegistrationPage())
    wizard.addPage(createConclusionPage())

    wizard.setWindowTitle("Trivial Wizard")
    wizard.show()

    return app.exec_()

if __name__ == "__main__":
    main()
//! [10] //! [11]
