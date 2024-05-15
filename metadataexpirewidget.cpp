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

#include "metadataexpirewidget.h"
#include "yumrepomanager.h"

#include <QHBoxLayout>
#include <QDebug>

MetadataExpireWidget::MetadataExpireWidget(QWidget *parent) : MyWidget(parent)
{

    QHBoxLayout *layout = new QHBoxLayout(this);

    spinBoxAmount = new QSpinBox(this);
    comboBoxFrequency = new QComboBox(this);

    spinBoxAmount->setMinimumSize(70, 23);
    spinBoxAmount->setMaximumSize(70, 23);

    comboBoxFrequency->setMinimumSize(80, 23);
    comboBoxFrequency->setMaximumSize(80, 23);

    comboBoxFrequency->addItem(YUMRepoManager::translate("RepoDataModel", "Day/s"));
    comboBoxFrequency->addItem(YUMRepoManager::translate("RepoDataModel", "Hour/s"));
    comboBoxFrequency->addItem(YUMRepoManager::translate("RepoDataModel", "Minute/s"));

    metadataExpireFrequency = YUMRepoManager::translate("RepoDataModel", "Day/s");

    layout->addWidget(spinBoxAmount);
    layout->addWidget(comboBoxFrequency);

    connect(spinBoxAmount, SIGNAL(valueChanged(QString)), this, SLOT(setSpinBoxValue(QString)));
    connect(comboBoxFrequency, SIGNAL(activated(QString)), this, SLOT(setComboBoxValue(QString)));

}// Constructor

void MetadataExpireWidget::setComboBoxValue(const QString &value) {

    if(value == "Day/s") {

        metadataExpireFrequency = YUMRepoManager::translate("RepoDataModel", "Day/s");

        comboBoxFrequency->clear();

        comboBoxFrequency->addItem(YUMRepoManager::translate("RepoDataModel", "Day/s"));
        comboBoxFrequency->addItem(YUMRepoManager::translate("RepoDataModel", "Hour/s"));
        comboBoxFrequency->addItem(YUMRepoManager::translate("RepoDataModel", "Minute/s"));

    }// if
    else if(value == "Hour/s") {

        metadataExpireFrequency = YUMRepoManager::translate("RepoDataModel", "Hour/s");

        comboBoxFrequency->clear();

        comboBoxFrequency->addItem(YUMRepoManager::translate("RepoDataModel", "Hour/s"));
        comboBoxFrequency->addItem(YUMRepoManager::translate("RepoDataModel", "Minute/s"));
        comboBoxFrequency->addItem(YUMRepoManager::translate("RepoDataModel", "Day/s"));

    }// else if
    else if(value == "Minute/s") {

        metadataExpireFrequency = YUMRepoManager::translate("RepoDataModel", "Minute/s");

        comboBoxFrequency->clear();

        comboBoxFrequency->addItem(YUMRepoManager::translate("RepoDataModel", "Minute/s"));
        comboBoxFrequency->addItem(YUMRepoManager::translate("RepoDataModel", "Day/s"));
        comboBoxFrequency->addItem(YUMRepoManager::translate("RepoDataModel", "Hour/s"));

    }// else if

    emit comboBoxValueChanged(metadataExpireFrequency);

}// setFrequency
