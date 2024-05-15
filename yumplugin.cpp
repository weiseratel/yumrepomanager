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

#include "yumplugin.h"
#include "ui_yumplugin.h"
#include "dataflow.h"
#include "plugindata.h"
#include "texteditor.h"
#include "yumrepomanager.h"

#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QIODevice>
#include <QDirIterator>
#include <QDebug>

YUMPlugin::YUMPlugin(QWidget *parent) : QDialog(parent), ui(new Ui::YUMPlugin)
{

    ui->setupUi(this);

    ui->listView->setModel(DataFlow::getPluginModel());

    connect(DataFlow::getPluginModel(), SIGNAL(stateChanged(int,bool)), this, SLOT(pluginStateChanged(int,bool)));

}// Constructor

YUMPlugin::~YUMPlugin()
{

    delete ui;

}// Destructor

void YUMPlugin::on_buttonBox_accepted()
{

    for(int index = 0; index < DataFlow::pluginDataListSize(); ++index) {

        PluginData pluginNode = DataFlow::getPluginDataList().at(index);

        QFile file(pluginNode.pluginFileName);

        file.open(QIODevice::ReadOnly | QIODevice::Text);

        QTextStream inFlow(&file);
        inFlow.setCodec("UTF-8");

        QString pluginData = inFlow.readAll();

        QTextStream editFlow(&pluginData);
        editFlow.setCodec("UTF-8");

        file.close();

        file.open(QIODevice::Truncate | QIODevice::WriteOnly | QIODevice::Text);

        QTextStream outFlow(&file);
        outFlow.setCodec("UTF-8");

        int line = 0;

        for(;!editFlow.atEnd();) {

            if(line == (pluginNode.pluginEnabledPosition - 1)) {

                outFlow << "enabled=" << pluginNode.pluginEnabled[1] << "\n";

                editFlow.readLine();

                ++line;

                continue;

            }// if

            outFlow << editFlow.readLine() << "\n";

            ++line;

        }// for

        outFlow.flush();

        file.close();

    }// for

    this->accept();

}// Accepted

void YUMPlugin::on_buttonBox_rejected()
{

    this->reject();

}// Rejected

void YUMPlugin::on_pushButtonEdit_clicked()
{

    QItemSelectionModel *selection = ui->listView->selectionModel();

    if(selection->hasSelection()) {

        //qDebug() << "Traying to open: " << DataFlow::getPluginDataList().at(selection->currentIndex().row()).pluginFileName;

        QFile myFile(DataFlow::getPluginDataList().at(selection->currentIndex().row()).pluginFileName);

        if(!myFile.open(QIODevice::ReadOnly | QIODevice::Text)) {

            YUMRepoManager::showMessage("Error", tr("Access denied when trying to open the config file"));

            return;

        }// if

        myFile.close();

        textEditor = new TextEditor(DataFlow::getPluginDataList().at(selection->currentIndex().row()).pluginFileName, "plugin", this);
        textEditor->setModal(true);
        textEditor->exec();

    }// if
    else {

        YUMRepoManager::showMessage("Information", tr("You have to select a plugin first."));

    }// else

}// Edit Clicked

void YUMPlugin::on_listView_doubleClicked(const QModelIndex &index)
{

    //qDebug() << "Traying to open: " << DataFlow::getPluginDataList().at(index.row()).pluginFileName;

    QFile myFile(DataFlow::getPluginDataList().at(index.row()).pluginFileName);

    if(!myFile.open(QIODevice::ReadOnly | QIODevice::Text)) {

        YUMRepoManager::showMessage("Error", tr("Access denied when trying to open the config file"));

        return;

    }// if

    myFile.close();

    textEditor = new TextEditor(DataFlow::getPluginDataList().at(index.row()).pluginFileName, "plugin", this);
    textEditor->setModal(true);
    textEditor->exec();

}// ListView DoubleClicked
