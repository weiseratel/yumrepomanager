/*
    YUMRepoManager, a manager for the configuration files of YUM and his repositories.
    Copyright (C) 2011-2012  Josué V. Herrera

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "errorpage.h"
#include "createrepowizard.h"

#include <QAbstractButton>
#include <QTimer>

ErrorPage::ErrorPage(QWidget *parent) : QWizardPage(parent)
{

}// Constructor

void ErrorPage::initializePage() {

    QTimer::singleShot(0, this, SLOT(disableButton()));

    if(!CreateRepoWizard::errorFlag) {

        wizard()->next();

    }// if

}// initializePage

void ErrorPage::disableButton() {

    wizard()->button(QWizard::BackButton)->setVisible(false);
    wizard()->button(QWizard::NextButton)->setVisible(false);

    wizard()->button(QWizard::FinishButton)->setVisible(true);
    wizard()->button(QWizard::FinishButton)->setEnabled(true);

}// start
