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

#include "progresspage.h"
#include "createrepowizard.h"
#include "yumrepomanager.h"

#include <QAbstractButton>
#include <QProcess>
#include <QTimer>
#include <QWizard>
#include <QDebug>
#include <QFile>
#include <QApplication>
#include <QDir>
#include <QDirIterator>
#include <QMessageBox>
#include <QDebug>

QString ProgressPage::yumCache;
QList<QString> ProgressPage::matches;
QHash<QString, QString> ProgressPage::packages;

ProgressPage::ProgressPage(QWidget *parent) : QWizardPage(parent)
{

    yumCache = "/var/cache/yum/";

    groupsFileName = "groups.xml";
    groupsFileDirectory = "groups";

    enableFinishButton = false;
    groupEnable = false;

    oldPackagesProcess = new QProcess(this);
    createrepoProcess = new QProcess(this);

    connect(oldPackagesProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(oldPackagesProcessReadyToOutput()));
    connect(createrepoProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(createrepoProcessReadyToOutput()));
    connect(this, SIGNAL(processFinished(QString)), this, SLOT(changeTitleAndImage(QString)));
    connect(this, SIGNAL(linksFinished()), this, SLOT(rpmFolderProcess()));

}// Constructor

void ProgressPage::initializePage() {

    QTimer::singleShot(0, this, SLOT(disableButton()));

    if(!CreateRepoWizard::rpmFolder.isEmpty()) {

        targetPath = CreateRepoWizard::rpmFolder;

        QTimer::singleShot(0, this, SLOT(rpmFolderProcess()));

    }// if

    if(!CreateRepoWizard::yumCache.isEmpty()) {

        targetPath = CreateRepoWizard::yumCache;

        QTimer::singleShot(0, this, SLOT(yumCacheProcess()));

    }// if

    if(!CreateRepoWizard::yumGlobalCache.isEmpty()) {

        targetPath = CreateRepoWizard::yumGlobalCache;

        QTimer::singleShot(0, this, SLOT(yumGlobalCacheProcess()));

    }// if

}// initializePage

