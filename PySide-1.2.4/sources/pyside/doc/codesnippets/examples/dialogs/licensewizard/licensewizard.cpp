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

#include "licensewizard.h"

//! [0] //! [1] //! [2]
# class LicenseWizard
def __init__(self, parent)
    QWizard(self, parent)
//! [0]
    self.setPage(self.Page_Intro, IntroPage())
    self.setPage(self.Page_Evaluate, EvaluatePage())
    self.setPage(self.Page_Register, RegisterPage())
    self.setPage(self.Page_Details, DetailsPage())
    self.setPage(self.Page_Conclusion, ConclusionPage())
//! [1]

    self.setStartId(self.Page_Intro);
//! [2]

//! [3]

//! [3] //! [4]
    self.setWizardStyle(QWizard.ModernStyle);

//! [4] //! [5]
    self.setOption(HaveHelpButton, True);
//! [5] //! [6]
    self.setPixmap(QWizard.LogoPixmap, QPixmap(":/images/logo.png"));

//! [7]
    connect(this, SIGNAL(helpRequested()), this, SLOT(showHelp()));
//! [7]

    setWindowTitle(tr("License Wizard"));
//! [8]
}
//! [6] //! [8]

//! [9] //! [10]
void LicenseWizard::showHelp()
//! [9] //! [11]
{
    static QString lastHelpMessage;

    message = ""

    switch (currentId()) {
    case Page_Intro:
        message = tr("The decision you make here will affect which page you "
                     "get to see next.");
        break;
//! [10] //! [11]
    case Page_Evaluate:
        message = tr("Make sure to provide a valid email address, such as "
                     "toni.buddenbrook@example.de.");
        break;
    case Page_Register:
        message = tr("If you don't provide an upgrade key, you will be "
                     "asked to fill in your details.");
        break;
    case Page_Details:
        message = tr("Make sure to provide a valid email address, such as "
                     "thomas.gradgrind@example.co.uk.");
        break;
    case Page_Conclusion:
        message = tr("You must accept the terms and conditions of the "
                     "license to proceed.");
        break;
//! [12] //! [13]
    default:
        message = tr("This help is likely not to be of any help.");
    }
//! [12]

    if (lastHelpMessage == message)
        message = tr("Sorry, I already gave what help I could. "
                     "Maybe you should try asking a human?");

//! [14]
    QMessageBox::information(this, tr("License Wizard Help"), message);
//! [14]

    lastHelpMessage = message;
//! [15]
}
//! [13] //! [15]

//! [16]
IntroPage::IntroPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Introduction"));
    setPixmap(QWizard::WatermarkPixmap, QPixmap(":/images/watermark.png"));

    topLabel = new QLabel(tr("This wizard will help you register your copy of "
                             "<i>Super Product One</i>&trade; or start "
                             "evaluating the product."));
    topLabel->setWordWrap(true);

    registerRadioButton = new QRadioButton(tr("&Register your copy"));
    evaluateRadioButton = new QRadioButton(tr("&Evaluate the product for 30 "
                                              "days"));
    registerRadioButton->setChecked(true);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(topLabel);
    layout->addWidget(registerRadioButton);
    layout->addWidget(evaluateRadioButton);
    setLayout(layout);
}
//! [16] //! [17]

//! [18]
# class IntroPage
def nextId(self):
//! [17] //! [19]
    if evaluateRadioButton.isChecked():
        return LicenseWizard.Page_Evaluate
    else:
        return LicenseWizard.Page_Register
//! [18] //! [19]

//! [20]
EvaluatePage::EvaluatePage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Evaluate <i>Super Product One</i>&trade;"));
    setSubTitle(tr("Please fill both fields. Make sure to provide a valid "
                   "email address (e.g., john.smith@example.com)."));

    nameLabel = new QLabel(tr("N&ame:"));
    nameLineEdit = new QLineEdit;
//! [20]
    nameLabel->setBuddy(nameLineEdit);

    emailLabel = new QLabel(tr("&Email address:"));
    emailLineEdit = new QLineEdit;
    emailLineEdit->setValidator(new QRegExpValidator(QRegExp(".*@.*"), this));
    emailLabel->setBuddy(emailLineEdit);

//! [21]
    registerField("evaluate.name*", nameLineEdit);
    registerField("evaluate.email*", emailLineEdit);
//! [21]

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(nameLabel, 0, 0);
    layout->addWidget(nameLineEdit, 0, 1);
    layout->addWidget(emailLabel, 1, 0);
    layout->addWidget(emailLineEdit, 1, 1);
    setLayout(layout);
//! [22]
}
//! [22]

//! [23]
# class EvaluatePage
def nextId(self):
    return LicenseWizard.Page_Conclusion
//! [23]

