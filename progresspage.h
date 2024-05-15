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

#ifndef PROGRESSPAGE_H
#define PROGRESSPAGE_H

#include "oldpackages.h"
#include "helperthread.h"

#include <QWizardPage>
#include <QProcess>
#include <QTextCursor>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QDir>
#include <QList>

class ProgressPage : public QWizardPage
{

    Q_OBJECT

public:
    explicit ProgressPage(QWidget *parent = 0);

    static QString yumCache;

    static QList<QString> matches;
    static QHash<QString, QString> packages;

    enum kind { FILE, DIRECTORY};

    static bool search(const QString rootDirectory, const QRegExp pattern, kind elementKind);

    void killOldPackagesProcess();
    void killCreaterepoProcess();

    void initializePage();

    bool isComplete() const;

signals:

    void titlesChanged(const QString title, const QString subTitle);
    void progressMessageChanged(const QString message);
    void progressImageChanged(const QString message);
    void logsChanged(const QString message);
    void cursorChanged();
    void processFinished(const QString status);
    void linksFinished();

public slots:
    void disableButton();
    void rpmFolderProcess();
    void yumCacheProcess();
    void yumGlobalCacheProcess();
    void changeTitleAndImage(const QString status);
    void toPrint(const QString &message);

private:

    void deleteOldPackages();
    bool validateTargetPath(const QString targetPath);

    bool removeDirectory(const QString &absolutePath);

    QString getYUMGroups();

    HelperThread *helperThread;

    QProcess *oldPackagesProcess;
    QProcess *createrepoProcess;

    OldPackages *oldPackages;

    QString oldPackagesList;
    QString targetPath;
    QString groupsFileName;
    QString groupsFileDirectory;

    bool enableFinishButton;
    bool groupEnable;

private slots:

    void linkYUMCache();
    void oldPackagesProcessReadyToOutput();
    void createrepoProcessReadyToOutput();

};

#endif // PROGRESSPAGE_H