void ProgressPage::rpmFolderProcess() {

    //
    //  Formating the logs
    //

    if(!CreateRepoWizard::rpmFolder.isEmpty() || !CreateRepoWizard::yumCache.isEmpty()) {

        toPrint("");

    }// if

    //
    //  Validating the folder specified by the user.
    //

    toPrint(tr("Validating the folder specified by the user...\n"));

    if(!validateTargetPath(targetPath)) {

        return;

    }// if

    //
    //  Cleaning repository of old versions packages.
    //

    emit progressMessageChanged(tr("Looking for old packages versions..."));

    toPrint(tr("Looking for old packages versions...\n"));

    QString app = "/bin/bash";

    QStringList args;

    args << "-c" << QString("/usr/bin/repomanage -co ./");

    oldPackagesProcess->setWorkingDirectory(targetPath);

    oldPackagesProcess->start(app, args);

    oldPackagesProcess->waitForStarted();

    while(oldPackagesProcess->state() == QProcess::Running) {

        qApp->processEvents();

    }// while

    oldPackagesProcess->waitForFinished();

    if(!oldPackagesList.isEmpty()) {

        if(!YUMRepoManager::automaticallyDeleteOldPackagesVersions) {

            oldPackages = new OldPackages(oldPackagesList, this);

            oldPackages->setModal(true);

            int dialogCode = oldPackages->exec();

            if(dialogCode == QDialog::Accepted) {

                deleteOldPackages();

            }// if

        }// if
        else {

            deleteOldPackages();

        }// else

    }// if
    else {

        toPrint(tr("There is not old packages versions...\n"));

    }// else

    oldPackagesProcess->close();

    //
    //  Trying to create the repository with group support.
    //

    emit progressMessageChanged(tr("Creating repository..."));

    toPrint(tr("Creating repository at: ") + targetPath + "\n");

    args.clear();

    QStringList createrepoArgumentsList = YUMRepoManager::createRepoArgs.split(" ", QString::SkipEmptyParts);

    groupEnable = false;

    int groupIndex = 0;

    for(int index = 0; index < createrepoArgumentsList.size(); ++index) {

        QString argument = createrepoArgumentsList.at(index);

        if(argument == "--groupfile") {

            groupEnable = true;

            groupIndex = index;

            break;

        }// if

    }// for

    if(groupEnable) {

        args << "-c" << QString("/usr/bin/createrepo %1 %2 %3").arg(YUMRepoManager::createRepoArgs).arg(targetPath + "/" + groupsFileDirectory + "/" + groupsFileName).arg(targetPath);

        QFile myFile(targetPath + "/" + groupsFileDirectory + "/" + groupsFileName);

        if(!myFile.open(QIODevice::ReadOnly)) {

            myFile.close();

            //
            //  If the groups.xml file doesn't exist at folder specified by user, we take the system one.
            //

            QString yumGroups = getYUMGroups();

            if(yumGroups.isEmpty()) {

                toPrint(tr("No groups found in the system.") + "\n");

                toPrint(tr("Creating repository without groups support.") + "\n");

                createrepoArgumentsList.removeAt(groupIndex);

                YUMRepoManager::createRepoArgs = createrepoArgumentsList.join(" ");

                args.clear();

                args << "-c" << QString("/usr/bin/createrepo %1 %2").arg(YUMRepoManager::createRepoArgs).arg(targetPath);

            }// if
            else {

                //
                //  Validating the groups directory.
                //

                QDir groupsPath;

                if(!groupsPath.setCurrent(targetPath + "/" + groupsFileDirectory)) {

                    groupsPath.mkdir(targetPath + "/" + groupsFileDirectory);

                }// if

                //
                //  Copying the groups file to the repository directory.
                //

                QFile myFile(yumGroups);

                myFile.copy(targetPath + "/" + groupsFileDirectory + "/" + groupsFileName);

            }// else

        }// if

        myFile.close();

    }// if
    else {

        args << "-c" << QString("/usr/bin/createrepo %1 %2").arg(YUMRepoManager::createRepoArgs).arg(targetPath);

    }// else

    createrepoProcess->start(app, args);

    createrepoProcess->waitForStarted();

    while(createrepoProcess->state() == QProcess::Running) {

        qApp->processEvents();

    }// while

    createrepoProcess->waitForFinished();

    if(createrepoProcess->exitStatus() == createrepoProcess->NormalExit) {

        emit processFinished("OK");

    }// if
    else {

        emit processFinished("NotOK");

    }// else

    createrepoProcess->close();

}// rpmFolderProcess

void ProgressPage::yumCacheProcess() {

    //
    //  Copying packages to the folder specified by the user.
    //

    emit progressMessageChanged(tr("Copying packages..."));

    toPrint(tr("\nCopying packages to: ") + targetPath + "\n");

    helperThread = new HelperThread("COPY", targetPath, this);

    connect(helperThread, SIGNAL(logsChanged(QString)), this, SLOT(toPrint(QString)));
    connect(helperThread, SIGNAL(finished()), this, SLOT(rpmFolderProcess()));

    helperThread->start();

}// yumCacheProcess

void ProgressPage::yumGlobalCacheProcess() {

    //
    //  Copying packages to the folder specified by the user.
    //

    emit progressMessageChanged(tr("Copying packages..."));

    toPrint(tr("\nCopying packages to: ") + targetPath + "\n");

    helperThread = new HelperThread("COPY-REMOVE", targetPath, this);

    connect(helperThread, SIGNAL(logsChanged(QString)), this, SLOT(toPrint(QString)));
    connect(helperThread, SIGNAL(changeTitleAndImage(QString)), this, SLOT(changeTitleAndImage(QString)));
    connect(helperThread, SIGNAL(finished()), this, SLOT(linkYUMCache()));

    helperThread->start();

    helperThread->setPriority(QThread::TimeCriticalPriority);

}// yumGlobalCacheProcess

