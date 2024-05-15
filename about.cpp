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

#include "about.h"
#include "ui_about.h"

#include <QDebug>
#include <QHBoxLayout>

About::About(QWidget *parent) :  QDialog(parent), ui(new Ui::About)
{

    ui->setupUi(this);

}// Constructor

About::~About()
{

    delete ui;

}// Destructor

void About::on_label_linkActivated(const QString &link)
{

    Q_UNUSED(link);

    licenseDialog = new LicenseDialog(this);
    licenseDialog->setModal(true);
    licenseDialog->exec();

}// linkActivated

void About::on_buttonBox_rejected()
{

    this->close();

}// Close
