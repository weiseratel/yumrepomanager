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

#include "dataflow.h"
#include "yumrepomanager.h"

#include <QString>
#include <QDir>
#include <QDirIterator>
#include <QMessageBox>
#include <QTextStream>
#include <QHash>
#include <QDebug>

QString DataFlow::repoPath;
QString DataFlow::yumConf;
QString DataFlow::protectedPath;

QMap<QString, QString> DataFlow::yumConfData;
QList<RepoData> DataFlow::repoDataList;
QList<PluginData> DataFlow::pluginDataList;
YUMPluginModel *DataFlow::pluginModel = new YUMPluginModel;
QStringListModel *DataFlow::protectedListModel = new QStringListModel;

DataFlow::DataFlow(QObject *parent) : QObject(parent)
{

    repoKeyList << "name" << "failovermethod" << "skip_if_unavailable" << "assumeyes" << "skip_broken" << "timeout"
                << "baseurl" << "metalink" << "proxy" << "proxy_username" << "proxy_password" << "throttle" << "reposdir"
                << "enabled" << "metadata_expire" << "gpgcheck" << "gpgkey" << "keepcache" << "cachedir" << "debuglevel"
                << "logfile" << "exactarch" << "obsoletes" << "retries" << "plugins" << "errorlevel" << "installonly_limit"
                << "exclude" << "tolerant" << "protected_multilib" << "clean_requirements_on_remove" << "includepkgs" << "color"
                << "async" << "assumeno" << "diskspacecheck" << "http_caching" << "max_connections" << "installonlypkgs" << "ip_resolve"
                << "upgrade_requirements_on_install" << "recheck_installed_requires" << "mirrorlist";

    repoPath = "/etc/yum.repos.d/";

    yumConf = "/etc/yum.conf";

    pluginPath = "/etc/yum/pluginconf.d/";

    protectedPath = "/etc/yum/protected.d/";

}// Constructor

//
// Locate configuration files.
//

QStringList DataFlow::locateFiles(const QString path, const QString fileExtension, FileFormat fileFormat) {

    QDir rootPath = QDir();

    if(!rootPath.setCurrent(path)) {

        YUMRepoManager::showMessage("Error", tr("The given path doesn't exist."));

        exit(1);

    }// if

    rootPath.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);

    QDirIterator rootPathIterator(rootPath, QDirIterator::NoIteratorFlags);

    QStringList files;

    if(fileFormat == AbsoluteFileName) {

        while(rootPathIterator.hasNext()) {

            rootPathIterator.next();

            if(!rootPathIterator.fileName().contains(QRegExp(QString("^%1$").arg(fileExtension))))
                continue;

            files.append(rootPath.absolutePath() + "/" + rootPathIterator.fileName());

        }// while

    }// if
    else if(fileFormat == FileName) {

        while(rootPathIterator.hasNext()) {

            rootPathIterator.next();

            if(!rootPathIterator.fileName().contains(QRegExp(QString("^%1$").arg(fileExtension))))
                continue;

            files.append(rootPathIterator.fileName());

        }// while

    }// else if

    return files;

}// locateRepoData

//
// Read YUM configuration files.
//

void DataFlow::fillYumConfData() {

    QFile myFile(yumConf);

    if(!myFile.open(QIODevice::ReadOnly | QIODevice::Text)) {

        YUMRepoManager::showMessage("Error", tr("Access deny when trying to open: %1").arg(yumConf));

    }// if

    QTextStream inFlow(&myFile);

    inFlow.setCodec("UTF-8");

    while(!inFlow.atEnd()) {

        QString yumConfLine = inFlow.readLine();

        if(yumConfLine.isEmpty() || yumConfLine.contains(QRegExp("^\\s+$"))) {

            if(inFlow.atEnd() && yumConfData.isEmpty()) {

                YUMRepoManager::showMessage("Error", tr("The Yum configuration file is empty"));

                break;

            }// if

            continue;

        }// if

        //
        // Validating and reading YUM configuration file.
        //

        if(yumConfLine.contains("=") && ( (!yumConfLine.contains(QRegExp("^\\s+=\\s+$"))) && (!yumConfLine.startsWith("=")) && (!yumConfLine.endsWith("=")) )) {

            int Flag = 0;
            int lineSize = yumConfLine.length();
            QString Key = "";
            QString Value = "";

            // Cheking if the line starts and ends with "[" and "]".

            if(yumConfLine.startsWith("[") && yumConfLine.endsWith("]")) {

                // Extracting the word between the "[" and "]" characters and saving it as the repository's name.

                QString repoName = yumConfLine.mid(1, (yumConfLine.length() - 2));

                if(repoName != "main") {

                    break;

                }// if

                // Cheking if this's not the end of the file.

                if(!inFlow.atEnd()) {

                    continue;

                }// if

            }// if

            for(int index = 0; index < lineSize; ++index) {

                if(yumConfLine[index].isSpace() && Flag == 0)
                    continue;

                if(yumConfLine[index] == '=' && Flag == 0) {

                    Flag = 1;

                    continue;

                }// if

                if(Flag == 0)
                    Key.append(yumConfLine[index]);

                if(Flag == 1)
                    Value.append(yumConfLine[index]);

            }// for

            if(Key.startsWith("#")) {

                Flag = 1;

                Key = Key.mid(1, (Key.length() - 1));

            }// if
            else
                Flag = 0;

            if(!repoKeyList.contains(Key)) {

                YUMRepoManager::showMessage("Error", tr("Option: \"%1=%2\", unknown or doesn't managed by this program").arg(Key).arg(Value));

                continue;

            }// if

            if(Flag == 1) {

                Key = "#" + Key;

            }// if

            if(Key == "reposdir") {

                if(Value.endsWith("/"))
                    repoPath = Value;
                else
                    repoPath = Value + "/";

            }// if

            yumConfData.insert(Key, Value);

            continue;

        }// if

    }// while

    inFlow.flush();
    myFile.close();

}// fillYumConfData

//
// Read repository configuration files.
//

