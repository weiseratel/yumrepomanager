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

#include "actionspage.h"
#include "createrepowizard.h"

#include <QTimer>
#include <QWizard>
#include <QAbstractButton>
#include <QApplication>

ActionsPage::ActionsPage(QWidget *parent) : QWizardPage(parent)
{

}// Constructor

void ActionsPage::initializePage() {

    QTimer::singleShot(0, this, SLOT(disableButton()));

}// initializePage

void ActionsPage::rpmFolderClick() {

    if(!CreateRepoWizard::rpmFolder.isEmpty()) {

        wizard()->next();

    }// if

}// RPM Folder Click

void ActionsPage::yumCacheClick() {

    if(!CreateRepoWizard::yumCache.isEmpty()) {

        wizard()->next();

    }// if

}// YUM Cache Click

void ActionsPage::yumGlobalCacheClick() {

    if(!CreateRepoWizard::yumGlobalCache.isEmpty()) {

        wizard()->next();

    }// else if

}// YUM Cache Click

bool ActionsPage::isComplete() const {

    return false;

}// isComplete

void ActionsPage::disableButton() {

    wizard()->button(QWizard::NextButton)->setVisible(false);

}// start