void ProgressPage::linkYUMCache() {

    //
    //  Deleting the parent folder and linking the cache
    //  to the folder specified by the user.
    //

    toPrint("");

    for(int index = 0; index < matches.size(); ++index) {

        //
        //  Removing the parent folder of packages.
        //

        if(removeDirectory(matches.at(index))) {

            toPrint(tr("OK - Removing parent folder: ") + matches.at(index));

        }// if
        else {

            toPrint(tr("Failed - Removing parent folder: ") + matches.at(index));

            continue;

        }// else

        //
        //  Linking the cache to the folder specified by the user.
        //

        if(QFile::link(targetPath, matches.at(index))) {

            toPrint(tr("OK - Linking: ") + matches.at(index) + " --> " + targetPath + "\n");

        }// if
        else {

            toPrint(tr("Failed - Linking: ") + matches.at(index) + " --> " + targetPath + "\n");

        }// else

    }// for

    emit linksFinished();

}// linkYUMCache

bool ProgressPage::search(const QString rootDirectory, const QRegExp pattern, kind elementKind) {

    //
    //  Cleaning matches
    //

    matches.clear();

    QDir rootPath;

    rootPath.setCurrent(rootDirectory);

    //
    //  Setting the kind of element.
    //

    if(elementKind == FILE) {

        rootPath.setFilter(QDir::Files | QDir::System | QDir::NoDotAndDotDot);

    }// if
    else if(elementKind == DIRECTORY) {

        rootPath.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);

    }// else if

    //
    //  Setting the iterator to list entries inside all subdirectories.
    //

    QDirIterator rootPathIterator(rootPath, QDirIterator::Subdirectories | QDirIterator::FollowSymlinks);

    while(rootPathIterator.hasNext()) {

        rootPathIterator.next();

        if(!rootPathIterator.fileName().contains(pattern)) {

            continue;

        }// if

        matches.append(rootPathIterator.fileInfo().absoluteFilePath());

    }// while

    if(matches.isEmpty()) {

        return false;

    }// if

    return true;

}// search

bool ProgressPage::removeDirectory(const QString &absolutePath) {

    bool OK = false;

    QDir myDirectory(absolutePath);
    QFileInfo absolutePathInfo(absolutePath);

    if(myDirectory.rmdir(absolutePath)) {

        OK = true;

        return OK;

    }// if
    else if(absolutePathInfo.isSymLink()) {

        if(QFile::remove(absolutePath)) {

            OK = true;

        }// if
        else {

            OK = false;

        }// else

    }// else if
    else {

        OK = false;

        myDirectory.setFilter(QDir::AllEntries | QDir::System | QDir::Hidden | QDir::NoDotAndDotDot);

        QDirIterator myIterator(myDirectory, QDirIterator::Subdirectories);

        while(myIterator.hasNext()) {

            myIterator.next();

            if(myIterator.fileInfo().isDir()) {

                if(myDirectory.rmdir(myIterator.fileInfo().absoluteFilePath())) {

                    OK = true;

                }// if
                else {

                    OK = false;

                    if(removeDirectory(myIterator.fileInfo().absoluteFilePath())) {

                        OK = true;

                    }// if

                }// else

                continue;

            }// if

            if(QFile::remove(myIterator.fileInfo().absoluteFilePath())) {

                OK = true;

            }// if
            else {

                OK = false;

            }// else

        }// while

        if(OK) {

            if(myDirectory.rmdir(absolutePath)) {

                OK = true;

            }// if
            else if(absolutePathInfo.isSymLink()) {

                if(QFile::remove(absolutePath)) {

                    OK = true;

                }// if
                else {

                    OK = false;

                }// else

            }// else if
            else {

                OK = false;

            }// else

        }// if

    }// else

    return OK;

}// removeDirectory

QString ProgressPage::getYUMGroups() {

    if(search(yumCache, QRegExp("^*.xml$"), FILE)) {

        QList<QString> matchesRightFormat;

        int fileSize = 0;

        //
        //  Getting the right ones
        //

        for(int index = 0; index < matches.size(); ++index) {

            QFile myFile(matches.at(index));

            if(!myFile.open(QIODevice::ReadOnly | QIODevice::Text)) {

                myFile.close();

                return QString("");

            }// if

            QDomDocument document;

            document.setContent(&myFile);

            if(document.firstChildElement().nodeName() == "comps") {

                matchesRightFormat.append(matches.at(index));

            }// if

            myFile.close();

        }// for

        if(matchesRightFormat.isEmpty()) {

            return QString("");

        }// if

        //
        //  Getting the most big.
        //

        QString yumGroups;

        for(int index = 0; index < matchesRightFormat.size(); ++index) {

            QFile myFile(matchesRightFormat.at(index));

            if(fileSize < myFile.size()) {

                fileSize = myFile.size();

                yumGroups = matchesRightFormat.at(index);

            }// if

            myFile.close();

        }// while

        matches.clear();

        return yumGroups;

    }// if

    return QString("");

}// getYUMGroups

