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

#include "createrepoarguments.h"
#include "ui_createrepoarguments.h"

#include "yumrepomanager.h"

#include <QFileDialog>
#include <QDebug>

CreaterepoArguments::CreaterepoArguments(QWidget *parent) : QDialog(parent), ui(new Ui::CreaterepoArguments)
{

    ui->setupUi(this);

    loadArguments();

    //
    //  ToolTip
    //

    ui->checkBoxNoDatabase->setToolTip("Do not generate sqlite databases in the repository.");
    ui->checkBoxDeltas->setToolTip("Tells createrepo to generate deltarpms and the delta metadata.");
    ui->checkBoxUpdate->setToolTip("<p>If metadata already exists in the outputdir and an rpm is unchanged (based on file size and mtime) since the metadata was generated,  reuse"
                                   "the existing metadata rather than recalculating it. In the case of a large repository with only a few new or modified rpms this can"
                                   "significantly reduce I/O and processing time.</p>");
    ui->checkBoxIgnoreSymlinks->setToolTip("Ignore symlinks of packages");
    ui->checkBoxIncludeGroups->setToolTip("Include groups information.");
    ui->checkBoxCheckTimestamps->setToolTip("<p>Don't generate repo metadata, if their timestamps are newer than its rpms.</p>");
    ui->checkBoxXZCompression->setToolTip("Use xz for repodata compression.");
    ui->checkBoxPrettyFormat->setToolTip("Output xml files in pretty format.");

    ui->labelWorkers->setToolTip("Number of workers to spawn to read rpms.");

    ui->labelCacheDir->setToolTip("<p>Specify a directory to use as a cachedir. This allows createrepo to create a cache of checksums of packages in the repository. In  consecutive"
                                     "runs  of  createrepo over the same repository of files that do not have a complete change out of all packages this decreases the"
                                     "processing time dramatically.</p>");

    //
    //  WhatsThis
    //

    ui->checkBoxNoDatabase->setWhatsThis("Do not generate sqlite databases in the repository.");
    ui->checkBoxDeltas->setWhatsThis("Tells createrepo to generate deltarpms and the delta metadata.");
    ui->checkBoxUpdate->setWhatsThis("<p>If metadata already exists in the outputdir and an rpm is unchanged (based on file size and mtime) since the metadata was generated,  reuse"
                                   "the existing metadata rather than recalculating it. In the case of a large repository with only a few new or modified rpms this can"
                                   "significantly reduce I/O and processing time.</p>");
    ui->checkBoxIgnoreSymlinks->setWhatsThis("Ignore symlinks of packages");
    ui->checkBoxIncludeGroups->setWhatsThis("Include groups information.");
    ui->checkBoxCheckTimestamps->setWhatsThis("<p>Don't generate repo metadata, if their timestamps are newer than its rpms.</p>");
    ui->checkBoxXZCompression->setWhatsThis("Use xz for repodata compression.");
    ui->checkBoxPrettyFormat->setWhatsThis("Output xml files in pretty format.");

    ui->labelWorkers->setWhatsThis("Number of workers to spawn to read rpms.");

    ui->labelCacheDir->setWhatsThis("<p>Specify a directory to use as a cachedir. This allows createrepo to create a cache of checksums of packages in the repository. In  consecutive"
                                     "runs  of  createrepo over the same repository of files that do not have a complete change out of all packages this decreases the"
                                     "processing time dramatically.</p>");

}// Constructor

CreaterepoArguments::~CreaterepoArguments()
{

    delete ui;

}// Destructor

