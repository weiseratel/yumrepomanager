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

#include "texteditor.h"
#include "ui_texteditor.h"
#include "yumrepomanager.h"

#include <QFile>
#include <QTextStream>
#include <QDebug>

TextEditor::TextEditor(const QString &absoluteFilePath, const QString target, QWidget *parent) : QDialog(parent), ui(new Ui::TextEditor)
{

    ui->setupUi(this);

    this->absoluteFilePath = absoluteFilePath;
    this->target = target;

    QFile myFile(absoluteFilePath);

    myFile.open(QIODevice::ReadOnly | QIODevice::Text);

    QTextStream inFlow(&myFile);

    inFlow.setCodec("UTF-8");

    ui->plainTextEditFileData->setPlainText(inFlow.readAll());

    myFile.close();

    connect(ui->plainTextEditFileData, SIGNAL(textChanged()), SLOT(textChanged()));

}// Constructor

TextEditor::~TextEditor()
{

    delete ui;

}// Destructor

void TextEditor::textChanged() {

    ui->pushButtonSave->setEnabled(true);

}// textChanged

void TextEditor::on_pushButtonSave_clicked()
{

    QFile myFile(absoluteFilePath);

    if(!myFile.open(QIODevice::Truncate | QIODevice::WriteOnly | QIODevice::Text)) {

        YUMRepoManager::showMessage("Error", tr("Access denied when trying to write in config file"));

        return;

    }// if

    QTextStream outFlow(&myFile);

    outFlow.setCodec("UTF-8");

    outFlow << ui->plainTextEditFileData->toPlainText();

    outFlow.flush();

    myFile.close();

    this->accept();

}// Save Clicked

void TextEditor::on_buttonBox_rejected()
{

    this->reject();

}// Cancel Clicked