void DataFlow::fillRepoDataList() {

    QStringList repoFiles = locateFiles(repoPath, "*.repo", DataFlow::AbsoluteFileName);

    // Loop for iterating through all repository files.

    for(int index = 0; index < repoFiles.size(); ++index) {

        QString item = repoFiles.at(index);

        QFile myFile(item);

        // Trying to open the file for read only.

        if(!myFile.open(QIODevice::ReadOnly | QIODevice::Text)) {

            YUMRepoManager::showMessage("Error", tr("When trying to open: %1").arg(item));

            continue;

        }// if

        // Creating the interface to the file.

        QTextStream inFlow(&myFile);

        inFlow.setCodec("UTF-8");

        // Creating a RepoData object when the repository's data will be stored.

        RepoData node;

        QHash<QString, QString> alreadyStored;

        // Loop for iterating through all the lines of the file.

        while(!inFlow.atEnd()) {

            // Reading line

            QString repoLine = inFlow.readLine();

            // Cheking if the line is empty or it just contains blank spaces.

            if(repoLine.isEmpty() || repoLine.contains(QRegExp("^\\s+$"))) {

                if(inFlow.atEnd() && node.repoName.isEmpty() && (node.repoBaseURL[0].isEmpty() || node.repoMetaLink[0].isEmpty())){

                    YUMRepoManager::showMessage("Error", tr("Empty Repository file: %1").arg(item));

                    break;

                }// if

                // Cheking if it's the last line.

                if(inFlow.atEnd()) {

                    setDefaultValues(node);

                    // This function validate the node looking for duplicated entries, if everything is ok the node will be inserted.

                    insertNewNode(node);

                    alreadyStored.clear();

                }

                continue;

            }// if

            // Storing the absolute path and file name of the file.

            node.repoFileName = item;

            // Cheking if the line has the required format.

            if((repoLine.contains("=") || (repoLine.startsWith("[") && repoLine.endsWith("]")) ) && ( (!repoLine.contains(QRegExp("\\s+=\\s"))) && (!repoLine.startsWith("=")) && (!repoLine.endsWith("=")) )) {

                int Flag = 0;
                int lineSize = repoLine.length();
                QString Key = "";
                QString Value = "";

                // Cheking if the line starts and ends with "[" and "]".

                if(repoLine.startsWith("[") && repoLine.endsWith("]")) {

                    // Cheking if this is the start of another repository declatation.

                    if(!node.repoName.isEmpty()) {

                        setDefaultValues(node);

                        // This function validate the node looking for duplicated entries, if everything is ok the node will be inserted.

                        insertNewNode(node);

                        alreadyStored.clear();

                        node.repoEnabled[0].clear();
                        node.repoEnabled[1].clear();

                        node.repoName.clear();

                        node.repoComment[0].clear();
                        node.repoComment[1].clear();

                        node.repoBaseURL[0].clear();
                        node.repoBaseURL[1].clear();
                        node.repoBaseURL[2].clear();

                        node.repoMetaLink[0].clear();
                        node.repoMetaLink[1].clear();
                        node.repoMetaLink[2].clear();

                        node.repoGPGCheck[0].clear();
                        node.repoGPGCheck[1].clear();
                        node.repoGPGCheck[2].clear();

                        node.repoGPGKey[0].clear();
                        node.repoGPGKey[1].clear();
                        node.repoGPGKey[2].clear();

                        node.repoProxy[0].clear();
                        node.repoProxy[1].clear();
                        node.repoProxy[2].clear();

                        node.repoProxyUsername[0].clear();
                        node.repoProxyUsername[1].clear();
                        node.repoProxyUsername[2].clear();

                        node.repoProxyPassword[0].clear();
                        node.repoProxyPassword[1].clear();
                        node.repoProxyPassword[2].clear();

                        node.repoFailoverMethod[0].clear();
                        node.repoFailoverMethod[1].clear();
                        node.repoFailoverMethod[2].clear();

                        node.repoSkipIfUnavailable[0].clear();
                        node.repoSkipIfUnavailable[1].clear();
                        node.repoSkipIfUnavailable[2].clear();

                        node.repoBandwidth[0].clear();
                        node.repoBandwidth[1].clear();
                        node.repoBandwidth[2].clear();

                        node.repoMetadataExpire[0].clear();
                        node.repoMetadataExpire[1].clear();
                        node.repoMetadataExpire[2].clear();

                        node.repoParallelDownload[0].clear();
                        node.repoParallelDownload[1].clear();
                        node.repoParallelDownload[2].clear();

                        node.repoInclude[0].clear();
                        node.repoInclude[1].clear();
                        node.repoInclude[2].clear();

                        node.repoExclude[0].clear();
                        node.repoExclude[1].clear();
                        node.repoExclude[2].clear();

                    }// if

                    // Extracting the word between the "[" and "]" characters and saving it as the repository's name.

                    node.repoName = repoLine.mid(1, (repoLine.length() - 2));

                    // Cheking if this's not the end of the file.

                    if(!inFlow.atEnd()) {

                        continue;

                    }// if
                    else {

                        YUMRepoManager::showMessage("Error", tr("Syntax error in repository file: %1").arg(node.repoFileName));

                        break;

                    }// else

                }// if

                // Loop for split the line and get the Key and Value.

                for(int count = 0; count < lineSize; count++) {

                    if(repoLine[count].isSpace() && Flag == 0) {

                        continue;

                    }// if

                    if(repoLine[count] == '=' && Flag == 0) {

                        Flag = 1;

                        continue;

                    }// if

                    if(Flag == 0)
                        Key.append(repoLine[count]);

                    if(Flag == 1)
                        Value.append(repoLine[count]);

                }// for

                // Cheking if the line is commented.

                if(Key.startsWith("#")) {

                    Flag = 1;

                    Key = Key.mid(1, (Key.length() - 1));

                }// if
                else
                    Flag = 0;

                // Cheking if the Key is one of what this program manage.

                if(!repoKeyList.contains(Key)) {

                    YUMRepoManager::showMessage("Warning", tr("Unknow or obsolete option \"%1\"in repository file: %2").arg(Key).arg(node.repoFileName));

                    continue;

                }// if

                // In this part starts the storing of the Keys and its values.

                if(Key == "name") {

                    node.repoComment[0] = Key;
                    node.repoComment[1] = Value;

                }// if
                else if(Key == "failovermethod") {

                    if(!alreadyStored.contains("failovermethod")) {

                        if(Flag == 1) {

                            node.repoFailoverMethod[0] = "0";

                            alreadyStored.insert(Key, "0");

                        }// if
                        else {

                            node.repoFailoverMethod[0] = "1";

                            alreadyStored.insert(Key, "1");

                        }// else

                        node.repoFailoverMethod[1] = Key;
                        node.repoFailoverMethod[2] = Value;

                    }// if
                    else {

                        if(alreadyStored.value("failovermethod") == "1") {

                            YUMRepoManager::showMessage("Warning", tr("Option \"%1\" duplicated in repository file: %2").arg(Key).arg(node.repoFileName));

                        }// if
                        else {

                            if(Flag == 1) {

                                node.repoFailoverMethod[0] = "0";

                                alreadyStored.remove(Key);

                                alreadyStored.insert(Key, "0");

                            }// if
                            else {

                                node.repoFailoverMethod[0] = "1";

                                alreadyStored.remove(Key);

                                alreadyStored.insert(Key, "1");

                            }// else

                            node.repoFailoverMethod[1] = Key;
                            node.repoFailoverMethod[2] = Value;

                        }// else

                    }// else

                }// else if
                else if(Key == "skip_if_unavailable") {

                    if(!alreadyStored.contains("skip_if_unavailable")) {

                        if(Flag == 1) {

                            node.repoSkipIfUnavailable[0] = "0";

                            alreadyStored.insert(Key, "0");

                        }// if
                        else {

                            node.repoSkipIfUnavailable[0] = "1";

                            alreadyStored.insert(Key, "1");

                        }// else

                        node.repoSkipIfUnavailable[1] = Key;
                        node.repoSkipIfUnavailable[2] = Value;

                    }// if
                    else {

                        if(alreadyStored.value("skip_if_unavailable") == "1") {

                            YUMRepoManager::showMessage("Warning", tr("Option \"%1\" duplicated in repository file: %2").arg(Key).arg(node.repoFileName));

                        }// if
                        else {

                            if(Flag == 1) {

                                node.repoSkipIfUnavailable[0] = "0";

                                alreadyStored.remove(Key);

                                alreadyStored.insert(Key, "0");

                            }// if
                            else {

                                node.repoSkipIfUnavailable[0] = "1";

                                alreadyStored.remove(Key);

                                alreadyStored.insert(Key, "1");

                            }// else

                            node.repoSkipIfUnavailable[1] = Key;
                            node.repoSkipIfUnavailable[2] = Value;

                        }// else

                    }// else

                }// else if
                else if(Key == "baseurl") {

                    if(!alreadyStored.contains("baseurl")) {

                        if(Flag == 1) {

                            node.repoBaseURL[0] = "0";

                            alreadyStored.insert(Key, "0");

                        }// if
                        else {

                            node.repoBaseURL[0] = "1";

                            alreadyStored.insert(Key, "1");

                        }// else

                        node.repoBaseURL[1] = Key;
                        node.repoBaseURL[2] = Value;

                    }// if
                    else {

                        if(alreadyStored.value("baseurl") == "1") {

                            YUMRepoManager::showMessage("Warning", tr("Option \"%1\" duplicated in repository file: %2").arg(Key).arg(node.repoFileName));

                        }// if
                        else {

                            if(Flag == 1) {

                                node.repoBaseURL[0] = "0";

                                alreadyStored.remove(Key);

                                alreadyStored.insert(Key, "0");

                            }// if
                            else {

                                node.repoBaseURL[0] = "1";

                                alreadyStored.remove(Key);

                                alreadyStored.insert(Key, "1");

                            }// else

                            node.repoBaseURL[1] = Key;
                            node.repoBaseURL[2] = Value;

                        }// else

                    }// else

                }// else if
                else if((Key == "metalink") || (Key == "mirrorlist")) {

                    if(!alreadyStored.contains("metalink")) {

                        if(Flag == 1) {

                            node.repoMetaLink[0] = "0";

                            alreadyStored.insert(Key, "0");

                        }// if
                        else {

                            node.repoMetaLink[0] = "1";

                            alreadyStored.insert(Key, "1");

                        }// else

                        node.repoMetaLink[1] = Key;
                        node.repoMetaLink[2] = Value;

                    }// if
                    else {

                        if(alreadyStored.value("metalink") == "1") {

                            YUMRepoManager::showMessage("Warning", tr("Option \"%1\" duplicated in repository file: %2").arg(Key).arg(node.repoFileName));

                        }// if
                        else {

                            if(Flag == 1) {

                                node.repoMetaLink[0] = "0";

                                alreadyStored.remove(Key);

                                alreadyStored.insert(Key, "0");

                            }// if
                            else {

                                node.repoMetaLink[0] = "1";

                                alreadyStored.remove(Key);

                                alreadyStored.insert(Key, "1");

                            }// else

                            node.repoMetaLink[1] = Key;
                            node.repoMetaLink[2] = Value;

                        }// else

                    }// else

                }// else if
                else if(Key == "proxy") {

                    if(!alreadyStored.contains("proxy")) {

                        if(Flag == 1) {

                            node.repoProxy[0] = "0";

                            alreadyStored.insert(Key, "0");

                        }// if
                        else {

                            node.repoProxy[0] = "1";

                            alreadyStored.insert(Key, "1");

                        }// else

                        node.repoProxy[1] = Key;
                        node.repoProxy[2] = Value;

                    }// if
                    else {

                        if(alreadyStored.value("proxy") == "1") {

                            YUMRepoManager::showMessage("Warning", tr("Option \"%1\" duplicated in repository file: %2").arg(Key).arg(node.repoFileName));

                        }// if
                        else {

                            if(Flag == 1) {

                                node.repoProxy[0] = "0";

                                alreadyStored.remove(Key);

                                alreadyStored.insert(Key, "0");

                            }// if
                            else {

                                node.repoProxy[0] = "1";

                                alreadyStored.remove(Key);

                                alreadyStored.insert(Key, "1");

                            }// else

                            node.repoProxy[1] = Key;
                            node.repoProxy[2] = Value;

                        }// else

                    }// else

                }// else if
                else if(Key == "proxy_username") {

                    if(!alreadyStored.contains("proxy_username")) {

                        if(Flag == 1) {

                            node.repoProxyUsername[0] = "0";

                            alreadyStored.insert(Key, "0");

                        }// if
                        else {

                            node.repoProxyUsername[0] = "1";

                            alreadyStored.insert(Key, "1");

                        }// else

                        node.repoProxyUsername[1] = Key;
                        node.repoProxyUsername[2] = Value;

                    }// if
                    else {

                        if(alreadyStored.value("proxy_username") == "1") {

                            YUMRepoManager::showMessage("Warning", tr("Option \"%1\" duplicated in repository file: %2").arg(Key).arg(node.repoFileName));

                        }// if
                        else {

                            if(Flag == 1) {

                                node.repoProxyUsername[0] = "0";

                                alreadyStored.remove(Key);

                                alreadyStored.insert(Key, "0");

                            }// if
                            else {

                                node.repoProxyUsername[0] = "1";

                                alreadyStored.remove(Key);

                                alreadyStored.insert(Key, "1");

                            }// else

                            node.repoProxyUsername[1] = Key;
                            node.repoProxyUsername[2] = Value;

                        }// else

                    }// else

                }// else if
                else if(Key == "proxy_password") {

                    if(!alreadyStored.contains("proxy_password")) {

                        if(Flag == 1) {

                            node.repoProxyPassword[0] = "0";

                            alreadyStored.insert(Key, "0");

                        }// if
                        else {

                            node.repoProxyPassword[0] = "1";

                            alreadyStored.insert(Key, "1");

                        }// else

                        node.repoProxyPassword[1] = Key;
                        node.repoProxyPassword[2] = Value;

                    }// if
                    else {

                        if(alreadyStored.value("proxy_password") == "1") {

                            YUMRepoManager::showMessage("Warning", tr("Option \"%1\" duplicated in repository file: %2").arg(Key).arg(node.repoFileName));

                        }// if
                        else {

                            if(Flag == 1) {

                                node.repoProxyPassword[0] = "0";

                                alreadyStored.remove(Key);

                                alreadyStored.insert(Key, "0");

                            }// if
                            else {

                                node.repoProxyPassword[0] = "1";

                                alreadyStored.remove(Key);

                                alreadyStored.insert(Key, "1");

                            }// else

                            node.repoProxyPassword[1] = Key;
                            node.repoProxyPassword[2] = Value;

                        }// else

                    }// else

                }// else if
                else if(Key == "throttle") {

                    if(!alreadyStored.contains("throttle")) {

                        if(Flag == 1) {

                            node.repoBandwidth[0] = "0";

                            alreadyStored.insert(Key, "0");

                        }// if
                        else {

                            node.repoBandwidth[0] = "1";

                            alreadyStored.insert(Key, "1");

                        }// else

                        node.repoBandwidth[1] = Key;
                        node.repoBandwidth[2] = Value;

                    }// if
                    else {

                        if(alreadyStored.value("throttle") == "1") {

                            YUMRepoManager::showMessage("Warning", tr("Option \"%1\" duplicated in repository file: %2").arg(Key).arg(node.repoFileName));

                        }// if
                        else {

                            if(Flag == 1) {

                                node.repoBandwidth[0] = "0";

                                alreadyStored.remove(Key);

                                alreadyStored.insert(Key, "0");

                            }// if
                            else {

                                node.repoBandwidth[0] = "1";

                                alreadyStored.remove(Key);

                                alreadyStored.insert(Key, "1");

                            }// else

                            node.repoBandwidth[1] = Key;
                            node.repoBandwidth[2] = Value;

                        }// else

                    }// else

                }// else if
                else if(Key == "includepkgs") {

                    if(!alreadyStored.contains("includepkgs")) {

                        if(Flag == 1) {

                            node.repoInclude[0] = "0";

                            alreadyStored.insert(Key, "0");

                        }// if
                        else {

                            node.repoInclude[0] = "1";

                            alreadyStored.insert(Key, "1");

                        }// else

                        node.repoInclude[1] = Key;
                        node.repoInclude[2] = Value;

                    }// if
                    else {

                        if(alreadyStored.value("includepkgs") == "1") {

                            YUMRepoManager::showMessage("Warning", tr("Option \"%1\" duplicated in repository file: %2").arg(Key).arg(node.repoFileName));

                        }// if
                        else {

                            if(Flag == 1) {

                                node.repoInclude[0] = "0";

                                alreadyStored.remove(Key);

                                alreadyStored.insert(Key, "0");

                            }// if
                            else {

                                node.repoInclude[0] = "1";

                                alreadyStored.remove(Key);

                                alreadyStored.insert(Key, "1");

                            }// else

                            node.repoInclude[1] = Key;
                            node.repoInclude[2] = Value;

                        }// else

                    }// else

                }// else if
                else if(Key == "exclude") {

                    if(!alreadyStored.contains("exclude")) {

                        if(Flag == 1) {

                            node.repoExclude[0] = "0";

                            alreadyStored.insert(Key, "0");

                        }// if
                        else {

                            node.repoExclude[0] = "1";

                            alreadyStored.insert(Key, "1");

                        }// else

                        node.repoExclude[1] = Key;
                        node.repoExclude[2] = Value;

                    }// if
                    else {

                        if(alreadyStored.value("exclude") == "1") {

                            YUMRepoManager::showMessage("Warning", tr("Option \"%1\" duplicated in repository file: %2").arg(Key).arg(node.repoFileName));

                        }// if
                        else {

                            if(Flag == 1) {

                                node.repoExclude[0] = "0";

                                alreadyStored.remove(Key);

                                alreadyStored.insert(Key, "0");

                            }// if
                            else {

                                node.repoExclude[0] = "1";

                                alreadyStored.remove(Key);

                                alreadyStored.insert(Key, "1");

                            }// else

                            node.repoExclude[1] = Key;
                            node.repoExclude[2] = Value;

                        }// else

                    }// else

                }// else if
                else if(Key == "enabled") {

                    node.repoEnabled[0] = Key;
                    node.repoEnabled[1] = Value;

                }// else if
                else if(Key == "metadata_expire") {

                    if(!alreadyStored.contains("metadata_expire")) {

                        if(Flag == 1) {

                            node.repoMetadataExpire[0] = "0";

                            alreadyStored.insert(Key, "0");

                        }// if
                        else {

                            node.repoMetadataExpire[0] = "1";

                            alreadyStored.insert(Key, "1");

                        }// else

                        node.repoMetadataExpire[1] = Key;
                        node.repoMetadataExpire[2] = Value;

                    }// if
                    else {

                        if(alreadyStored.value("metadata_expire") == "1") {

                            YUMRepoManager::showMessage("Warning", tr("Option \"%1\" duplicated in repository file: %2").arg(Key).arg(node.repoFileName));

                        }// if
                        else {

                            if(Flag == 1) {

                                node.repoMetadataExpire[0] = "0";

                                alreadyStored.remove(Key);

                                alreadyStored.insert(Key, "0");

                            }// if
                            else {

                                node.repoMetadataExpire[0] = "1";

                                alreadyStored.remove(Key);

                                alreadyStored.insert(Key, "1");

                            }// else

                            node.repoMetadataExpire[1] = Key;
                            node.repoMetadataExpire[2] = Value;

                        }// else

                    }// else

                }// else if
                else if(Key == "gpgcheck") {

                    if(!alreadyStored.contains("gpgcheck")) {

                        if(Flag == 1) {

                            node.repoGPGCheck[0] = "0";

                            alreadyStored.insert(Key, "0");

                        }// if
                        else {

                            node.repoGPGCheck[0] = "1";

                            alreadyStored.insert(Key, "1");

                        }// else

                        node.repoGPGCheck[1] = Key;
                        node.repoGPGCheck[2] = Value;

                    }// if
                    else {

                        if(alreadyStored.value("gpgcheck") == "1") {

                            YUMRepoManager::showMessage("Warning", tr("Option \"%1\" duplicated in repository file: %2").arg(Key).arg(node.repoFileName));

                        }// if
                        else {

                            if(Flag == 1) {

                                node.repoGPGCheck[0] = "0";

                                alreadyStored.remove(Key);

                                alreadyStored.insert(Key, "0");

                            }// if
                            else {

                                node.repoGPGCheck[0] = "1";

                                alreadyStored.remove(Key);

                                alreadyStored.insert(Key, "1");

                            }// else

                            node.repoGPGCheck[1] = Key;
                            node.repoGPGCheck[2] = Value;

                        }// else

                    }// else

                }// else if
                else if(Key == "gpgkey") {

                    if(!alreadyStored.contains("gpgkey")) {

                        if(Flag == 1) {

                            node.repoGPGKey[0] = "0";

                            alreadyStored.insert(Key, "0");

                        }// if
                        else {

                            node.repoGPGKey[0] = "1";

                            alreadyStored.insert(Key, "1");

                        }// else

                        node.repoGPGKey[1] = Key;
                        node.repoGPGKey[2] = Value;

                    }// if
                    else {

                        if(alreadyStored.value("gpgkey") == "1") {

                            YUMRepoManager::showMessage("Warning", tr("Option \"%1\" duplicated in repository file: %2").arg(Key).arg(node.repoFileName));

                        }// if
                        else {

                            if(Flag == 1) {

                                node.repoGPGKey[0] = "0";

                                alreadyStored.remove(Key);

                                alreadyStored.insert(Key, "0");

                            }// if
                            else {

                                node.repoGPGKey[0] = "1";

                                alreadyStored.remove(Key);

                                alreadyStored.insert(Key, "1");

                            }// else

                            node.repoGPGKey[1] = Key;
                            node.repoGPGKey[2] = Value;

                        }// else

                    }// else

                }// else if
                else if(Key == "async") {

                    if(!alreadyStored.contains("async")) {

                        if(Flag == 1) {

                            node.repoParallelDownload[0] = "0";

                            alreadyStored.insert(Key, "0");

                        }// if
                        else {

                            node.repoParallelDownload[0] = "1";

                            alreadyStored.insert(Key, "1");

                        }// else

                        node.repoParallelDownload[1] = Key;
                        node.repoParallelDownload[2] = Value;

                    }// if
                    else {

                        if(alreadyStored.value("async") == "1") {

                            YUMRepoManager::showMessage("Warning", tr("Option \"%1\" duplicated in repository file: %2").arg(Key).arg(node.repoFileName));

                        }// if
                        else {

                            if(Flag == 1) {

                                node.repoParallelDownload[0] = "0";

                                alreadyStored.remove(Key);

                                alreadyStored.insert(Key, "0");

                            }// if
                            else {

                                node.repoParallelDownload[0] = "1";

                                alreadyStored.remove(Key);

                                alreadyStored.insert(Key, "1");

                            }// else

                            node.repoParallelDownload[1] = Key;
                            node.repoParallelDownload[2] = Value;

                        }// else

                    }// else

                }// else if

                // The next "if" is just for the case where the repository file doesn't has a new line at the end.

                if(inFlow.atEnd()) {

                    setDefaultValues(node);

                    // This function validate the node looking for duplicated entries, if everything is ok the node will be inserted.

                    insertNewNode(node);

                    alreadyStored.clear();

                }// if

            }// if

        }// while

        inFlow.flush();
        myFile.close();

    }// foreach

}// fillRepoDataList