void CreaterepoArguments::loadArguments() {

    QStringList rawArguments = YUMRepoManager::createRepoArgs.split(QRegExp(" "), QString::SkipEmptyParts);

    for(int count = 0; count < rawArguments.size(); ++count) {

        if(rawArguments.at(count) == "--cachedir") {

            ui->lineEditCacheDir->setText(rawArguments.at(count + 1));

            count++;

        }// if
        else if(rawArguments.at(count) == "--checkts") {

            ui->checkBoxCheckTimestamps->setChecked(true);

        }// else if
        else if(rawArguments.at(count) == "--no-database") {

            ui->checkBoxNoDatabase->setChecked(true);

        }// else if
        else if(rawArguments.at(count) == "--deltas") {

            ui->checkBoxDeltas->setChecked(true);

        }// else if
        else if(rawArguments.at(count) == "--groupfile") {

            ui->checkBoxIncludeGroups->setChecked(true);

        }// else if
        else if(rawArguments.at(count) == "--skip-symlinks") {

            ui->checkBoxIgnoreSymlinks->setChecked(true);

        }// else if
        else if(rawArguments.at(count) == "--update") {

            ui->checkBoxUpdate->setChecked(true);

        }// else if
        else if(rawArguments.at(count) == "--xz") {

            ui->checkBoxXZCompression->setChecked(true);

        }// else if
        else if(rawArguments.at(count) == "--pretty") {

            ui->checkBoxPrettyFormat->setChecked(true);

        }// else if
        else if(rawArguments.at(count) == "--workers") {

            ui->spinBoxWorkers->setValue(rawArguments.at(count + 1).toInt());

            count++;

        }// else if

    }// foreach

    restoreDefault();

}// Load Arguments

void CreaterepoArguments::restoreDefault() {

    bool state = false;

    //
    // Database
    //

    if(ui->checkBoxNoDatabase->isChecked()) {

        state = true;

    }// if

    //
    // Deltas
    //

    if(ui->checkBoxDeltas->isChecked()) {

        state = true;

    }// if

    //
    // Check Timestamp
    //

    if(ui->checkBoxCheckTimestamps->isChecked()) {

        state = true;

    }// if

    //
    // Ignore Symlinks
    //

    if(ui->checkBoxIgnoreSymlinks->isChecked()) {

        state = true;

    }// if

    //
    // Update
    //

    if(!ui->checkBoxUpdate->isChecked()) {

        state = true;

    }// if

    //
    // XZ Compression
    //

    if(ui->checkBoxXZCompression->isChecked()) {

        state = true;

    }// if

    //
    // Pretty Format
    //

    if(!ui->checkBoxPrettyFormat->isChecked()) {

        state = true;

    }// if

    //
    // Cache Dir
    //

    if(!ui->lineEditCacheDir->text().isEmpty()) {

        if( (ui->lineEditCacheDir->text() != "/tmp") && (ui->lineEditCacheDir->text() != "/tmp/")) {

            state = true;

        }// if

    }// if
    else {

        state = true;

    }// else

    //
    // Workers
    //

    if(ui->spinBoxWorkers->value() != 2) {

        state = true;

    }// if

    //
    // Groups
    //

    if(!ui->checkBoxIncludeGroups->isChecked()) {

        state = true;

    }// if

    if(state) {

        ui->pushButtonRestoreDefault->setEnabled(true);

    }// if
    else {

        ui->pushButtonRestoreDefault->setEnabled(false);

    }// else

}// Restore Default

