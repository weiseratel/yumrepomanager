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

#include "addrepository.h"
#include "ui_addrepository.h"

#include "repodata.h"
#include "yumrepomanager.h"

#include <QDebug>
#include <QFileDialog>

AddRepository::AddRepository(QWidget *parent) : QDialog(parent), ui(new Ui::AddRepository)
{

    ui->setupUi(this);

    ui->comboBoxAddress->addItem("Base URL");
    ui->comboBoxAddress->addItem("Mirror List");

}// Constructor

AddRepository::~AddRepository()
{

    delete ui;

}// Destructor

void AddRepository::on_buttonBox_accepted()
{

    if(ui->lineEditRepoName->text().isEmpty()) {

        YUMRepoManager::showMessage("Error", "You have to specify a name for this repository.");

        return;

    }// if

    if(ui->lineEditAddress->text().isEmpty()) {

        YUMRepoManager::showMessage("Error", "You have to specify the address of this repository.");

        return;

    }// if

    RepoData node;

    node.repoName = ui->lineEditRepoName->text();

    if(!ui->lineEditComment->text().isEmpty()) {

        node.repoComment[0] = "name";
        node.repoComment[1] = ui->lineEditComment->text();

    }// if
    else {

        node.repoComment[0] = "name";
        node.repoComment[1] = ui->lineEditRepoName->text();

    }// else

    if(ui->comboBoxAddress->currentText() == "Base URL") {

        node.repoBaseURL[0] = "1";
        node.repoBaseURL[1] = "baseurl";
        node.repoBaseURL[2] = ui->lineEditAddress->text();

    }// if
    else if(ui->comboBoxAddress->currentText() == "Mirror List") {

        node.repoMetaLink[0] = "1";
        node.repoMetaLink[1] = "metalink";
        node.repoMetaLink[2] = ui->lineEditAddress->text();

    }// else if

    DataFlow::setDefaultValues(node);

    DataFlow::insertNewNode(node);

}// ButtonBox Accepted

void AddRepository::on_toolButton_clicked()
{

    QString repository;

    repository = QFileDialog::getExistingDirectory(this, tr("Select The Repositories Directory"), "/home/");

    ui->lineEditAddress->setText(QString("file://") + repository);

}// ToolButton Clicked
