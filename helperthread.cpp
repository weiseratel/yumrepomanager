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

#include "helperthread.h"
#include "yumrepomanager.h"
#include "dataflow.h"
#include "createrepowizard.h"
#include "progresspage.h"

#include <QHashIterator>
#include <QRegExp>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDirIterator>
#include <QMessageBox>
#include <QDebug>

HelperThread::HelperThread(const QString &mode, const QString &destination, QObject *parent) : QThread(parent)
{

    this->mode = mode;

    this->destination = destination;

}// Constructor - 1

HelperThread::HelperThread(const QString &mode, QObject *parent) : QThread(parent)
{

    this->mode = mode;

}// Constructor - 2

void HelperThread::run() {

    if(mode == "COPY" || mode == "COPY-REMOVE") {

        bool noneMessage = true;

        if(ProgressPage::search(ProgressPage::yumCache, QRegExp("^*packages$"), ProgressPage::DIRECTORY)) {

            for(int index = 0; index < ProgressPage::matches.size(); ++index) {

                QDir parentDirectory(ProgressPage::matches.at(index));

                parentDirectory.setFilter(QDir::Files | QDir::NoDotAndDotDot);

                QDirIterator parentDirectoryIterator(parentDirectory, QDirIterator::NoIteratorFlags);

                while(parentDirectoryIterator.hasNext()) {

                    parentDirectoryIterator.next();

                    ProgressPage::packages.insert(parentDirectoryIterator.fileInfo().fileName(), ProgressPage::matches.at(index));

                }// while

            }// for

            QHashIterator<QString, QString> packagesIterator(ProgressPage::packages);

            while(packagesIterator.hasNext()) {

                packagesIterator.next();

                QFile myFileSource(packagesIterator.value() + "/" + packagesIterator.key());
                QFile myFileDestination(destination + "/" + packagesIterator.key());

                QFileInfo sourceInfo(packagesIterator.value() + "/" + packagesIterator.key());
                QFileInfo destinationInfo(destination + "/" + packagesIterator.key());
                QFileInfo sourcePath(packagesIterator.value());

                if((sourceInfo == destinationInfo) && sourcePath.isSymLink()) {

                    ProgressPage::packages.remove(packagesIterator.key());

                    continue;

                }// if

                if(myFileDestination.exists()) {

                    myFileDestination.remove();

                }// if

                myFileDestination.close();

                if(myFileSource.copy(destination + "/" + packagesIterator.key())) {

                    emit logsChanged("OK - Copying: " + packagesIterator.value() + "/" + packagesIterator.key());

                    noneMessage = false;

                }// if
                else {

                    emit logsChanged("Failed - Copying: " + packagesIterator.value() + "/" + packagesIterator.key());

                    int optionChoosed = YUMRepoManager::showMessage("Question", tr("Error ocurred when copying package."), tr("Do you want to continue with this operation and check the logs when done?"));

                    if(optionChoosed == QMessageBox::No) {

                        emit changeTitleAndImage("NotOK");

                        return;

                    }// if

                    noneMessage = false;

                }// else

                if(mode == "COPY-REMOVE") {

                    if(myFileSource.remove()) {

                        emit logsChanged(tr("OK - Removing: ") + packagesIterator.value() + "/" + packagesIterator.key());

                    }// if
                    else {

                        emit logsChanged(tr("Failed - Removing: ") + packagesIterator.value() + "/" + packagesIterator.key());

                        int optionChoosed = YUMRepoManager::showMessage("Question", tr("Error ocurred when removing package."), tr("Do you want to continue with this operation and check the logs when done?"));

                        if(optionChoosed == QMessageBox::No) {

                            emit changeTitleAndImage("NotOK");

                            return;

                        }// if

                    }// else

                }// if

                myFileSource.close();

            }// while

            if(noneMessage) {

                emit logsChanged(tr("None"));

            }// if

        }// if
        else {

            emit changeTitleAndImage("NotOK");

        }// else

    }// if

}// run