void DataFlow::fillRepoDataList(const QStringList &repoFiles) {

    // Loop for iterating through all repository files.

    for(int index = 0; index < repoFiles.size(); ++index) {

        QString item = repoFiles.at(index);

        QFile myFile(item);

        // Trying to open the file for read only.

        if(!myFile.open(QIODevice::ReadOnly | QIODevice::Text)) {

            YUMRepoManager::showMessage("Error", tr("When trying to open: %1").arg(item));

            continue;

        }// if

        // Creating the interface to the file.

        QTextStream inFlow(&myFile);

        inFlow.setCodec("UTF-8");

        // Creating a RepoData object when the repository's data will be stored.

        RepoData node;

        QHash<QString, QString> alreadyStored;

        // Loop for iterating through all the lines of the file.

        while(!inFlow.atEnd()) {

            // Reading line

            QString repoLine = inFlow.readLine();

            // Cheking if the line is empty or it just contains blank spaces.

            if(repoLine.isEmpty() || repoLine.contains(QRegExp("^\\s+$"))) {

                if(inFlow.atEnd() && node.repoName.isEmpty() && (node.repoBaseURL[0].isEmpty() || node.repoMetaLink[0].isEmpty())){

                    YUMRepoManager::showMessage("Warning", tr("Empty Repository file: %1").arg(item));

                    break;

                }// if

                // Cheking if it's the last line.

                if(inFlow.atEnd()) {

                    setDefaultValues(node);

                    // This function validate the node looking for duplicated entries, if everything is ok the node will be inserted.

                    insertNewNode(node);

                    alreadyStored.clear();

                }

                continue;

            }// if

            // Storing the absolute path and file name of the file.

            QStringList subFolders = item.split("/");

            QString repoFile = subFolders.at(subFolders.size() - 1);

            node.repoFileName = repoPath + repoFile;

            // Cheking if the line has the required format.

            if((repoLine.contains("=") || (repoLine.startsWith("[") && repoLine.endsWith("]")) ) && ( (!repoLine.contains(QRegExp("\\s+=\\s"))) && (!repoLine.startsWith("=")) && (!repoLine.endsWith("=")) )) {

                int Flag = 0;
                int lineSize = repoLine.length();
                QString Key = "";
                QString Value = "";

                // Cheking if the line starts and ends with "[" and "]".

                if(repoLine.startsWith("[") && repoLine.endsWith("]")) {

                    // Cheking if this is the start of another repository declatation.

                    if(!node.repoName.isEmpty()) {

                        setDefaultValues(node);

                        // This function validate the node looking for duplicated entries, if everything is ok the node will be inserted.

                        insertNewNode(node);

                        alreadyStored.clear();

                        node.repoEnabled[0].clear();
                        node.repoEnabled[1].clear();

                        node.repoName.clear();

                        node.repoComment[0].clear();
                        node.repoComment[1].clear();

                        node.repoBaseURL[0].clear();
                        node.repoBaseURL[1].clear();
                        node.repoBaseURL[2].clear();

                        node.repoMetaLink[0].clear();
                        node.repoMetaLink[1].clear();
                        node.repoMetaLink[2].clear();

                        node.repoGPGCheck[0].clear();
                        node.repoGPGCheck[1].clear();
                        node.repoGPGCheck[2].clear();

                        node.repoGPGKey[0].clear();
                        node.repoGPGKey[1].clear();
                        node.repoGPGKey[2].clear();

                        node.repoProxy[0].clear();
                        node.repoProxy[1].clear();
                        node.repoProxy[2].clear();

                        node.repoProxyUsername[0].clear();
                        node.repoProxyUsername[1].clear();
                        node.repoProxyUsername[2].clear();

                        node.repoProxyPassword[0].clear();
                        node.repoProxyPassword[1].clear();
                        node.repoProxyPassword[2].clear();

                        node.repoFailoverMethod[0].clear();
                        node.repoFailoverMethod[1].clear();
                        node.repoFailoverMethod[2].clear();

                        node.repoSkipIfUnavailable[0].clear();
                        node.repoSkipIfUnavailable[1].clear();
                        node.repoSkipIfUnavailable[2].clear();

                        node.repoBandwidth[0].clear();
                        node.repoBandwidth[1].clear();
                        node.repoBandwidth[2].clear();

                        node.repoMetadataExpire[0].clear();
                        node.repoMetadataExpire[1].clear();
                        node.repoMetadataExpire[2].clear();

                        node.repoParallelDownload[0].clear();
                        node.repoParallelDownload[1].clear();
                        node.repoParallelDownload[2].clear();

                        node.repoInclude[0].clear();
                        node.repoInclude[1].clear();
                        node.repoInclude[2].clear();

                        node.repoExclude[0].clear();
                        node.repoExclude[1].clear();
                        node.repoExclude[2].clear();

                    }// if

                    // Extracting the word between the "[" and "]" characters and saving it as the repository's name.

                    node.repoName = repoLine.mid(1, (repoLine.length() - 2));

                    // Cheking if this's not the end of the file.

                    if(!inFlow.atEnd()) {

                        continue;

                    }// if
                    else {

                        YUMRepoManager::showMessage("Warning", tr("Syntax error in repository file: %1").arg(node.repoFileName));

                        break;

                    }// else

                }// if

                // Loop for split the line and get the Key and Value.

                for(int count = 0; count < lineSize; count++) {

                    if(repoLine[count].isSpace() && Flag == 0)
                        continue;

                    if(repoLine[count] == '=' && Flag == 0) {

                        Flag = 1;

                        continue;

                    }// if

                    if(Flag == 0)
                        Key.append(repoLine[count]);

                    if(Flag == 1)
                        Value.append(repoLine[count]);

                }// for

                // Cheking if the line is commented.

                if(Key.startsWith("#")) {

                    Flag = 1;

                    Key = Key.mid(1, (Key.length() - 1));

                }// if
                else
                    Flag = 0;

                // Cheking if the Key is one of what this program manage.

                if(!repoKeyList.contains(Key)) {

                    YUMRepoManager::showMessage("Warning", tr("Unknow option \"%1\"in repository file: %2").arg(Key).arg(node.repoFileName));

                    continue;

                }// if

                // In this part starts the storing of the Keys and its values.

                if(Key == "name") {

                    node.repoComment[0] = Key;
                    node.repoComment[1] = Value;

                }// if
                else if(Key == "failovermethod") {

                    if(!alreadyStored.contains("failovermethod")) {

                        if(Flag == 1) {

                            node.repoFailoverMethod[0] = "0";

                            alreadyStored.insert(Key, "0");

                        }// if
                        else {

                            node.repoFailoverMethod[0] = "1";

                            alreadyStored.insert(Key, "1");

                        }// else

                        node.repoFailoverMethod[1] = Key;
                        node.repoFailoverMethod[2] = Value;

                    }// if
                    else {

                        if(alreadyStored.value("failovermethod") == "1") {

                            YUMRepoManager::showMessage("Warning", tr("Option \"%1\" duplicated in repository file: %2").arg(Key).arg(node.repoFileName));

                        }// if
                        else {

                            if(Flag == 1) {

                                node.repoFailoverMethod[0] = "0";

                                alreadyStored.remove(Key);

                                alreadyStored.insert(Key, "0");

                            }// if
                            else {

                                node.repoFailoverMethod[0] = "1";

                                alreadyStored.remove(Key);

                                alreadyStored.insert(Key, "1");

                            }// else

                            node.repoFailoverMethod[1] = Key;
                            node.repoFailoverMethod[2] = Value;

                        }// else

                    }// else

                }// else if
                else if(Key == "skip_if_unavailable") {

                    if(!alreadyStored.contains("skip_if_unavailable")) {

                        if(Flag == 1) {

                            node.repoSkipIfUnavailable[0] = "0";

                            alreadyStored.insert(Key, "0");

                        }// if
                        else {

                            node.repoSkipIfUnavailable[0] = "1";

                            alreadyStored.insert(Key, "1");

                        }// else

                        node.repoSkipIfUnavailable[1] = Key;
                        node.repoSkipIfUnavailable[2] = Value;

                    }// if
                    else {

                        if(alreadyStored.value("skip_if_unavailable") == "1") {

                            YUMRepoManager::showMessage("Warning", tr("Option \"%1\" duplicated in repository file: %2").arg(Key).arg(node.repoFileName));

                        }// if
                        else {

                            if(Flag == 1) {

                                node.repoSkipIfUnavailable[0] = "0";

                                alreadyStored.remove(Key);

                                alreadyStored.insert(Key, "0");

                            }// if
                            else {

                                node.repoSkipIfUnavailable[0] = "1";

                                alreadyStored.remove(Key);

                                alreadyStored.insert(Key, "1");

                            }// else

                            node.repoSkipIfUnavailable[1] = Key;
                            node.repoSkipIfUnavailable[2] = Value;

                        }// else

                    }// else

                }// else if
                else if(Key == "baseurl") {

                    if(!alreadyStored.contains("baseurl")) {

                        if(Flag == 1) {

                            node.repoBaseURL[0] = "0";

                            alreadyStored.insert(Key, "0");

                        }// if
                        else {

                            node.repoBaseURL[0] = "1";

                            alreadyStored.insert(Key, "1");

                        }// else

                        node.repoBaseURL[1] = Key;
                        node.repoBaseURL[2] = Value;

                    }// if
                    else {

                        if(alreadyStored.value("baseurl") == "1") {

                            YUMRepoManager::showMessage("Warning", tr("Option \"%1\" duplicated in repository file: %2").arg(Key).arg(node.repoFileName));

                        }// if
                        else {

                            if(Flag == 1) {

                                node.repoBaseURL[0] = "0";

                                alreadyStored.remove(Key);

                                alreadyStored.insert(Key, "0");

                            }// if
                            else {

                                node.repoBaseURL[0] = "1";

                                alreadyStored.remove(Key);

                                alreadyStored.insert(Key, "1");

                            }// else

                            node.repoBaseURL[1] = Key;
                            node.repoBaseURL[2] = Value;

                        }// else

                    }// else

                }// else if
                else if(Key == "MetaLink") {

                    if(!alreadyStored.contains("MetaLink")) {

                        if(Flag == 1) {

                            node.repoMetaLink[0] = "0";

                            alreadyStored.insert(Key, "0");

                        }// if
                        else {

                            node.repoMetaLink[0] = "1";

                            alreadyStored.insert(Key, "1");

                        }// else

                        node.repoMetaLink[1] = Key;
                        node.repoMetaLink[2] = Value;

                    }// if
                    else {

                        if(alreadyStored.value("MetaLink") == "1") {

                            YUMRepoManager::showMessage("Warning", tr("Option \"%1\" duplicated in repository file: %2").arg(Key).arg(node.repoFileName));

                        }// if
                        else {

                            if(Flag == 1) {

                                node.repoMetaLink[0] = "0";

                                alreadyStored.remove(Key);

                                alreadyStored.insert(Key, "0");

                            }// if
                            else {

                                node.repoMetaLink[0] = "1";

                                alreadyStored.remove(Key);

                                alreadyStored.insert(Key, "1");

                            }// else

                            node.repoMetaLink[1] = Key;
                            node.repoMetaLink[2] = Value;

                        }// else

                    }// else

                }// else if
                else if(Key == "proxy") {

                    if(!alreadyStored.contains("proxy")) {

                        if(Flag == 1) {

                            node.repoProxy[0] = "0";

                            alreadyStored.insert(Key, "0");

                        }// if
                        else {

                            node.repoProxy[0] = "1";

                            alreadyStored.insert(Key, "1");

                        }// else

                        node.repoProxy[1] = Key;
                        node.repoProxy[2] = Value;

                    }// if
                    else {

                        if(alreadyStored.value("proxy") == "1") {

                            YUMRepoManager::showMessage("Warning", tr("Option \"%1\" duplicated in repository file: %2").arg(Key).arg(node.repoFileName));

                        }// if
                        else {

                            if(Flag == 1) {

                                node.repoProxy[0] = "0";

                                alreadyStored.remove(Key);

                                alreadyStored.insert(Key, "0");

                            }// if
                            else {

                                node.repoProxy[0] = "1";

                                alreadyStored.remove(Key);

                                alreadyStored.insert(Key, "1");

                            }// else

                            node.repoProxy[1] = Key;
                            node.repoProxy[2] = Value;

                        }// else

                    }// else

                }// else if
                else if(Key == "proxy_username") {

                    if(!alreadyStored.contains("proxy_username")) {

                        if(Flag == 1) {

                            node.repoProxyUsername[0] = "0";

                            alreadyStored.insert(Key, "0");

                        }// if
                        else {

                            node.repoProxyUsername[0] = "1";

                            alreadyStored.insert(Key, "1");

                        }// else

                        node.repoProxyUsername[1] = Key;
                        node.repoProxyUsername[2] = Value;

                    }// if
                    else {

                        if(alreadyStored.value("proxy_username") == "1") {

                            YUMRepoManager::showMessage("Warning", tr("Option \"%1\" duplicated in repository file: %2").arg(Key).arg(node.repoFileName));

                        }// if
                        else {

                            if(Flag == 1) {

                                node.repoProxyUsername[0] = "0";

                                alreadyStored.remove(Key);

                                alreadyStored.insert(Key, "0");

                            }// if
                            else {

                                node.repoProxyUsername[0] = "1";

                                alreadyStored.remove(Key);

                                alreadyStored.insert(Key, "1");

                            }// else

                            node.repoProxyUsername[1] = Key;
                            node.repoProxyUsername[2] = Value;

                        }// else

                    }// else

                }// else if
                else if(Key == "proxy_password") {

                    if(!alreadyStored.contains("proxy_password")) {

                        if(Flag == 1) {

                            node.repoProxyPassword[0] = "0";

                            alreadyStored.insert(Key, "0");

                        }// if
                        else {

                            node.repoProxyPassword[0] = "1";

                            alreadyStored.insert(Key, "1");

                        }// else

                        node.repoProxyPassword[1] = Key;
                        node.repoProxyPassword[2] = Value;

                    }// if
                    else {

                        if(alreadyStored.value("proxy_password") == "1") {

                            YUMRepoManager::showMessage("Warning", tr("Option \"%1\" duplicated in repository file: %2").arg(Key).arg(node.repoFileName));

                        }// if
                        else {

                            if(Flag == 1) {

                                node.repoProxyPassword[0] = "0";

                                alreadyStored.remove(Key);

                                alreadyStored.insert(Key, "0");

                            }// if
                            else {

                                node.repoProxyPassword[0] = "1";

                                alreadyStored.remove(Key);

                                alreadyStored.insert(Key, "1");

                            }// else

                            node.repoProxyPassword[1] = Key;
                            node.repoProxyPassword[2] = Value;

                        }// else

                    }// else

                }// else if
                else if(Key == "throttle") {

                    if(!alreadyStored.contains("throttle")) {

                        if(Flag == 1) {

                            node.repoBandwidth[0] = "0";

                            alreadyStored.insert(Key, "0");

                        }// if
                        else {

                            node.repoBandwidth[0] = "1";

                            alreadyStored.insert(Key, "1");

                        }// else

                        node.repoBandwidth[1] = Key;
                        node.repoBandwidth[2] = Value;

                    }// if
                    else {

                        if(alreadyStored.value("throttle") == "1") {

                            YUMRepoManager::showMessage("Warning", tr("Option \"%1\" duplicated in repository file: %2").arg(Key).arg(node.repoFileName));

                        }// if
                        else {

                            if(Flag == 1) {

                                node.repoBandwidth[0] = "0";

                                alreadyStored.remove(Key);

                                alreadyStored.insert(Key, "0");

                            }// if
                            else {

                                node.repoBandwidth[0] = "1";

                                alreadyStored.remove(Key);

                                alreadyStored.insert(Key, "1");

                            }// else

                            node.repoBandwidth[1] = Key;
                            node.repoBandwidth[2] = Value;

                        }// else

                    }// else

                }// else if
                else if(Key == "includepkgs") {

                    if(!alreadyStored.contains("includepkgs")) {

                        if(Flag == 1) {

                            node.repoInclude[0] = "0";

                            alreadyStored.insert(Key, "0");

                        }// if
                        else {

                            node.repoInclude[0] = "1";

                            alreadyStored.insert(Key, "1");

                        }// else

                        node.repoInclude[1] = Key;
                        node.repoInclude[2] = Value;

                    }// if
                    else {

                        if(alreadyStored.value("includepkgs") == "1") {

                            YUMRepoManager::showMessage("Warning", tr("Option \"%1\" duplicated in repository file: %2").arg(Key).arg(node.repoFileName));

                        }// if
                        else {

                            if(Flag == 1) {

                                node.repoInclude[0] = "0";

                                alreadyStored.remove(Key);

                                alreadyStored.insert(Key, "0");

                            }// if
                            else {

                                node.repoInclude[0] = "1";

                                alreadyStored.remove(Key);

                                alreadyStored.insert(Key, "1");

                            }// else

                            node.repoInclude[1] = Key;
                            node.repoInclude[2] = Value;

                        }// else

                    }// else

                }// else if
                else if(Key == "exclude") {

                    if(!alreadyStored.contains("exclude")) {

                        if(Flag == 1) {

                            node.repoExclude[0] = "0";

                            alreadyStored.insert(Key, "0");

                        }// if
                        else {

                            node.repoExclude[0] = "1";

                            alreadyStored.insert(Key, "1");

                        }// else

                        node.repoExclude[1] = Key;
                        node.repoExclude[2] = Value;

                    }// if
                    else {

                        if(alreadyStored.value("exclude") == "1") {

                            YUMRepoManager::showMessage("Warning", tr("Option \"%1\" duplicated in repository file: %2").arg(Key).arg(node.repoFileName));

                        }// if
                        else {

                            if(Flag == 1) {

                                node.repoExclude[0] = "0";

                                alreadyStored.remove(Key);

                                alreadyStored.insert(Key, "0");

                            }// if
                            else {

                                node.repoExclude[0] = "1";

                                alreadyStored.remove(Key);

                                alreadyStored.insert(Key, "1");

                            }// else

                            node.repoExclude[1] = Key;
                            node.repoExclude[2] = Value;

                        }// else

                    }// else

                }// else if
                else if(Key == "enabled") {

                    node.repoEnabled[0] = Key;
                    node.repoEnabled[1] = Value;

                }// else if
                else if(Key == "metadata_expire") {

                    if(!alreadyStored.contains("metadata_expire")) {

                        if(Flag == 1) {

                            node.repoMetadataExpire[0] = "0";

                            alreadyStored.insert(Key, "0");

                        }// if
                        else {

                            node.repoMetadataExpire[0] = "1";

                            alreadyStored.insert(Key, "1");

                        }// else

                        node.repoMetadataExpire[1] = Key;
                        node.repoMetadataExpire[2] = Value;

                    }// if
                    else {

                        if(alreadyStored.value("metadata_expire") == "1") {

                            YUMRepoManager::showMessage("Warning", tr("Option \"%1\" duplicated in repository file: %2").arg(Key).arg(node.repoFileName));

                        }// if
                        else {

                            if(Flag == 1) {

                                node.repoMetadataExpire[0] = "0";

                                alreadyStored.remove(Key);

                                alreadyStored.insert(Key, "0");

                            }// if
                            else {

                                node.repoMetadataExpire[0] = "1";

                                alreadyStored.remove(Key);

                                alreadyStored.insert(Key, "1");

                            }// else

                            node.repoMetadataExpire[1] = Key;
                            node.repoMetadataExpire[2] = Value;

                        }// else

                    }// else

                }// else if
                else if(Key == "gpgcheck") {

                    if(!alreadyStored.contains("gpgcheck")) {

                        if(Flag == 1) {

                            node.repoGPGCheck[0] = "0";

                            alreadyStored.insert(Key, "0");

                        }// if
                        else {

                            node.repoGPGCheck[0] = "1";

                            alreadyStored.insert(Key, "1");

                        }// else

                        node.repoGPGCheck[1] = Key;
                        node.repoGPGCheck[2] = Value;

                    }// if
                    else {

                        if(alreadyStored.value("gpgcheck") == "1") {

                            YUMRepoManager::showMessage("Warning", tr("Option \"%1\" duplicated in repository file: %2").arg(Key).arg(node.repoFileName));

                        }// if
                        else {

                            if(Flag == 1) {

                                node.repoGPGCheck[0] = "0";

                                alreadyStored.remove(Key);

                                alreadyStored.insert(Key, "0");

                            }// if
                            else {

                                node.repoGPGCheck[0] = "1";

                                alreadyStored.remove(Key);

                                alreadyStored.insert(Key, "1");

                            }// else

                            node.repoGPGCheck[1] = Key;
                            node.repoGPGCheck[2] = Value;

                        }// else

                    }// else

                }// else if
                else if(Key == "gpgkey") {

                    if(!alreadyStored.contains("gpgkey")) {

                        if(Flag == 1) {

                            node.repoGPGKey[0] = "0";

                            alreadyStored.insert(Key, "0");

                        }// if
                        else {

                            node.repoGPGKey[0] = "1";

                            alreadyStored.insert(Key, "1");

                        }// else

                        node.repoGPGKey[1] = Key;
                        node.repoGPGKey[2] = Value;

                    }// if
                    else {

                        if(alreadyStored.value("gpgkey") == "1") {

                            YUMRepoManager::showMessage("Warning", tr("Option \"%1\" duplicated in repository file: %2").arg(Key).arg(node.repoFileName));

                        }// if
                        else {

                            if(Flag == 1) {

                                node.repoGPGKey[0] = "0";

                                alreadyStored.remove(Key);

                                alreadyStored.insert(Key, "0");

                            }// if
                            else {

                                node.repoGPGKey[0] = "1";

                                alreadyStored.remove(Key);

                                alreadyStored.insert(Key, "1");

                            }// else

                            node.repoGPGKey[1] = Key;
                            node.repoGPGKey[2] = Value;

                        }// else

                    }// else

                }// else if
                else if(Key == "async") {

                    if(!alreadyStored.contains("async")) {

                        if(Flag == 1) {

                            node.repoParallelDownload[0] = "0";

                            alreadyStored.insert(Key, "0");

                        }// if
                        else {

                            node.repoParallelDownload[0] = "1";

                            alreadyStored.insert(Key, "1");

                        }// else

                        node.repoParallelDownload[1] = Key;
                        node.repoParallelDownload[2] = Value;

                    }// if
                    else {

                        if(alreadyStored.value("async") == "1") {

                            YUMRepoManager::showMessage("Warning", tr("Option \"%1\" duplicated in repository file: %2").arg(Key).arg(node.repoFileName));

                        }// if
                        else {

                            if(Flag == 1) {

                                node.repoParallelDownload[0] = "0";

                                alreadyStored.remove(Key);

                                alreadyStored.insert(Key, "0");

                            }// if
                            else {

                                node.repoParallelDownload[0] = "1";

                                alreadyStored.remove(Key);

                                alreadyStored.insert(Key, "1");

                            }// else

                            node.repoParallelDownload[1] = Key;
                            node.repoParallelDownload[2] = Value;

                        }// else

                    }// else

                }// else if

                // The next "if" is just for the case where the repository file doesn't has a new line at the end.

                if(inFlow.atEnd()) {

                    setDefaultValues(node);

                    // This function validate the node looking for duplicated entries, if everything is ok the node will be inserted.

                    insertNewNode(node);

                    alreadyStored.clear();

                }// if

            }// if

        }// while

        inFlow.flush();
        myFile.close();

    }// for

}// fillRepoDataList