void CreaterepoArguments::on_buttonBox_accepted()
{

    YUMRepoManager::createRepoArgs.clear();

    //
    // Checksum
    //

    YUMRepoManager::createRepoArgs.append("--checksum sha256 ");

    //
    // No Database
    //

    if(ui->checkBoxNoDatabase->isChecked()) {

        YUMRepoManager::createRepoArgs.append("--no-database ");

    }// if

    //
    // Deltas
    //

    if(ui->checkBoxDeltas->isChecked()) {

        YUMRepoManager::createRepoArgs.append("--deltas ");

    }// if

    //
    // Check Timestamp
    //

    if(ui->checkBoxCheckTimestamps->isChecked()) {

        YUMRepoManager::createRepoArgs.append("--checkts ");

    }// if

    //
    // Ignore Symlinks
    //

    if(ui->checkBoxIgnoreSymlinks->isChecked()) {

        YUMRepoManager::createRepoArgs.append("--skip-symlinks ");

    }// if

    //
    // XZ Compression
    //

    if(ui->checkBoxXZCompression->isChecked()) {

        YUMRepoManager::createRepoArgs.append("--xz ");

    }// if

    //
    // Pretty Format
    //

    if(ui->checkBoxPrettyFormat->isChecked()) {

        YUMRepoManager::createRepoArgs.append("--pretty ");

    }// if

    //
    // Include the file's checksum in the metadata filename, helps HTTP caching
    //

    YUMRepoManager::createRepoArgs.append(QString("--unique-md-filenames "));

    //
    // Update
    //

    if(ui->checkBoxUpdate->isChecked()) {

        YUMRepoManager::createRepoArgs.append("--update ");

    }// if

    //
    // Workers
    //

    YUMRepoManager::createRepoArgs.append(QString(QString("--workers %1 ").arg(ui->spinBoxWorkers->value())));

    //
    // Cache Dir
    //

    if(!ui->lineEditCacheDir->text().isEmpty()) {

        YUMRepoManager::createRepoArgs.append(QString("--cachedir %1 ").arg(ui->lineEditCacheDir->text()));

    }// if
    else {

        YUMRepoManager::createRepoArgs.append(QString("--cachedir /tmp "));

    }// else

    //
    // Groups
    //

    if(ui->checkBoxIncludeGroups->isChecked()) {

        YUMRepoManager::createRepoArgs.append("--groupfile ");

    }// if

    this->accept();

}// ButtonBox Accepted

void CreaterepoArguments::on_buttonBox_rejected()
{

    this->reject();

}// ButtonBox Rejected

void CreaterepoArguments::on_toolButton_clicked()
{

    QString cacheDir = QFileDialog::getExistingDirectory(this, tr("Select The Cache's Directory"), "/tmp");

    if (!cacheDir.isEmpty()) {

      ui->lineEditCacheDir->setText(cacheDir);

    }//if

}// ToolButton Clicked

void CreaterepoArguments::on_pushButtonRestoreDefault_clicked()
{

    ui->checkBoxNoDatabase->setChecked(false);
    ui->checkBoxUpdate->setChecked(true);
    ui->checkBoxIncludeGroups->setChecked(true);

    ui->checkBoxCheckTimestamps->setChecked(false);
    ui->checkBoxDeltas->setChecked(false);
    ui->checkBoxIgnoreSymlinks->setChecked(false);
    ui->checkBoxXZCompression->setChecked(false);
    ui->checkBoxPrettyFormat->setChecked(true);

    ui->spinBoxWorkers->setValue(2);

    ui->lineEditCacheDir->setText("/tmp");

    restoreDefault();

}// PushButton Restore Default Clicked

void CreaterepoArguments::on_checkBoxNoDatabase_clicked()
{

    restoreDefault();

}// CheckBox NoDatabase Clicked

void CreaterepoArguments::on_checkBoxCheckTimestamps_clicked()
{

    restoreDefault();

}// CheckBox CheckTimestamps Clicked

void CreaterepoArguments::on_checkBoxUpdate_clicked()
{

    restoreDefault();

}// CheckBox Update Clicked

void CreaterepoArguments::on_checkBoxIncludeGroups_clicked()
{

    restoreDefault();

}// CheckBox Include Groups Clicked

void CreaterepoArguments::on_checkBoxDeltas_clicked()
{

    restoreDefault();

}// CheckBox Deltas Clicked

void CreaterepoArguments::on_checkBoxIgnoreSymlinks_clicked()
{

    restoreDefault();

}// CheckBox IgnoreSymlinks Clicked

void CreaterepoArguments::on_checkBoxXZCompression_clicked()
{

    restoreDefault();

}// CheckBox XZ Compression Clicked

void CreaterepoArguments::on_checkBoxPrettyFormat_clicked()
{

    restoreDefault();

}// CheckBox Pretty Format Clicked

void CreaterepoArguments::on_spinBoxWorkers_valueChanged(int arg1)
{

    Q_UNUSED(arg1);

    restoreDefault();

}// SpinBox Workers Value Changed

void CreaterepoArguments::on_lineEditCacheDir_textChanged(const QString &arg1)
{

    Q_UNUSED(arg1);

    restoreDefault();

}// LineEdit CacheDir Text Changed