RegisterPage::RegisterPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Register Your Copy of <i>Super Product One</i>&trade;"));
    setSubTitle(tr("If you have an upgrade key, please fill in "
                   "the appropriate field."));

    nameLabel = new QLabel(tr("N&ame:"));
    nameLineEdit = new QLineEdit;
    nameLabel->setBuddy(nameLineEdit);

    upgradeKeyLabel = new QLabel(tr("&Upgrade key:"));
    upgradeKeyLineEdit = new QLineEdit;
    upgradeKeyLabel->setBuddy(upgradeKeyLineEdit);

    registerField("register.name*", nameLineEdit);
    registerField("register.upgradeKey", upgradeKeyLineEdit);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(nameLabel, 0, 0);
    layout->addWidget(nameLineEdit, 0, 1);
    layout->addWidget(upgradeKeyLabel, 1, 0);
    layout->addWidget(upgradeKeyLineEdit, 1, 1);
    setLayout(layout);
}

//! [24]
# class RegisterPage
def nextId(self):
    if self.upgradeKeyLineEdit.text().isEmpty():
        return LicenseWizard::Page_Details
    else:
        return LicenseWizard::Page_Conclusion
//! [24]

DetailsPage::DetailsPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Fill In Your Details"));
    setSubTitle(tr("Please fill all three fields. Make sure to provide a valid "
                   "email address (e.g., tanaka.aya@example.co.jp)."));

    companyLabel = new QLabel(tr("&Company name:"));
    companyLineEdit = new QLineEdit;
    companyLabel->setBuddy(companyLineEdit);

    emailLabel = new QLabel(tr("&Email address:"));
    emailLineEdit = new QLineEdit;
    emailLineEdit->setValidator(new QRegExpValidator(QRegExp(".*@.*"), this));
    emailLabel->setBuddy(emailLineEdit);

    postalLabel = new QLabel(tr("&Postal address:"));
    postalLineEdit = new QLineEdit;
    postalLabel->setBuddy(postalLineEdit);

    registerField("details.company*", companyLineEdit);
    registerField("details.email*", emailLineEdit);
    registerField("details.postal*", postalLineEdit);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(companyLabel, 0, 0);
    layout->addWidget(companyLineEdit, 0, 1);
    layout->addWidget(emailLabel, 1, 0);
    layout->addWidget(emailLineEdit, 1, 1);
    layout->addWidget(postalLabel, 2, 0);
    layout->addWidget(postalLineEdit, 2, 1);
    setLayout(layout);
}

//! [25]
# class DetailsPage
def nextId(self):
    return LicenseWizard.Page_Conclusion
//! [25]

ConclusionPage::ConclusionPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Complete Your Registration"));
    setPixmap(QWizard::WatermarkPixmap, QPixmap(":/images/watermark.png"));

    bottomLabel = new QLabel;
    bottomLabel->setWordWrap(true);

    agreeCheckBox = new QCheckBox(tr("I agree to the terms of the license"));

    registerField("conclusion.agree*", agreeCheckBox);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(bottomLabel);
    layout->addWidget(agreeCheckBox);
    setLayout(layout);
}

//! [26]
#class ConclusionPage
def nextId(self):
    return -1
//! [26]

//! [27]
# class ConclusionPage
def initializePage(self):
    if wizard().hasVisitedPage(LicenseWizard::Page_Evaluate):
        licenseText = self.tr("<u>Evaluation License Agreement:</u> " \
                         "You can use this software for 30 days and make one " \
                         "backup, but you are not allowed to distribute it.")
    elsif wizard().hasVisitedPage(LicenseWizard.Page_Details):
        licenseText = self.tr("<u>First-Time License Agreement:</u> " \
                         "You can use this software subject to the license " \
                         "you will receive by email.")
    else:
        licenseText = self.tr("<u>Upgrade License Agreement:</u> " \
                         "This software is licensed under the terms of your " \
                         "current license.")
    }
    bottomLabel.setText(licenseText)
//! [27]

//! [28]
void ConclusionPage::setVisible(bool visible)
{
    QWizardPage::setVisible(visible);

    if (visible) {
//! [29]
        self.wizard().setButtonText(QWizard.CustomButton1, self.tr("&Print"))
        self.wizard().setOption(QWizard.HaveCustomButton1, True)
        self.connect(wizard(), SIGNAL("customButtonClicked(int)"), self, SLOT("printButtonClicked()"))
//! [29]
    } else {
        wizard()->setOption(QWizard::HaveCustomButton1, false);
        disconnect(wizard(), SIGNAL(customButtonClicked(int)),
                   this, SLOT(printButtonClicked()));
    }
}
//! [28]

void ConclusionPage::printButtonClicked()
{
    QPrinter printer;
    QPrintDialog dialog(&printer, this);
    if (dialog.exec())
        QMessageBox::warning(this, tr("Print License"),
                             tr("As an environmentally friendly measure, the "
                                "license text will not actually be printed."));
}