void DataFlow::setDefaultValues(RepoData &node) {

    if(node.repoEnabled[0].isEmpty()) {

        node.repoEnabled[0] = "enabled";
        node.repoEnabled[1] = "1";

    }// if

    if(node.repoGPGCheck[0].isEmpty()) {

        node.repoGPGCheck[0] = "1";
        node.repoGPGCheck[1] = "gpgcheck";
        node.repoGPGCheck[2] = "1";

    }// if

    if(node.repoSkipIfUnavailable[0].isEmpty()) {

        node.repoSkipIfUnavailable[0] = "1";
        node.repoSkipIfUnavailable[1] = "skip_if_unavailable";
        node.repoSkipIfUnavailable[2] = "1";

    }// if

    if(node.repoFailoverMethod[0].isEmpty()) {

        node.repoFailoverMethod[0] = "1";
        node.repoFailoverMethod[1] = "failovermethod";
        node.repoFailoverMethod[2] = "roundrobin";

    }// if

    if(node.repoMetadataExpire[0].isEmpty()) {

        node.repoMetadataExpire[0] = "1";
        node.repoMetadataExpire[1] = "metadata_expire";
        node.repoMetadataExpire[2] = "1d";

    }// if

    if(node.repoParallelDownload[0].isEmpty()) {

        node.repoParallelDownload[0] = "1";
        node.repoParallelDownload[1] = "async";
        node.repoParallelDownload[2] = "1";

    }// if

}// setDefaultValues