void ProgressPage::oldPackagesProcessReadyToOutput() {

    oldPackagesProcess->waitForBytesWritten();

    oldPackagesList.append(oldPackagesProcess->readAllStandardOutput().constData());

}// oldPackagesProcessReadyToOutput

void ProgressPage::createrepoProcessReadyToOutput() {

    createrepoProcess->waitForBytesWritten();

    QString standardOutput = createrepoProcess->readAllStandardOutput().constData();

    emit logsChanged(standardOutput);

    emit cursorChanged();

}// createrepoProcessReadyToOutput

void ProgressPage::toPrint(const QString &message) {

    emit logsChanged(message);
    emit logsChanged("\n");

    emit cursorChanged();

}// toPrint

void ProgressPage::deleteOldPackages() {

    QStringList packagesToDelete = oldPackagesList.split("\n");

    for(int index = 0; index < (packagesToDelete.size() - 1); ++index) {

        qApp->processEvents();

        QFile myFile(oldPackagesProcess->workingDirectory() + "/" + packagesToDelete.at(index));

        if(!myFile.open(QIODevice::WriteOnly)) {

            toPrint(tr("ERROR: while trying to access old package version: %1/%2\n").arg(oldPackagesProcess->workingDirectory()).arg(packagesToDelete.at(index)));

        }// if

        if(!myFile.remove()) {

            toPrint(tr("Failed - Removing old package version: %1/%2\n").arg(oldPackagesProcess->workingDirectory()).arg(packagesToDelete.at(index)));

        }// if
        else {

            toPrint(tr("OK - Removing old package version: %1/%2\n").arg(oldPackagesProcess->workingDirectory()).arg(packagesToDelete.at(index)));

        }

        myFile.close();

        emit cursorChanged();

    }// for

    toPrint("");

}// deleteOldPackages

void ProgressPage::changeTitleAndImage(const QString status) {

    if(status == "OK") {

        emit titlesChanged(tr("Complete"), tr("All operations complited successfully"));

        emit progressMessageChanged(tr("Done!"));

        emit progressImageChanged(":/Icons/Images/OK.png");

        enableFinishButton = true;

        emit completeChanged();

    }// if
    else if(status == "NotOK") {

        emit titlesChanged(tr("Not complete"), tr("An error has ocurred, operations not complited"));

        emit progressMessageChanged("Error!");

        emit progressImageChanged(":/Icons/Images/Failed.png");

        enableFinishButton = true;

        emit completeChanged();

    }// else if

}// changeTitleAndImage

bool ProgressPage::validateTargetPath(const QString targetPath) {

    QDir rootPath = QDir();

    rootPath.setCurrent(targetPath);

    rootPath.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);

    QDirIterator rootPathIterator(rootPath, QDirIterator::NoIteratorFlags);

    QStringList packages;

    while(rootPathIterator.hasNext()) {

        rootPathIterator.next();

        if(!rootPathIterator.fileName().contains(QRegExp("^*.rpm$")))
                continue;

        packages.append(rootPathIterator.fileName());

    }// while

    if(packages.isEmpty()) {

        emit processFinished("NotOK");

        YUMRepoManager::showMessage("Error", tr("No packages found at the given direcory."));

        return false;

    }// if

    return true;

}// validateTargetPath

void ProgressPage::killOldPackagesProcess() {

    oldPackagesProcess->kill();

    //qDebug() << "The pid is: " << oldPackagesProcess->pid();

}// killOldPackagesProcess

void ProgressPage::killCreaterepoProcess() {

    createrepoProcess->kill();

}// killCreaterepoProcess

bool ProgressPage::isComplete() const {

    if(enableFinishButton) {

        return true;

    }// if

    return false;

}// isComplete

void ProgressPage::disableButton() {

    wizard()->button(QWizard::FinishButton)->setEnabled(false);
    wizard()->button(QWizard::CancelButton)->setEnabled(false);

}// disableButton
