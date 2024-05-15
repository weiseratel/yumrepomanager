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

#include "createrepowizard.h"
#include "ui_createrepowizard.h"
#include "actionspage.h"
#include "yumrepomanager.h"
#include "progresspage.h"

#include <QWizard>
#include <QFileDialog>
#include <QDebug>
#include <QApplication>
#include <QMovie>

QString CreateRepoWizard::rpmFolder;
QString CreateRepoWizard::yumCache;
QString CreateRepoWizard::yumGlobalCache;

bool CreateRepoWizard::errorFlag;

CreateRepoWizard::CreateRepoWizard(QWidget *parent) : QWizard(parent), ui(new Ui::CreateRepoWizard)
{

    ui->setupUi(this);

    rpmFolder = "";
    yumCache = "";
    yumGlobalCache = "";
    homePath = "/home/";

    errorFlag = false;

    setWindowTitle(tr("Create Repository Wizard"));

    progress = new QMovie(":/Icons/Images/CreateRepository.gif");

    ui->labelProgress->setMovie(progress);

    progress->start();

    cursor = ui->textEditLogs->textCursor();

    ui->checkBoxAutomaticallyDeleteOldPackagesVersions->setChecked(YUMRepoManager::automaticallyDeleteOldPackagesVersions);

    connect(ui->Progress, SIGNAL(titlesChanged(QString,QString)), this, SLOT(updateTitles(QString,QString)));
    connect(ui->Progress, SIGNAL(progressMessageChanged(QString)), this, SLOT(updateProgressMessage(QString)));
    connect(ui->Progress, SIGNAL(progressImageChanged(QString)), this, SLOT(updateProgressImage(QString)));
    connect(ui->Progress, SIGNAL(logsChanged(QString)), this, SLOT(updateLogs(QString)));
    connect(ui->Progress, SIGNAL(cursorChanged()), this, SLOT(updateCursor()));
    connect(this, SIGNAL(rejected()), this, SLOT(rejected()));

}// Constructor

CreateRepoWizard::~CreateRepoWizard()
{

    delete ui;

}// Destructor

void CreateRepoWizard::updateTitles(const QString &title, const QString &subTitle) {

    ui->Progress->setTitle(title);
    ui->Progress->setSubTitle(subTitle);

}// updateTitles

void CreateRepoWizard::updateProgressMessage(const QString &message) {

    ui->toolBox->setItemText(0, message);

}// updateProgressMessage

void CreateRepoWizard::updateProgressImage(const QString &image) {

    progress->stop();

    ui->labelProgress->clear();
    ui->labelProgress->setPixmap(QPixmap(image));

    ui->labelProgress->setGeometry(135, 5, 271, 201);

}// updateProgressImage

void CreateRepoWizard::updateLogs(const QString &message) {

    ui->textEditLogs->insertPlainText(message);

}// updateLogs

void CreateRepoWizard::updateCursor() {

    cursor.movePosition(QTextCursor::End);

    ui->textEditLogs->setTextCursor(cursor);

}// updateCursor

void CreateRepoWizard::on_pushButtonFolder_clicked()
{

    rpmFolder = QFileDialog::getExistingDirectory(this, tr("Select The RPM Folder"), homePath);

    ui->Actions->rpmFolderClick();

}// RPM Folder

void CreateRepoWizard::on_pushButtonCache_clicked()
{

    yumCache = QFileDialog::getExistingDirectory(this, tr("Select Destination Folder"), homePath);

    ui->Actions->yumCacheClick();

}// YUM Cache

void CreateRepoWizard::on_pushButtonGlobalCache_clicked()
{

    yumGlobalCache = QFileDialog::getExistingDirectory(this, tr("Select Global Destination Folder"), homePath);

    ui->Actions->yumGlobalCacheClick();

}// Global YUM Cache

void CreateRepoWizard::on_checkBoxAutomaticallyDeleteOldPackagesVersions_clicked(bool checked)
{

    YUMRepoManager::automaticallyDeleteOldPackagesVersions = checked;

}// Automatically Delete Old Packages Versions

void CreateRepoWizard::rejected()
{

    //qDebug() << "Cancel Pressed!";

    ui->Progress->killOldPackagesProcess();
    ui->Progress->killCreaterepoProcess();

}// rejected