void DataFlow::fillPluginDataList() {

    QStringList pluginFiles = locateFiles(pluginPath, "*.conf", DataFlow::AbsoluteFileName);

    if(pluginFiles.isEmpty()) {

        YUMRepoManager::showMessage("Warning", tr("There is no plugins installed."));

        return;

    }// if

    int size = pluginFiles.size();

    for(int index1 = 0; index1 < size; ++index1) {

        QFile file(pluginFiles.at(index1));

        if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {

            YUMRepoManager::showMessage("Error", tr("Access deny when trying to open: %1").arg(pluginFiles.at(index1)));

            file.close();

            continue;

        }// if

        QTextStream inFlow(&file);

        inFlow.setCodec("UTF-8");

        int flowPosition = 0;

        for(;!inFlow.atEnd();) {

            ++flowPosition;

            QString pluginLine = inFlow.readLine();

            if(pluginLine.isEmpty()) {

                continue;

            }// if

            PluginData pluginData;

            int flag = 0;
            bool enabledPositionEstablished = false;
            QString key = "";
            QString value = "";

            if(pluginLine.contains("=") && ( (!pluginLine.startsWith("#")) && (!pluginLine.startsWith("=")) && (!pluginLine.endsWith("=")) && (!pluginLine.contains(QRegExp("^\\s+=\\s+$"))) && (!pluginLine.startsWith("[")) )) {

                int pluginLineSize = pluginLine.length();

                for(int index2 = 0; index2 < pluginLineSize; ++index2) {

                    if(pluginLine[index2].isSpace())
                        continue;

                    if( (pluginLine[index2] == '=') && (flag == 0) ) {

                        flag = 1;

                        continue;

                    }// if

                    if(flag == 0)
                        key.append(pluginLine[index2]);

                    if(flag == 1) {

                        if((!enabledPositionEstablished) && (key == "enabled")) {

                            pluginData.pluginEnabledPosition = (flowPosition);

                            enabledPositionEstablished = true;

                        }// if

                        value.append(pluginLine[index2]);

                    }// if

                }// for

                if(key == "enabled") {

                    pluginData.pluginEnabled[0] = key;
                    pluginData.pluginEnabled[1] = value;

                    pluginData.pluginName = pluginFiles.at(index1).mid(0, pluginFiles.at(index1).size() - 5);

                    pluginData.pluginFileName = pluginFiles.at(index1);

                    pluginDataList.append(pluginData);

                }// if

            }// if

        }// for

        file.close();

    }// for

}// fillPluginDataList

