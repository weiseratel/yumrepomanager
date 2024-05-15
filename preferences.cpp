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

#include "preferences.h"
#include "ui_preferences.h"
#include "yumrepomanager.h"

#include <QTranslator>
#include <QDebug>

Preferences::Preferences(QWidget *parent) :  QDialog(parent), ui(new Ui::Preferences)
{

    ui->setupUi(this);

    //
    //  Color
    //

    cellColor[Enabled] = YUMRepoManager::colorEnabled();
    cellColor[Disabled] = YUMRepoManager::colorDisabled();

    defaultCellColor[Enabled].setRgb(111, 166, 99, 255);
    defaultCellColor[Disabled].setRgb(226, 150, 112, 255);

    restoreDefaultStatus();

    ui->labelEnabledColor->setAutoFillBackground(true);
    ui->labelDisabledColor->setAutoFillBackground(true);

    ui->labelEnabledColor->setPalette(QPalette(cellColor[Enabled]));
    ui->labelDisabledColor->setPalette(QPalette(cellColor[Disabled]));

    colorDialog[Enabled] = new QColorDialog(this);
    colorDialog[Disabled] = new QColorDialog(this);

    connect(colorDialog[Enabled], SIGNAL(colorSelected(QColor)), this, SLOT(setColorEnabled(QColor)));
    connect(colorDialog[Disabled], SIGNAL(colorSelected(QColor)), this, SLOT(setColorDisabled(QColor)));

    //
    //  Language
    //
    setActiveFlag(YUMRepoManager::preferredLanguage);

}// Constructor

Preferences::~Preferences()
{

    delete ui;

}// Destructor

void Preferences::setColorEnabled(const QColor &color) {

    cellColor[Enabled] = color;

    ui->labelEnabledColor->setPalette(QPalette(cellColor[Enabled]));

    restoreDefaultStatus();

    emit colorEnabledChanged(cellColor[Enabled]);

}// setColor

void Preferences::setColorDisabled(const QColor &color) {

    cellColor[Disabled] = color;

    ui->labelDisabledColor->setPalette(QPalette(cellColor[Disabled]));

    restoreDefaultStatus();

    emit colorDisabledChanged(cellColor[Disabled]);

}// setColor

void Preferences::on_pushButtonEnabledSelect_clicked()
{

    colorDialog[Enabled]->setModal(true);
    colorDialog[Enabled]->setCurrentColor(cellColor[Enabled]);
    colorDialog[Enabled]->show();

}// EnableSelect

void Preferences::on_pushButtonDisabledSelect_clicked()
{

    colorDialog[Disabled]->setModal(true);
    colorDialog[Disabled]->setCurrentColor(cellColor[Disabled]);
    colorDialog[Disabled]->show();

}// DisableSelect

void Preferences::setActiveFlag(const QString &activeFlag) {

    ui->comboBoxLanguage->clear();

    if(activeFlag == "en") {

        ui->comboBoxLanguage->addItem(QIcon(":/Icons/Images/EnglishFlag.png"), "English");
        ui->comboBoxLanguage->addItem(QIcon(":/Icons/Images/SpanishFlag.png"), "Spanish");

    }// if
    else if(activeFlag == "es") {

        ui->comboBoxLanguage->addItem(QIcon(":/Icons/Images/SpanishFlag.png"), "Spanish");
        ui->comboBoxLanguage->addItem(QIcon(":/Icons/Images/EnglishFlag.png"), "English");

    }// else if
    else {

        ui->comboBoxLanguage->addItem(QIcon(":/Icons/Images/EnglishFlag.png"), "English");
        ui->comboBoxLanguage->addItem(QIcon(":/Icons/Images/SpanishFlag.png"), "Spanish");

    }// else

    if(YUMRepoManager::preferredLanguage != "SystemLocale") {

        ui->checkBoxLanguage->setChecked(false);

        ui->labelLanguage->setEnabled(true);
        ui->comboBoxLanguage->setEnabled(true);

    }// if

}// setActiveFlag

void Preferences::on_checkBoxLanguage_clicked()
{

    if(ui->checkBoxLanguage->isChecked()) {

        ui->labelLanguage->setEnabled(false);
        ui->comboBoxLanguage->setEnabled(false);

        setActiveFlag(YUMRepoManager::systemLocale);

        YUMRepoManager::preferredLanguage = "SystemLocale";

        emit languageChangeOcurred(YUMRepoManager::systemLocale);

        restoreDefaultStatus();

    }// if
    else {

        ui->labelLanguage->setEnabled(true);
        ui->comboBoxLanguage->setEnabled(true);

        restoreDefaultStatus();

    }// else

}// checkBox Language Clicked

void Preferences::on_comboBoxLanguage_currentIndexChanged(const QString &language)
{

    if(language == "English") {

        emit languageChangeOcurred("en");

    }// if
    else if(language == "Spanish") {

        emit languageChangeOcurred("es");

    }// else if

}// comboBoxLanguage currentIndex Changed

void Preferences::restoreDefaultStatus() {

    if(ui->tabWidget->currentIndex() == 0) {

        if(cellColor[Enabled] != defaultCellColor[Enabled]) {

            ui->pushButtonRestoreDefault->setEnabled(true);

        }// if
        else if(cellColor[Disabled] != defaultCellColor[Disabled]) {

            ui->pushButtonRestoreDefault->setEnabled(true);

        }// else if
        else {

            ui->pushButtonRestoreDefault->setEnabled(false);

        }// else

    }// if
    else if(ui->tabWidget->currentIndex() == 1) {

        if(ui->checkBoxLanguage->isChecked()) {

            ui->pushButtonRestoreDefault->setEnabled(false);

        }// if
        else {

            ui->pushButtonRestoreDefault->setEnabled(true);

        }// else

    }// else if

}// restoreDefaultStatus

void Preferences::on_pushButtonRestoreDefault_clicked()
{

    if(ui->tabWidget->currentIndex() == 0) {

        setColorEnabled(defaultCellColor[Enabled]);
        setColorDisabled(defaultCellColor[Disabled]);

        restoreDefaultStatus();

    }// if
    else if(ui->tabWidget->currentIndex() == 1) {

        ui->checkBoxLanguage->setChecked(true);

        ui->labelLanguage->setEnabled(false);
        ui->comboBoxLanguage->setEnabled(false);

        setActiveFlag(YUMRepoManager::systemLocale);

        YUMRepoManager::preferredLanguage = "SystemLocale";

        emit languageChangeOcurred(YUMRepoManager::systemLocale);

        restoreDefaultStatus();

    }// else

}// RestoreDefault

void Preferences::on_tabWidget_currentChanged(int index)
{

    Q_UNUSED(index);

    restoreDefaultStatus();

}// tabWidget currentChanged

void Preferences::changeEvent(QEvent *event) {

    if(event->type() == QEvent::LanguageChange) {

        ui->retranslateUi(this);

    }// if

    QDialog::changeEvent(event);

}// changeEvent