void DataFlow::fillProtectedList() {

    QStringList protectedList = DataFlow::locateFiles(DataFlow::protectedPath, "*.conf", DataFlow::FileName);

    protectedListModel->setStringList(protectedList);

}// fillprotectedList

void DataFlow::setRepoDataList(int repoLabelIndex, RepoData::RepoLabel repoLabel, int valueIndex, QVariant value[3]) {

    if(repoLabel == RepoData::Name) {

        repoDataList[repoLabelIndex].repoName = value[valueIndex].toString();

        return;

    }// if

    if(repoLabel == RepoData::Comment) {

        repoDataList[repoLabelIndex].repoComment[valueIndex] = value[valueIndex].toString();

        return;

    }// if

    if(repoLabel == RepoData::FailoverMethod) {

        repoDataList[repoLabelIndex].repoFailoverMethod[valueIndex] = value[valueIndex].toString();

        return;

    }// if

    if(repoLabel == RepoData::SkipIfUnavailable) {

        repoDataList[repoLabelIndex].repoSkipIfUnavailable[valueIndex] = value[valueIndex].toString();

        return;

    }// if

    if(repoLabel == RepoData::BaseURL) {

        repoDataList[repoLabelIndex].repoBaseURL[valueIndex] = value[valueIndex].toString();

        return;

    }// if

    if(repoLabel == RepoData::MetaLink) {

        repoDataList[repoLabelIndex].repoMetaLink[valueIndex] = value[valueIndex].toString();

        return;

    }// if

    if(repoLabel == RepoData::Proxy) {

        repoDataList[repoLabelIndex].repoProxy[valueIndex] = value[valueIndex].toString();

        return;

    }// if

    if(repoLabel == RepoData::ProxyUsername) {

        repoDataList[repoLabelIndex].repoProxyUsername[valueIndex] = value[valueIndex].toString();

        return;

    }// if

    if(repoLabel == RepoData::ProxyPassword) {

        repoDataList[repoLabelIndex].repoProxyPassword[valueIndex] = value[valueIndex].toString();

        return;

    }// if

    if(repoLabel == RepoData::Speed) {

        repoDataList[repoLabelIndex].repoBandwidth[valueIndex] = value[valueIndex].toString();

        return;

    }// if

    if(repoLabel == RepoData::Include) {

        repoDataList[repoLabelIndex].repoInclude[valueIndex] = value[valueIndex].toString();

        return;

    }// if

    if(repoLabel == RepoData::Exclude) {

        repoDataList[repoLabelIndex].repoExclude[valueIndex] = value[valueIndex].toString();

        return;

    }// if

    if(repoLabel == RepoData::Enabled) {

        repoDataList[repoLabelIndex].repoEnabled[valueIndex] = value[valueIndex].toString();

        return;

    }// if

    if(repoLabel == RepoData::MetadataExpire) {

        repoDataList[repoLabelIndex].repoMetadataExpire[valueIndex] = value[valueIndex].toString();

        return;

    }// if

    if(repoLabel == RepoData::GPGCheck) {

        repoDataList[repoLabelIndex].repoGPGCheck[valueIndex] = value[valueIndex].toString();

        return;

    }// if

    if(repoLabel == RepoData::GPGKey) {

        repoDataList[repoLabelIndex].repoGPGKey[valueIndex] = value[valueIndex].toString();

        return;

    }// if

    if(repoLabel == RepoData::FileName) {

        repoDataList[repoLabelIndex].repoFileName = value[valueIndex].toString();

        return;

    }// if

    if(repoLabel == RepoData::ParallelDownload) {

        repoDataList[repoLabelIndex].repoParallelDownload[valueIndex] = value[valueIndex].toString();

        return;

    }// if

}// setRepoDataList

void DataFlow::insertNewNode(const RepoData &node) {

    if(node.repoBaseURL[2].isEmpty() && node.repoMetaLink[2].isEmpty()) {

        YUMRepoManager::showMessage("Error", tr("No repository address specified in: %1").arg(node.repoName));

    }// if

    bool flag = true;
    bool match = false;

    int size = DataFlow::repoDataList.size();
    int indexPosition = 0;

    for(int index = 0; index < size; ++index) {

        RepoData myData = DataFlow::repoDataList.at(index);

        if(node.repoName == myData.repoName) {

            flag = false;
            match = false;

            break;

        }// if

        if(node.repoFileName == myData.repoFileName) {

            indexPosition = index;

            match = true;

        }// if

    }// for

    if(match) {

        repoDataList.insert(indexPosition, node);

        flag = false;

    }// if

    if(flag) {

        repoDataList.append(node);

    }// if

}// insertNewNode
