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

#include "repodatamodel.h"
#include "dataflow.h"
#include "yumrepomanager.h"
#include "mytableview.h"
#include "repodata.h"

#include <QList>
#include <QDebug>
#include <QMessageBox>
#include <QFile>
#include <QUrl>
#include <QEvent>

RepoDataModel::RepoDataModel(QObject *parent) : QAbstractTableModel(parent)
{

    header << "" << tr("Name") << tr("Comment") << "" << tr("Base URL") << "" << tr("Metalink") << "" << tr("GPG Check") << "" << tr("GPG Key")
           << "" << tr("Proxy") << "" << tr("Proxy Username") << "" << tr("Proxy Password") << "" << tr("Bandwidth Throttle") << "" << tr("Skip If Unavailable")
           << "" << tr("Failover Method") << "" << tr("Metadata Expire") << "" << tr("Parallel Download") << "" << tr("Include") << "" << tr("Exclude");

    firstTime = true;

    initDataState();

    cellColor[Enabled].setRgb(182, 255, 132, 128);
    cellColor[Disabled].setRgb(255, 164, 196, 128);

}// Constructor

QVariant RepoDataModel::headerData(int section, Qt::Orientation orientation, int role) const {

    if(orientation == Qt::Horizontal) {

        if(role == Qt::DisplayRole) {

            return header.at(section);

        }// if

    }// if

    if(orientation == Qt::Vertical) {

        if(role == Qt::DisplayRole) {

            return data(index(section, 1), Qt::DisplayRole).toString();

        }// if

    }// if

    if(orientation == Qt::Vertical) {

        if(role == Qt::BackgroundColorRole) {

            if(enabledState.at(section) == true) {

                return YUMRepoManager::colorEnabled();

            }// if
            else {

                return YUMRepoManager::colorDisabled();

            }// else

        }// if

    }// if

    return QAbstractTableModel::headerData(section, orientation, role);

}// headerData

int RepoDataModel::rowCount(const QModelIndex &parent) const {

    Q_UNUSED(parent);

    return DataFlow::repoDataListSize();

}// rowCount

int RepoDataModel::columnCount(const QModelIndex &parent) const {

    Q_UNUSED(parent);

    return 31;

}// columnCount

Qt::ItemFlags RepoDataModel::flags(const QModelIndex &index) const {

    if(!index.isValid()) {

        return 0;

    }// if

    if(index.column() == 0 || index.column() == 3 || index.column() == 5 || index.column() == 7 || index.column() == 9 || index.column() == 11
       || index.column() == 13 || index.column() == 15 || index.column() == 17 || index.column() == 19 || index.column() == 21 || index.column() == 23
       || index.column() == 25 || index.column() == 27 || index.column() == 29) {

        return QAbstractItemModel::flags(index) | Qt::ItemIsUserCheckable | Qt::ItemIsDragEnabled;

    }// if

    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable | Qt::ItemIsDragEnabled;

}// flags

QVariant RepoDataModel::data(const QModelIndex &index, int role) const {

    if(!index.isValid()) {

        return QVariant();

    }// if

    QString repoName = DataFlow::getRepoDataList().at(index.row()).repoName;
    QString repoComment = DataFlow::getRepoDataList().at(index.row()).repoComment[1];
    QString repoProxy = DataFlow::getRepoDataList().at(index.row()).repoProxy[2];
    QString repoProxyUsername = DataFlow::getRepoDataList().at(index.row()).repoProxyUsername[2];
    QString repoProxyPassword = DataFlow::getRepoDataList().at(index.row()).repoProxyPassword[2];
    QString repoBaseURL = DataFlow::getRepoDataList().at(index.row()).repoBaseURL[2];
    QString repoMetaLink = DataFlow::getRepoDataList().at(index.row()).repoMetaLink[2];
    QString repoBandwidth = DataFlow::getRepoDataList().at(index.row()).repoBandwidth[2];
    QString repoInclude = DataFlow::getRepoDataList().at(index.row()).repoInclude[2];
    QString repoExclude = DataFlow::getRepoDataList().at(index.row()).repoExclude[2];
    QString repoSkipIfUnavailable = DataFlow::getRepoDataList().at(index.row()).repoSkipIfUnavailable[2];
    QString repoFailoverMethod = DataFlow::getRepoDataList().at(index.row()).repoFailoverMethod[2];
    QString repoMetadataExpire = DataFlow::getRepoDataList().at(index.row()).repoMetadataExpire[2];
    QString repoGPGCheck = DataFlow::getRepoDataList().at(index.row()).repoGPGCheck[2];
    QString repoGPGKey = DataFlow::getRepoDataList().at(index.row()).repoGPGKey[2];
    QString repoParallelDownload = DataFlow::getRepoDataList().at(index.row()).repoParallelDownload[2];

    QStringList repoIncludeList;

    repoIncludeList = repoInclude.split(" ", QString::SkipEmptyParts);

    repoInclude.clear();

    foreach(QString item, repoIncludeList) {

        repoInclude.append(item + " ");

    }// foreach

    QString bindProxyPassword;

    if(!repoProxyPassword.isEmpty()) {

        int passwordSize = repoProxyPassword.size();

        for(int count = 0; count < passwordSize; count++) {

            bindProxyPassword.append("*");

        }// for

    }// if

    //
    // SkipIfUnavailable
    //

    if(repoSkipIfUnavailable == "1") {

        repoSkipIfUnavailable = YUMRepoManager::translate("RepoDataDelegate", "Yes");

    }
    else if(repoSkipIfUnavailable == "0") {

        repoSkipIfUnavailable = YUMRepoManager::translate("RepoDataDelegate", "No");

    }// else

    //
    // ParallelDownload
    //

    if(repoParallelDownload == "1") {

        repoParallelDownload = YUMRepoManager::translate("RepoDataDelegate", "Yes");

    }
    else if(repoParallelDownload == "0") {

        repoParallelDownload = YUMRepoManager::translate("RepoDataDelegate", "No");

    }// else

    //
    //  GPGCheck
    //

    if(repoGPGCheck == "1") {

        repoGPGCheck = YUMRepoManager::translate("RepoDataDelegate", "Yes");

    }
    else if(repoGPGCheck == "0") {

        repoGPGCheck = YUMRepoManager::translate("RepoDataDelegate", "No");

    }// else

    //
    //  BandWidth
    //

    QString bandwidthAmount = repoBandwidth.mid(0, (repoBandwidth.size() - 1));
    QString bandwidth = repoBandwidth.mid((repoBandwidth.size() - 1), repoBandwidth.size());

    if(bandwidth == "G") {

        bandwidth = "Gigabytes";

    }// if
    else if(bandwidth == "M") {

        bandwidth = "Megabytes";

    }// else if
    else if(bandwidth == "K") {

        bandwidth = "Kilobytes";

    }// else if

    //
    //  MetadataExpire
    //

    QString metadataExpireAmount = repoMetadataExpire.mid(0, (repoMetadataExpire.size() - 1));
    QString metadataExpireFrequency = repoMetadataExpire.mid((repoMetadataExpire.size() - 1), repoMetadataExpire.size());

    if(metadataExpireFrequency == "d") {

        metadataExpireFrequency = YUMRepoManager::translate("RepoDataModel", "Day/s");

    }// if
    else if(metadataExpireFrequency == "h") {

        metadataExpireFrequency = YUMRepoManager::translate("RepoDataModel", "Hour/s");

    }// else if
    else if(metadataExpireFrequency == "m") {

        metadataExpireFrequency = YUMRepoManager::translate("RepoDataModel", "Minute/s");

    }// else if

    QStringList myRow;

    myRow << "" << repoName << repoComment << "" << repoBaseURL << "" << repoMetaLink << "" << repoGPGCheck << "" << repoGPGKey
          << "" << repoProxy << "" << repoProxyUsername << "" << bindProxyPassword << "" << (bandwidthAmount + " " + bandwidth) << "" << repoSkipIfUnavailable
          << "" << repoFailoverMethod << "" << (metadataExpireAmount + " " + metadataExpireFrequency) << "" << repoParallelDownload << "" << repoInclude << "" << repoExclude;

    if(role == Qt::DisplayRole || role == Qt::EditRole) {

        return myRow.at(index.column());

    }// if

    if(role == Qt::CheckStateRole) {

        if(index.column() == 0) {

            if(enabledState.at(index.row()) == true) {

                return Qt::Checked;

            }// if
            else {

                return Qt::Unchecked;

            }// else

        }// if

        if(index.column() == 3) {

            if(enabledBaseURL.at(index.row()) == true) {

                return Qt::Checked;

            }// if
            else {

                return Qt::Unchecked;

            }// else

        }//if

        if(index.column() == 5) {

            if(enabledMetaLink.at(index.row()) == true) {

                return Qt::Checked;

            }// if
            else {

                return Qt::Unchecked;

            }// else

        }//if

        if(index.column() == 7) {

            if(enabledGPGCheck.at(index.row()) == true) {

                return Qt::Checked;

            }// if
            else {

                return Qt::Unchecked;

            }// else

        }// if

        if(index.column() == 9) {

            if(enabledGPGKey.at(index.row()) == true) {

                return Qt::Checked;

            }// if
            else {

                return Qt::Unchecked;

            }// else

        }// if

        if(index.column() == 11) {

            if(enabledProxy.at(index.row()) == true) {

                return Qt::Checked;

            }// if
            else {

                return Qt::Unchecked;

            }// else

        }// if

        if(index.column() == 13) {

            if(enabledProxy.at(index.row()) == true) {

                if(enabledProxyUsername.at(index.row()) == true) {

                    return Qt::Checked;

                }// if
                else {

                    return Qt::Unchecked;

                }// else

            }// if
            else {

                if(!DataFlow::getRepoDataList().at(index.row()).repoProxyUsername[0].isEmpty()) {

                    QVariant myValue[3];

                    myValue[0] = "0";

                    DataFlow::setRepoDataList(index.row(), RepoData::ProxyUsername, 0, myValue);

                }// if

                return Qt::Unchecked;

            }// else

        }// if

        if(index.column() == 15) {

            if(enabledProxy.at(index.row()) == true) {

                if(enabledProxyPassword.at(index.row()) == true) {

                    return Qt::Checked;

                }// if
                else {

                    return Qt::Unchecked;

                }// else

            }// if
            else {

                if(!DataFlow::getRepoDataList().at(index.row()).repoProxyPassword[0].isEmpty()) {

                    QVariant myValue[3];

                    myValue[0] = "0";

                    DataFlow::setRepoDataList(index.row(), RepoData::ProxyPassword, 0, myValue);

                }// if

                return Qt::Unchecked;

            }// else

        }// if

        if(index.column() == 17) {

            if(enabledBandwidth.at(index.row()) == true) {

                return Qt::Checked;

            }// if
            else {

                return Qt::Unchecked;

            }// else

        }// if

        if(index.column() == 19) {

            if(enabledSkipIfUnavailable.at(index.row()) == true) {

                return Qt::Checked;

            }// if
            else {

                return Qt::Unchecked;

            }// else

        }// if

        if(index.column() == 21) {

            if(enabledFailoverMethod.at(index.row()) == true) {

                return Qt::Checked;

            }// if
            else {

                return Qt::Unchecked;

            }// else

        }// if

        if(index.column() == 23) {

            if(enabledMetadataExpire.at(index.row()) == true) {

                return Qt::Checked;

            }// if
            else {

                return Qt::Unchecked;

            }// else

        }// if

        if(index.column() == 25) {

            if(enabledParallelDownload.at(index.row()) == true) {

                return Qt::Checked;

            }// if
            else {

                return Qt::Unchecked;

            }// else

        }// if

        if(index.column() == 27) {

            if(enabledInclude.at(index.row()) == true) {

                return Qt::Checked;

            }// if
            else {

                return Qt::Unchecked;

            }// else

        }// if

        if(index.column() == 29) {

            if(enabledExclude.at(index.row()) == true) {

                return Qt::Checked;

            }// if
            else {

                return Qt::Unchecked;

            }// else

        }// if

    }// if

    if(role == Qt::TextAlignmentRole && (index.column() == 8 || index.column() == 12 || index.column() == 18 || index.column() == 20 || index.column() == 22 || index.column() == 24 || index.column() == 26)) {

        return Qt::AlignCenter;

    }// if

    if(role == Qt::BackgroundRole) {

        if((index.column() < 3)) {

            if(enabledState.at(index.row()) == true) {

                return YUMRepoManager::colorEnabled();

            }// if
            else {

                return YUMRepoManager::colorDisabled();

            }// else

        }// if

        if(index.column() == 3 || index.column() == 4) {

            if((enabledBaseURL.at(index.row()) == true) && (enabledState.at(index.row()) == true)) {

               return YUMRepoManager::colorEnabled();

            }// if
            else {

               return YUMRepoManager::colorDisabled();

            }// else

        }// if

        if(index.column() == 5 || index.column() == 6) {

            if((enabledMetaLink.at(index.row()) == true) && (enabledState.at(index.row()) == true)) {

               return YUMRepoManager::colorEnabled();

            }// if
            else {

               return YUMRepoManager::colorDisabled();

            }// else

        }// if

        if(index.column() == 7 || index.column() == 8) {

            if((enabledGPGCheck.at(index.row()) == true) && (enabledState.at(index.row()) == true)) {

               return YUMRepoManager::colorEnabled();

            }// if
            else {

               return YUMRepoManager::colorDisabled();

            }// else

        }// if

        if(index.column() == 9 || index.column() == 10) {

            if((enabledGPGKey.at(index.row()) == true) && (enabledState.at(index.row()) == true)) {

               return YUMRepoManager::colorEnabled();

            }// if
            else {

               return YUMRepoManager::colorDisabled();

            }// else

        }// if

        if(index.column() == 11 || index.column() == 12) {

            if((enabledProxy.at(index.row()) == true) && (enabledState.at(index.row()) == true)) {

               return YUMRepoManager::colorEnabled();

            }// if
            else {

               return YUMRepoManager::colorDisabled();

            }// else

        }// if

        if(index.column() == 13 || index.column() == 14) {

            if(( (enabledProxyUsername.at(index.row()) == true) && (enabledProxy.at(index.row()) == true) ) && (enabledState.at(index.row()) == true)) {

               return YUMRepoManager::colorEnabled();

            }// if
            else {

               return YUMRepoManager::colorDisabled();

            }// else

        }// if

        if(index.column() == 15 || index.column() == 16) {

            if(( (enabledProxyPassword.at(index.row()) == true) && (enabledProxy.at(index.row()) == true) ) && (enabledState.at(index.row()) == true)) {

               return YUMRepoManager::colorEnabled();

            }// if
            else {

               return YUMRepoManager::colorDisabled();

            }// else

        }// if

        if(index.column() == 17 || index.column() == 18) {

            if((enabledBandwidth.at(index.row()) == true) && (enabledState.at(index.row()) == true)) {

               return YUMRepoManager::colorEnabled();

            }// if
            else {

               return YUMRepoManager::colorDisabled();

            }// else

        }// if

        if(index.column() == 19 || index.column() == 20) {

            if((enabledSkipIfUnavailable.at(index.row()) == true) && (enabledState.at(index.row()) == true)) {

               return YUMRepoManager::colorEnabled();

            }// if
            else {

               return YUMRepoManager::colorDisabled();

            }// else

        }// if

        if(index.column() == 21 || index.column() == 22) {

            if((enabledFailoverMethod.at(index.row()) == true) && (enabledState.at(index.row()) == true)) {

               return YUMRepoManager::colorEnabled();

            }// if
            else {

               return YUMRepoManager::colorDisabled();

            }// else

        }// if

        if(index.column() == 23 || index.column() == 24) {

            if((enabledMetadataExpire.at(index.row()) == true) && (enabledState.at(index.row()) == true)) {

               return YUMRepoManager::colorEnabled();

            }// if
            else {

               return YUMRepoManager::colorDisabled();

            }// else

        }// if

        if(index.column() == 25 || index.column() == 26) {

            if((enabledParallelDownload.at(index.row()) == true) && (enabledState.at(index.row()) == true)) {

               return YUMRepoManager::colorEnabled();

            }// if
            else {

               return YUMRepoManager::colorDisabled();

            }// else

        }// if

        if(index.column() == 27 || index.column() == 28) {

            if((enabledInclude.at(index.row()) == true) && (enabledState.at(index.row()) == true)) {

               return YUMRepoManager::colorEnabled();

            }// if
            else {

               return YUMRepoManager::colorDisabled();

            }// else

        }// if

        if(index.column() == 29 || index.column() == 30) {

            if((enabledExclude.at(index.row()) == true) && (enabledState.at(index.row()) == true)) {

               return YUMRepoManager::colorEnabled();

            }// if
            else {

               return YUMRepoManager::colorDisabled();

            }// else

        }// if

    }// if

    if(role == Qt::ToolTipRole) {

        if(index.column() == 1) {

            QString tip, columnName;

            tip = "<table>";

            columnName = headerData(index.column(), Qt::Horizontal, Qt::DisplayRole).toString();

            tip += QString("<tr><td><b>%1:</b></td></tr> <tr><td>The name that identify the repository.</td></tr>").arg(columnName);

            tip += "</table>";

            return tip;

        }// if

        if(index.column() == 2) {

            QString tip, columnName;

            tip = "<table>";

            columnName = headerData(index.column(), Qt::Horizontal, Qt::DisplayRole).toString();

            tip += QString("<tr><td><b>%1:</b></td></tr> <tr><td>A comment about the repository.</td></tr>").arg(columnName);

            tip += "</table>";

            return tip;

        }// if

        if(index.column() == 4) {

            QString tip, columnName;

            tip = "<table>";

            columnName = headerData(index.column(), Qt::Horizontal, Qt::DisplayRole).toString();

            tip += QString("<tr><td><b>%1:</b></td></tr> <tr><td>Must be a URL to the directory where the yum repository's \"repodata\" directory lives."
                           " Can be an http://, ftp:// or file:// URL. You can specify multiple URLs in one baseurl statement.</td></tr>").arg(columnName);

            tip += "</table>";

            return tip;

        }// if

        if(index.column() == 6) {

            QString tip, columnName;

            tip = "<table>";

            columnName = headerData(index.column(), Qt::Horizontal, Qt::DisplayRole).toString();

            tip += QString("<tr><td><b>%1:</b></td></tr> <tr><td>Specifies  a  URL to a file containing a list of baseurls. This can be used instead of or with the <b>baseurl</b> option.</td></tr>").arg(columnName);

            tip += "</table>";

            return tip;

        }// if

        if(index.column() == 8) {

            QString tip, columnName;

            tip = "<table>";

            columnName = headerData(index.column(), Qt::Horizontal, Qt::DisplayRole).toString();

            tip += QString("<tr><td><b>%1:</b></td></tr> <tr><td>This tells yum whether or not it should perform a GPG signature check on the packages gotten from the repository.</td></tr>").arg(columnName);

            tip += "</table>";

            return tip;

        }// if

        if(index.column() == 10) {

            QString tip, columnName;

            tip = "<table>";

            columnName = headerData(index.column(), Qt::Horizontal, Qt::DisplayRole).toString();

            tip += QString("<tr><td><b>%1:</b></td></tr> <tr><td>A URL pointing to the ASCII-armoDisabled GPG key file for the repository. This option is used if yum needs a public key to verify a package"
                           " and the requiDisabled key hasn't been imported into the RPM database. If this option is set, yum will automatically import the key from  the"
                           " specified URL. You will be prompted before the key is installed unless the <b>assumeyes</b> option is set.</td></tr>").arg(columnName);

            tip += "</table>";

            return tip;

        }// if

        if(index.column() == 12) {

            QString tip, columnName;

            tip = "<table>";

            columnName = headerData(index.column(), Qt::Horizontal, Qt::DisplayRole).toString();

            tip += QString("<tr><td><b>%1:</b></td></tr> <tr><td>URL  to  the  proxy  server for this repository. If this is unset it inherits it from the global setting.</td></tr>").arg(columnName);

            tip += "</table>";

            return tip;

        }// if

        if(index.column() == 14) {

            QString tip, columnName;

            tip = "<table>";

            columnName = headerData(index.column(), Qt::Horizontal, Qt::DisplayRole).toString();

            tip += QString("<tr><td><b>%1:</b></td></tr> <tr><td>Username to use for proxy. If this is unset it inherits it from the global setting.</td></tr>").arg(columnName);

            tip += "</table>";

            return tip;

        }// if

        if(index.column() == 16) {

            QString tip, columnName;

            tip = "<table>";

            columnName = headerData(index.column(), Qt::Horizontal, Qt::DisplayRole).toString();

            tip += QString("<tr><td><b>%1:</b></td></tr> <tr><td>Password for the proxy. If this is unset it inherits it from the global setting.</td></tr>").arg(columnName);

            tip += "</table>";

            return tip;

        }// if

        if(index.column() == 18) {

            QString tip, columnName;

            tip = "<table>";

            columnName = headerData(index.column(), Qt::Horizontal, Qt::DisplayRole).toString();

            tip += QString("<tr><td><b>%1:</b></td></tr> <tr><td>Enable bandwidth throttling for downloads. This option can be expressed as a absolute data rate in bytes/sec."
                           " An SI prefix (k, M or G) may be appended to the bandwidth value (eg. \"5.5k\" is 5.5 kilobytes/sec, \"2M\" is 2 Megabytes/sec).</td></tr>").arg(columnName);

            tip += "</table>";

            return tip;

        }// if

        if(index.column() == 20) {

            QString tip, columnName;

            tip = "<table>";

            columnName = headerData(index.column(), Qt::Horizontal, Qt::DisplayRole).toString();

            tip += QString("<tr><td><b>%1:</b></td></tr> <tr><td>If set to Yes yum will continue running if this repository cannot be contacted for any  reason.</td></tr>").arg(columnName);

            tip += "</table>";

            return tip;

        }// if

        if(index.column() == 22) {

            QString tip, columnName;

            tip = "<table>";

            columnName = headerData(index.column(), Qt::Horizontal, Qt::DisplayRole).toString();

            tip += QString("<tr><td><b>%1:</b></td></tr> <tr><td>Either \"roundrobin\" or \"priority\".</td></tr>"
                           "<tr><td><b>roundrobin</b> randomly selects a URL out of the list of URLs to start with and proceeds through each of them as it encounters a failure contacting the host.</b></td></tr>"
                           "<tr><td><b>priority</b> starts from the first baseurl listed and reads through them sequentially.</td></tr>").arg(columnName);

            tip += "</table>";

            return tip;

        }// if

        if(index.column() == 24) {

            QString tip, columnName;

            tip = "<table>";

            columnName = headerData(index.column(), Qt::Horizontal, Qt::DisplayRole).toString();

            tip += QString("<tr><td><b>%1:</b></td></tr> <tr><td>Time  (in seconds) after which the metadata will expire. So that if the current metadata downloaded is less than this many"
                           " seconds old then yum will not update the metadata against the repository. If you find that yum is not downloading information on updates"
                           " as often as you would like lower the value of this option. You can also change from the default of using seconds to using days, hours or"
                           " minutes by appending a d, h or m respectively. The default is 6 hours, to compliment yum-updatesd running once an hour. It's also possible to use the word \"never\","
                           " meaning that the metadata will never expire.</td></tr>").arg(columnName);

            tip += "</table>";

            return tip;

        }// if

        if(index.column() == 26) {

            QString tip, columnName;

            tip = "<table>";

            columnName = headerData(index.column(), Qt::Horizontal, Qt::DisplayRole).toString();

            tip += QString("<tr><td><b>%1:</b></td></tr> <tr><td>If set to Yes Yum will download packages and metadata from this repo in parallel, if possible.</td></tr>").arg(columnName);

            tip += "</table>";

            return tip;

        }// if

        if(index.column() == 28) {

            QString tip, columnName;

            tip = "<table>";

            columnName = headerData(index.column(), Qt::Horizontal, Qt::DisplayRole).toString();

            tip += QString("<tr><td><b>%1:</b></td></tr> <tr><td>Inverse of exclude. This is a list of packages you want to use from a repository."
                           " If this option lists only one package then that is all yum will ever see from the repository.</td></tr>").arg(columnName);

            tip += "</table>";

            return tip;

        }// if

        if(index.column() == 30) {

            QString tip, columnName;

            tip = "<table>";

            columnName = headerData(index.column(), Qt::Horizontal, Qt::DisplayRole).toString();

            tip += QString("<tr><td><b>%1:</b></td></tr> <tr><td>List of packages to exclude from updates or installs. This should be a space separated list."
                           " Shell globs using wildcards (eg. * and ?) are allowed.</td></tr>").arg(columnName);

            tip += "</table>";

            return tip;

        }// if

    }// if

    return QVariant();

}// data

bool RepoDataModel::setData(const QModelIndex &index, const QVariant &value, int role) {

    if(!index.isValid()) {

        return false;

    }// if

    if(role == Qt::DisplayRole || role == Qt::EditRole) {

        if(index.column() == 1) {

            if(value.toString().isEmpty()) {

                return false;

            }// if

            QVariant myValue[3];

            myValue[0] = value;

            DataFlow::setRepoDataList(index.row(), RepoData::Name, 0, myValue);

            if(DataFlow::getRepoDataList().at(index.row()).repoComment[1].isEmpty()) {

                QVariant myValueComment[3];

                myValueComment[1] = DataFlow::getRepoDataList().at(index.row()).repoName;

                DataFlow::setRepoDataList(index.row(), RepoData::Comment, 1, myValueComment);

                if(DataFlow::getRepoDataList().at(index.row()).repoComment[0].isEmpty()) {

                    myValue[0] = "name";

                    DataFlow::setRepoDataList(index.row(), RepoData::Comment, 0, myValue);

                }// if

            }// if

            emit dataChanged(index, index);

            return true;

        }// if

        if(index.column() == 2) {

            QVariant myValue[2];

            if(!value.toString().isEmpty()) {

                myValue[1] = value.toString();

            }// if
            else {

                myValue[1] = DataFlow::getRepoDataList().at(index.row()).repoName;

            }// else

            DataFlow::setRepoDataList(index.row(), RepoData::Comment, 1, myValue);

            if (DataFlow::getRepoDataList().at(index.row()).repoComment[0].isEmpty()) {

                myValue[0] = "name";

                DataFlow::setRepoDataList(index.row(), RepoData::Comment, 0, myValue);

            }// if

            emit dataChanged(index, index);

            return true;

        }// if

        if(index.column() == 4) {

            QVariant myValue[3];

            if(!value.toString().isEmpty()) {

                QUrl url(value.toString());

                if(url.scheme().isEmpty()) {

                        YUMRepoManager::showMessage("Error", tr("The protocol of the URL has not be correctly specified."));

                        return false;

                }// if

                if(url.host().isEmpty()) {

                    if(!value.toString().startsWith("file:/")) {

                        YUMRepoManager::showMessage("Error", tr("The hostname or ip address has not be correctly specified."));

                        return false;

                    }// if

                }// if

                myValue[2] = value.toString();

                DataFlow::setRepoDataList(index.row(), RepoData::BaseURL, 2, myValue);

                if (DataFlow::getRepoDataList().at(index.row()).repoBaseURL[1].isEmpty()) {

                    myValue[1] = "baseurl";

                    DataFlow::setRepoDataList(index.row(), RepoData::BaseURL, 1, myValue);

                }// if

                if(enabledMetaLink[index.row()] == false) {

                    myValue[0] = "1";

                    DataFlow::setRepoDataList(index.row(), RepoData::BaseURL, 0, myValue);

                    enabledBaseURL[index.row()] = true;

                }// if
                else {

                    myValue[0] = "0";

                    DataFlow::setRepoDataList(index.row(), RepoData::BaseURL, 0, myValue);

                }// else

                emit dataChanged(index, index);

                return true;

            }// if
            else {

                QVariant myValue[3];

                myValue[0] = "0";
                myValue[2] = "";

                DataFlow::setRepoDataList(index.row(), RepoData::BaseURL, 0, myValue);
                DataFlow::setRepoDataList(index.row(), RepoData::BaseURL, 2, myValue);

                enabledBaseURL[index.row()] = false;

                emit dataChanged(index, index);

                return true;

            }// else if

            return false;

        }// if

        if(index.column() == 6) {

            QVariant myValue[3];

            if(!value.toString().isEmpty()) {

                QUrl url(value.toString());

                if(url.scheme().isEmpty()) {

                        YUMRepoManager::showMessage("Error", tr("The protocol of the URL has not be correctly specified."));

                        return false;

                }// if

                if(url.host().isEmpty()) {

                    if(!value.toString().startsWith("file:/")) {

                        YUMRepoManager::showMessage("Error", tr("The hostname or ip address has not be correctly specified."));

                        return false;

                    }// if

                }// if

                myValue[2] = value.toString();

                DataFlow::setRepoDataList(index.row(), RepoData::MetaLink, 2, myValue);

                if (DataFlow::getRepoDataList().at(index.row()).repoMetaLink[1].isEmpty()) {

                    myValue[1] = "metalink";

                    DataFlow::setRepoDataList(index.row(), RepoData::MetaLink, 1, myValue);

                }// if

                if(enabledBaseURL[index.row()] == false) {

                    myValue[0] = "1";

                    DataFlow::setRepoDataList(index.row(), RepoData::MetaLink, 0, myValue);

                    enabledMetaLink[index.row()] = true;

                }// if
                else {

                    myValue[0] = "0";

                    DataFlow::setRepoDataList(index.row(), RepoData::MetaLink, 0, myValue);

                }

                emit dataChanged(index, index);

                return true;

            }// if
            else {

                QVariant myValue[3];

                myValue[0] = "0";
                myValue[2] = "";

                DataFlow::setRepoDataList(index.row(), RepoData::MetaLink, 0, myValue);
                DataFlow::setRepoDataList(index.row(), RepoData::MetaLink, 2, myValue);

                enabledMetaLink[index.row()] = false;

                emit dataChanged(index, index);

                return true;

            }// else if

            return false;

        }// if

        if(index.column() == 8) {

            QVariant myValue[3];

            if(value.toString() == "Yes" || value.toString() == YUMRepoManager::currentTranslator->translate("RepoDataDelegate", "Yes")) {

                myValue[2] = "1";

                DataFlow::setRepoDataList(index.row(), RepoData::GPGCheck, 2, myValue);

            }// if
            else {

                myValue[2] = "0";

                DataFlow::setRepoDataList(index.row(), RepoData::GPGCheck, 2, myValue);

            }// else

            if (DataFlow::getRepoDataList().at(index.row()).repoGPGCheck[0].isEmpty()) {

                myValue[0] = "1";
                myValue[1] = "gpgcheck";

                DataFlow::setRepoDataList(index.row(), RepoData::GPGCheck, 0, myValue);
                DataFlow::setRepoDataList(index.row(), RepoData::GPGCheck, 1, myValue);

            }// if

            enabledGPGCheck[index.row()] = true;

            emit dataChanged(index, index);

            return true;

        }// if

        if(index.column() == 10) {

            QVariant myValue[3];

            if(!value.toString().isEmpty()) {

                QUrl url(value.toString());

                if(url.scheme().isEmpty()) {

                        YUMRepoManager::showMessage("Error", tr("The protocol of the URL has not be correctly specified."));

                        return false;

                }// if

                if(url.host().isEmpty()) {

                    if(!value.toString().startsWith("file:/")) {

                        YUMRepoManager::showMessage("Error", tr("The hostname or ip address has not be correctly specified."));

                        return false;

                    }// if

                }// if

                myValue[2] = value.toString();

                DataFlow::setRepoDataList(index.row(), RepoData::GPGKey, 2, myValue);

                if (DataFlow::getRepoDataList().at(index.row()).repoGPGKey[0].isEmpty()) {

                    myValue[0] = "1";
                    myValue[1] = "gpgkey";

                    DataFlow::setRepoDataList(index.row(), RepoData::GPGKey, 0, myValue);
                    DataFlow::setRepoDataList(index.row(), RepoData::GPGKey, 1, myValue);

                }// if

                enabledGPGKey[index.row()] = true;

                emit dataChanged(index, index);

                return true;

            }// if
            else if(enabledGPGKey[index.row()] == true) {

                myValue[0] = "0";
                myValue[2] = "";

                DataFlow::setRepoDataList(index.row(), RepoData::GPGKey, 0, myValue);
                DataFlow::setRepoDataList(index.row(), RepoData::GPGKey, 2, myValue);

                enabledGPGKey[index.row()] = false;

                emit dataChanged(index, index);

                return true;

            }// else if

            return false;

        }// if

        if(index.column() == 12) {

            QVariant myValue[3];

            if(!value.toString().isEmpty()) {

                myValue[2] = value.toString();

                DataFlow::setRepoDataList(index.row(), RepoData::Proxy, 2, myValue);

                if (DataFlow::getRepoDataList().at(index.row()).repoProxy[0].isEmpty()) {

                    myValue[0] = "1";
                    myValue[1] = "proxy";

                    DataFlow::setRepoDataList(index.row(), RepoData::Proxy, 0, myValue);
                    DataFlow::setRepoDataList(index.row(), RepoData::Proxy, 1, myValue);

                }// if

                enabledProxy[index.row()] = true;

                emit dataChanged(index, index);

                return true;

            }// if
            else if(enabledProxy[index.row()] == true) {

                myValue[0] = "";
                myValue[1] = "";
                myValue[2] = "";

                DataFlow::setRepoDataList(index.row(), RepoData::ProxyUsername, 2, myValue);

                DataFlow::setRepoDataList(index.row(), RepoData::Proxy, 0, myValue);
                DataFlow::setRepoDataList(index.row(), RepoData::Proxy, 1, myValue);
                DataFlow::setRepoDataList(index.row(), RepoData::Proxy, 2, myValue);

                DataFlow::setRepoDataList(index.row(), RepoData::ProxyUsername, 0, myValue);
                DataFlow::setRepoDataList(index.row(), RepoData::ProxyUsername, 1, myValue);
                DataFlow::setRepoDataList(index.row(), RepoData::ProxyUsername, 2, myValue);

                DataFlow::setRepoDataList(index.row(), RepoData::ProxyPassword, 0, myValue);
                DataFlow::setRepoDataList(index.row(), RepoData::ProxyPassword, 1, myValue);
                DataFlow::setRepoDataList(index.row(), RepoData::ProxyPassword, 2, myValue);

                enabledProxy[index.row()] = false;
                enabledProxyUsername[index.row()] = false;
                enabledProxyPassword[index.row()] = false;

                emit dataChanged(index, index);

                return true;

            }// else if

            return false;

        }// if

        if(index.column() == 14) {

            QVariant myValue[3];

            if(!value.toString().isEmpty()) {

                if(enabledProxy.at(index.row()) == true) {

                    myValue[2] = value.toString();

                    DataFlow::setRepoDataList(index.row(), RepoData::ProxyUsername, 2, myValue);

                    if (DataFlow::getRepoDataList()[index.row()].repoProxyUsername[0].isEmpty()) {

                        myValue[0] = "1";
                        myValue[1] = "proxy_username";

                        DataFlow::setRepoDataList(index.row(), RepoData::ProxyUsername, 0, myValue);
                        DataFlow::setRepoDataList(index.row(), RepoData::ProxyUsername, 1, myValue);

                    }// if

                    enabledProxyUsername[index.row()] = true;

                    emit dataChanged(index, index);

                    return true;

                }// if
                else {

                    return false;

                }// else

            }// if
            else if(enabledProxyUsername[index.row()] == true) {

                myValue[0] = "0";
                myValue[2] = "";

                DataFlow::setRepoDataList(index.row(), RepoData::ProxyUsername, 0, myValue);
                DataFlow::setRepoDataList(index.row(), RepoData::ProxyUsername, 2, myValue);

                enabledProxyUsername[index.row()] = false;
                enabledProxyPassword[index.row()] = false;

                emit dataChanged(index, index);

                return true;

            }// else if

            return false;

        }// if

        if(index.column() == 16) {

            QVariant myValue[3];

            if(!value.toString().isEmpty()) {

                if(enabledProxyUsername.at(index.row()) == true) {

                    myValue[2] = value.toString();

                    DataFlow::setRepoDataList(index.row(), RepoData::ProxyPassword, 2, myValue);

                    if (DataFlow::getRepoDataList().at(index.row()).repoProxyPassword[0].isEmpty()) {

                        myValue[0] = "1";
                        myValue[1] = "proxy_password";

                        DataFlow::setRepoDataList(index.row(), RepoData::ProxyPassword, 0, myValue);
                        DataFlow::setRepoDataList(index.row(), RepoData::ProxyPassword, 1, myValue);

                    }// if

                    enabledProxyPassword[index.row()] = true;

                    emit dataChanged(index, index);

                    return true;

                }// if
                else {

                    return false;

                }// else

            }// if
            else if(enabledProxyPassword[index.row()] == true) {

                myValue[0] = "0";
                myValue[2] = "";

                DataFlow::setRepoDataList(index.row(), RepoData::ProxyPassword, 0, myValue);
                DataFlow::setRepoDataList(index.row(), RepoData::ProxyPassword, 2, myValue);

                enabledProxyPassword[index.row()] = false;

                emit dataChanged(index, index);

                return true;

            }// else if

            return false;

        }// if

        if(index.column() == 18) {

            QVariant myValue[3];

            if(!value.toString().isEmpty()) {

                QStringList amountAndBandwidth = value.toString().split(" ");

                QString bandwidthValue;

                if(amountAndBandwidth.at(1) == "Gigabytes") {

                    bandwidthValue = amountAndBandwidth.at(0) + "G";

                }// if
                else if(amountAndBandwidth.at(1) == "Megabytes") {

                    bandwidthValue = amountAndBandwidth.at(0) + "M";

                }// else if
                else if(amountAndBandwidth.at(1) == "Kilobytes") {

                    bandwidthValue = amountAndBandwidth.at(0) + "K";

                }// else if

                myValue[2] = bandwidthValue;

                DataFlow::setRepoDataList(index.row(), RepoData::Speed, 2, myValue);

                if (DataFlow::getRepoDataList()[index.row()].repoBandwidth[0].isEmpty()) {

                    myValue[0] = "1";
                    myValue[1] = "throttle";

                    DataFlow::setRepoDataList(index.row(), RepoData::Speed, 0, myValue);
                    DataFlow::setRepoDataList(index.row(), RepoData::Speed, 1, myValue);

                }// if

                enabledBandwidth[index.row()] = true;

                emit dataChanged(index, index);

                return true;

            }// if
            else if(enabledBandwidth[index.row()] == true) {

                myValue[0] = "0";
                myValue[2] = "";

                DataFlow::setRepoDataList(index.row(), RepoData::Speed, 0, myValue);
                DataFlow::setRepoDataList(index.row(), RepoData::Speed, 2, myValue);

                enabledBandwidth[index.row()] = false;

                emit dataChanged(index, index);

                return true;

            }// else if

            return false;

        }// if

        if(index.column() == 20) {

            QVariant myValue[3];

            if(value.toString() == "Yes" || value.toString() == YUMRepoManager::currentTranslator->translate("RepoDataDelegate", "Yes")) {

               myValue[2] = "1";

               DataFlow::setRepoDataList(index.row(), RepoData::SkipIfUnavailable, 2, myValue);

            }// if
            else {

               myValue[2] = "0";

               DataFlow::setRepoDataList(index.row(), RepoData::SkipIfUnavailable, 2, myValue);

            }// else

            if (DataFlow::getRepoDataList().at(index.row()).repoSkipIfUnavailable[0].isEmpty()) {

                myValue[0] = "1";
                myValue[1] = "skip_if_unavailable";

                DataFlow::setRepoDataList(index.row(), RepoData::SkipIfUnavailable, 0, myValue);
                DataFlow::setRepoDataList(index.row(), RepoData::SkipIfUnavailable, 1, myValue);

            }// if

            enabledSkipIfUnavailable[index.row()] = true;

            emit dataChanged(index, index);

            return true;

        }// if

        if(index.column() == 22) {

            QVariant myValue[3];

            myValue[2] = value.toString();

            DataFlow::setRepoDataList(index.row(), RepoData::FailoverMethod, 2, myValue);

            if (DataFlow::getRepoDataList().at(index.row()).repoFailoverMethod[0].isEmpty()) {

                myValue[0] = "1";
                myValue[1] = "failovermethod";

                DataFlow::setRepoDataList(index.row(), RepoData::FailoverMethod, 0, myValue);
                DataFlow::setRepoDataList(index.row(), RepoData::FailoverMethod, 1, myValue);

            }// if

            enabledFailoverMethod[index.row()] = true;

            emit dataChanged(index, index);

            return true;

        }// if

        if(index.column() == 24) {

            QVariant myValue[3];

            if(!value.toString().isEmpty()) {

                QStringList amountAndFrequency = value.toString().split(" ");

                QString metadataExpireValue;

                if(amountAndFrequency.at(1) == "Day/s" || amountAndFrequency.at(1) == YUMRepoManager::currentTranslator->translate("RepoDataModel", "Day/s")) {

                    metadataExpireValue = amountAndFrequency.at(0) + "d";

                }// if
                else if(amountAndFrequency.at(1) == "Hour/s" || amountAndFrequency.at(1) == YUMRepoManager::currentTranslator->translate("RepoDataModel", "Hour/s")) {

                    metadataExpireValue = amountAndFrequency.at(0) + "h";

                }// else if
                else if(amountAndFrequency.at(1) == "Minute/s" || amountAndFrequency.at(1) == YUMRepoManager::currentTranslator->translate("RepoDataModel", "Minute/s")) {

                    metadataExpireValue = amountAndFrequency.at(0) + "m";

                }// else if

                myValue[2] = metadataExpireValue;

                DataFlow::setRepoDataList(index.row(), RepoData::MetadataExpire, 2, myValue);

                if(DataFlow::getRepoDataList().at(index.row()).repoMetadataExpire[0].isEmpty()) {

                    myValue[0] = "1";
                    myValue[1] = "metadata_expire";

                    DataFlow::setRepoDataList(index.row(), RepoData::MetadataExpire, 0, myValue);
                    DataFlow::setRepoDataList(index.row(), RepoData::MetadataExpire, 1, myValue);

                }// if

                enabledMetadataExpire[index.row()] = true;

                emit dataChanged(index, index);

                return true;

            }// if
            else if(enabledMetadataExpire[index.row()] == true) {

                myValue[0] = "0";
                myValue[2] = "";

                DataFlow::setRepoDataList(index.row(), RepoData::MetadataExpire, 0, myValue);
                DataFlow::setRepoDataList(index.row(), RepoData::MetadataExpire, 2, myValue);

                enabledMetadataExpire[index.row()] = false;

                emit dataChanged(index, index);

                return true;

            }// else if

            return false;

        }// if

        if(index.column() == 26) {

            QVariant myValue[3];

            if(!value.toString().isEmpty()) {

                if(value.toString() == "Yes" || value.toString() == YUMRepoManager::currentTranslator->translate("RepoDataDelegate", "Yes")) {

                    myValue[2] = "1";

                    DataFlow::setRepoDataList(index.row(), RepoData::ParallelDownload, 2, myValue);

                }// if
                else {

                    myValue[2] = "0";

                    DataFlow::setRepoDataList(index.row(), RepoData::ParallelDownload, 2, myValue);

                }// else

                if (DataFlow::getRepoDataList().at(index.row()).repoParallelDownload[0].isEmpty()) {

                    myValue[0] = "1";
                    myValue[1] = "async";

                    DataFlow::setRepoDataList(index.row(), RepoData::ParallelDownload, 0, myValue);
                    DataFlow::setRepoDataList(index.row(), RepoData::ParallelDownload, 1, myValue);

                }// if

                enabledParallelDownload[index.row()] = true;

            }// if
            else {

                myValue[0] = "0";
                myValue[2] = "";

                DataFlow::setRepoDataList(index.row(), RepoData::ParallelDownload, 0, myValue);
                DataFlow::setRepoDataList(index.row(), RepoData::ParallelDownload, 2, myValue);

                enabledParallelDownload[index.row()] = false;

                emit dataChanged(index, index);

                return true;

            }// else

            emit dataChanged(index, index);

            return true;

        }// if

        if(index.column() == 28) {

            QVariant myValue[3];

            if(!value.toString().isEmpty()) {

                if((!DataFlow::getRepoDataList()[index.row()].repoExclude[2].isEmpty()) && (enabledExclude.at(index.row()) == true)) {

                    YUMRepoManager::showMessage("Error", tr("Exclude and Include options can not be used at the same time."));

                    return false;

                }// if

                myValue[2] = value.toString();

                DataFlow::setRepoDataList(index.row(), RepoData::Include, 2, myValue);

                if (DataFlow::getRepoDataList().at(index.row()).repoInclude[0].isEmpty()) {

                    myValue[0] = "1";
                    myValue[1] = "includepkgs";

                    DataFlow::setRepoDataList(index.row(), RepoData::Include, 0, myValue);
                    DataFlow::setRepoDataList(index.row(), RepoData::Include, 1, myValue);

                }// if

                enabledInclude[index.row()] = true;

                emit dataChanged(index, index);

                return true;

            }// if
            else if(enabledInclude[index.row()] == true) {

                myValue[0] = "0";
                myValue[2] = "";

                DataFlow::setRepoDataList(index.row(), RepoData::Include, 0, myValue);
                DataFlow::setRepoDataList(index.row(), RepoData::Include, 2, myValue);

                enabledInclude[index.row()] = false;

                emit dataChanged(index, index);

                return true;

            }// else if

            return false;

        }// if

        if(index.column() == 30) {

            QVariant myValue[3];

            if(!value.toString().isEmpty()) {

                if((!DataFlow::getRepoDataList()[index.row()].repoInclude[2].isEmpty()) && (enabledInclude.at(index.row()) == true)) {

                    YUMRepoManager::showMessage("Error", tr("Include and Exclude options can not be used at the same time."));

                    return false;

                }// if

                myValue[2] = value.toString();

                DataFlow::setRepoDataList(index.row(), RepoData::Exclude, 2, myValue);

                if (DataFlow::getRepoDataList().at(index.row()).repoExclude[0].isEmpty()) {

                    myValue[0] = "1";
                    myValue[1] = "exclude";

                    DataFlow::setRepoDataList(index.row(), RepoData::Exclude, 0, myValue);
                    DataFlow::setRepoDataList(index.row(), RepoData::Exclude, 1, myValue);

                }// if

                enabledExclude[index.row()] = true;

                emit dataChanged(index, index);

                return true;

            }// if
            else if(enabledExclude[index.row()] == true) {

                myValue[0] = "0";
                myValue[2] = "";

                DataFlow::setRepoDataList(index.row(), RepoData::Exclude, 0, myValue);
                DataFlow::setRepoDataList(index.row(), RepoData::Exclude, 2, myValue);

                enabledExclude[index.row()] = false;

                emit dataChanged(index, index);

                return true;

            }// else if

            return false;

        }// if

    }// if

    if(role == Qt::CheckStateRole) {

        if(index.column() == 0) {

            QVariant myValue[3];

            enabledState[index.row()] = !enabledState[index.row()];

            if(enabledState[index.row()] == true) {

                myValue[1] = "1";

                DataFlow::setRepoDataList(index.row(), RepoData::Enabled, 1, myValue);

            }// if
            else {

                myValue[1] = "0";

                DataFlow::setRepoDataList(index.row(), RepoData::Enabled, 1, myValue);

            }// else

            emit dataChanged(index, index);

            return true;

        }// if

        if(index.column() == 3) {

            QVariant myValue[3];

            enabledBaseURL[index.row()] = !enabledBaseURL[index.row()];

            if(enabledBaseURL[index.row()] == true) {

                myValue[0] = "1";

                DataFlow::setRepoDataList(index.row(), RepoData::BaseURL, 0, myValue);

                if (!DataFlow::getRepoDataList().at(index.row()).repoMetaLink[0].isEmpty()) {

                    myValue[0] = "0";

                    DataFlow::setRepoDataList(index.row(), RepoData::MetaLink, 0, myValue);

                    enabledMetaLink[index.row()] = false;

                }// if

            }// if

            emit dataChanged(index, index);

            return true;

        }// if

        if(index.column() == 5) {

            QVariant myValue[3];

            enabledMetaLink[index.row()] = !enabledMetaLink[index.row()];

            if(enabledMetaLink[index.row()] == true) {

                myValue[0] = "1";

                DataFlow::setRepoDataList(index.row(), RepoData::MetaLink, 0, myValue);

                if (!DataFlow::getRepoDataList().at(index.row()).repoBaseURL[0].isEmpty()) {

                    myValue[0] = "0";

                    DataFlow::setRepoDataList(index.row(), RepoData::BaseURL, 0, myValue);

                    enabledBaseURL[index.row()] = false;

                }// if

            }// if

            emit dataChanged(index, index);

            return true;


        }// if

        if(index.column() == 7) {

            QVariant myValue[3];

            enabledGPGCheck[index.row()] = !enabledGPGCheck[index.row()];

            if(enabledGPGCheck[index.row()] == true) {

                myValue[0] = "1";

                DataFlow::setRepoDataList(index.row(), RepoData::GPGCheck, 0, myValue);

            }// if
            else {

                myValue[0] = "0";

                DataFlow::setRepoDataList(index.row(), RepoData::GPGCheck, 0, myValue);

            }// else

            emit dataChanged(index, index);

            return true;

        }// if

        if(index.column() == 9) {

            QVariant myValue[3];

            enabledGPGKey[index.row()] = !enabledGPGKey[index.row()];

            if(enabledGPGKey[index.row()] == true) {

                myValue[0] = "1";

                DataFlow::setRepoDataList(index.row(), RepoData::GPGKey, 0, myValue);

            }// if
            else {

                myValue[0] = "0";

                DataFlow::setRepoDataList(index.row(), RepoData::GPGKey, 0, myValue);

            }// else

            emit dataChanged(index, index);

            return true;

        }// if

        if(index.column() == 11) {

            QVariant myValue[3];

            enabledProxy[index.row()] = !enabledProxy[index.row()];

            if(enabledProxy[index.row()] == true) {

                myValue[0] = "1";

                DataFlow::setRepoDataList(index.row(), RepoData::Proxy, 0, myValue);

            }// if
            else {

                myValue[0] = "0";

                DataFlow::setRepoDataList(index.row(), RepoData::Proxy, 0, myValue);

                if(!this->index(index.row(), 14).data(Qt::DisplayRole).toString().isEmpty()) {

                    enabledProxyUsername[index.row()] = false;

                    DataFlow::setRepoDataList(index.row(), RepoData::ProxyUsername, 0, myValue);

                    enabledProxyPassword[index.row()] = false;

                    DataFlow::setRepoDataList(index.row(), RepoData::ProxyPassword, 0, myValue);

                }// if

            }// else

            emit dataChanged(index, index);

            return true;

        }// if

        if(index.column() == 13) {

            QVariant myValue[3];

            enabledProxyUsername[index.row()] = !enabledProxyUsername[index.row()];

            if(enabledProxy[index.row()] == false) {

                enabledProxyUsername[index.row()] = false;

            }// if

            if(enabledProxyUsername[index.row()] == false) {

                enabledProxyPassword[index.row()] = false;

            }// if

            if(enabledProxyUsername[index.row()] == true) {

                myValue[0] = "1";

                DataFlow::setRepoDataList(index.row(), RepoData::ProxyUsername, 0, myValue);

            }// if
            else {

                myValue[0] = "0";

                DataFlow::setRepoDataList(index.row(), RepoData::ProxyUsername, 0, myValue);

            }// else

            emit dataChanged(index, index);

            return true;

        }// if

        if(index.column() == 15) {

            QVariant myValue[3];

            enabledProxyPassword[index.row()] = !enabledProxyPassword[index.row()];

            if(enabledProxy[index.row()] == false) {

                enabledProxyPassword[index.row()] = false;

            }// if

            if((enabledProxyPassword[index.row()] == true) && (enabledProxyUsername[index.row()] == false)) {

                enabledProxyPassword[index.row()] = false;

            }// if

            if(enabledProxyPassword[index.row()] == true) {

                myValue[0] = "1";

                DataFlow::setRepoDataList(index.row(), RepoData::ProxyPassword, 0, myValue);

            }// if
            else {

                myValue[0] = "0";

                DataFlow::setRepoDataList(index.row(), RepoData::ProxyPassword, 0, myValue);

            }// else

            emit dataChanged(index, index);

            return true;

        }// if

        if(index.column() == 17) {

            QVariant myValue[3];

            enabledBandwidth[index.row()] = !enabledBandwidth[index.row()];

            if(enabledBandwidth[index.row()] == true) {

                myValue[0] = "1";

                DataFlow::setRepoDataList(index.row(), RepoData::Speed, 0, myValue);

            }// if
            else {

                myValue[0] = "0";

                DataFlow::setRepoDataList(index.row(), RepoData::Speed, 0, myValue);

            }// else

            emit dataChanged(index, index);

            return true;

        }// if

        if(index.column() == 19) {

            QVariant myValue[3];

            enabledSkipIfUnavailable[index.row()] = !enabledSkipIfUnavailable[index.row()];

            if(enabledSkipIfUnavailable[index.row()] == true) {

                myValue[0] = "1";

                DataFlow::setRepoDataList(index.row(), RepoData::SkipIfUnavailable, 0, myValue);

            }// if
            else {

                myValue[0] = "0";

                DataFlow::setRepoDataList(index.row(), RepoData::SkipIfUnavailable, 0, myValue);

            }// else

            emit dataChanged(index, index);

            return true;

        }// if

        if(index.column() == 21) {

            QVariant myValue[3];

            enabledFailoverMethod[index.row()] = !enabledFailoverMethod[index.row()];

            if(enabledFailoverMethod[index.row()] == true) {

                myValue[0] = "1";

                DataFlow::setRepoDataList(index.row(), RepoData::FailoverMethod, 0, myValue);

            }// if
            else {

                myValue[0] = "0";

                DataFlow::setRepoDataList(index.row(), RepoData::FailoverMethod, 0, myValue);

            }// else

            emit dataChanged(index, index);

            return true;

        }// if

        if(index.column() == 23) {

            QVariant myValue[3];

            enabledMetadataExpire[index.row()] = !enabledMetadataExpire[index.row()];

            if(enabledMetadataExpire[index.row()] == true) {

                myValue[0] = "1";

                DataFlow::setRepoDataList(index.row(), RepoData::MetadataExpire, 0, myValue);

            }// if
            else {

                myValue[0] = "0";

                DataFlow::setRepoDataList(index.row(), RepoData::MetadataExpire, 0, myValue);

            }// else

            emit dataChanged(index, index);

            return true;

        }// if

        if(index.column() == 25) {

            QVariant myValue[3];

            enabledParallelDownload[index.row()] = !enabledParallelDownload[index.row()];

            if(enabledParallelDownload[index.row()] == true) {

                myValue[0] = "1";

                DataFlow::setRepoDataList(index.row(), RepoData::ParallelDownload, 0, myValue);

            }// if
            else {

                myValue[0] = "0";

                DataFlow::setRepoDataList(index.row(), RepoData::ParallelDownload, 0, myValue);

            }// else

            emit dataChanged(index, index);

            return true;

        }// if

        if(index.column() == 27) {

            QVariant myValue[3];

            enabledInclude[index.row()] = !enabledInclude[index.row()];

            if(enabledInclude[index.row()] == true) {

                myValue[0] = "1";

                DataFlow::setRepoDataList(index.row(), RepoData::Include, 0, myValue);

                if(enabledExclude.at(index.row()) == true) {

                    myValue[0] = "0";

                    DataFlow::setRepoDataList(index.row(), RepoData::Exclude, 0, myValue);

                    enabledExclude[index.row()] = false;

                }// if

            }// if
            else {

                myValue[0] = "0";

                DataFlow::setRepoDataList(index.row(), RepoData::Include, 0, myValue);

                if(enabledExclude.at(index.row()) == true) {

                    myValue[0] = "1";

                    DataFlow::setRepoDataList(index.row(), RepoData::Exclude, 0, myValue);

                    enabledExclude[index.row()] = true;

                }// if

            }// else

            emit dataChanged(index, index);

            return true;

        }// if

        if(index.column() == 29) {

            QVariant myValue[3];

            enabledExclude[index.row()] = !enabledExclude[index.row()];

            if(enabledExclude[index.row()] == true) {

                myValue[0] = "1";

                DataFlow::setRepoDataList(index.row(), RepoData::Exclude, 0, myValue);

                if(enabledInclude.at(index.row()) == true) {

                    myValue[0] = "0";

                    DataFlow::setRepoDataList(index.row(), RepoData::Include, 0, myValue);

                    enabledInclude[index.row()] = false;

                }// if

            }// if
            else {

                myValue[0] = "0";

                DataFlow::setRepoDataList(index.row(), RepoData::Exclude, 0, myValue);

                if(enabledInclude.at(index.row()) == true) {

                    myValue[0] = "1";

                    DataFlow::setRepoDataList(index.row(), RepoData::Include, 0, myValue);

                    enabledInclude[index.row()] = true;

                }// if

            }// else

            emit dataChanged(index, index);

            return true;

        }// if

    }// if

    return false;

}// setData

bool RepoDataModel::insertRow(int row, const QModelIndex &parent) {

    Q_UNUSED(parent);

    beginInsertRows(QModelIndex(), row, row);

    DataFlow::setRepoDataList(RepoData());

    QVariant value[2] = { QString("enabled"), QString("1") };

    DataFlow::setRepoDataList(DataFlow::repoDataListSize() - 1, RepoData::Enabled, 0, value);
    DataFlow::setRepoDataList(DataFlow::repoDataListSize() - 1, RepoData::Enabled, 1, value);

    initDataState();

    endInsertRows();

    return true;

}// insertRows

bool RepoDataModel::removeRow(int row, const QModelIndex &parent) {

    Q_UNUSED(parent);

    int matchs = 0;
    bool fileExists = false;

    foreach(RepoData myData, DataFlow::getRepoDataList()) {

        if(myData.repoFileName == DataFlow::getRepoDataList().at(row).repoFileName) {

            matchs++;

        }// if

    }// foreach

    if(!DataFlow::getRepoDataList().at(row).repoName.isEmpty()) {

        QFile myFile(DataFlow::getRepoDataList().at(row).repoFileName);

        if(myFile.exists()) {

            fileExists = true;

            if(!myFile.open(QIODevice::ReadOnly | QIODevice::Text)) {

                YUMRepoManager::showMessage("Error", tr("Access deny when trying to delete: %1").arg(myFile.fileName()));

                myFile.close();

                return false;

            }// if

        }// if

        int option = YUMRepoManager::showMessage("Question", tr("Are you sure to delete the repository: %1?").arg(DataFlow::getRepoDataList().at(row).repoName));

        if(option == QMessageBox::No) {

            myFile.close();

            return false;

        }// if

        if(fileExists && matchs == 1) {

            if(!myFile.remove()) {

                YUMRepoManager::showMessage("Error", tr("Access deny when trying to delete: %1").arg(myFile.fileName()));

                myFile.close();

                return false;

            }// if

        }// if

        myFile.close();

    }// if

    DataFlow::removeDataListAt(row);

    beginRemoveRows(QModelIndex(), row, row);

    enabledState.removeAt(row);
    enabledProxy.removeAt(row);
    enabledProxyUsername.removeAt(row);
    enabledProxyPassword.removeAt(row);
    enabledBaseURL.removeAt(row);
    enabledMetaLink.removeAt(row);
    enabledBandwidth.removeAt(row);
    enabledInclude.removeAt(row);
    enabledExclude.removeAt(row);
    enabledSkipIfUnavailable.removeAt(row);
    enabledFailoverMethod.removeAt(row);
    enabledMetadataExpire.removeAt(row);
    enabledGPGCheck.removeAt(row);
    enabledGPGKey.removeAt(row);

    endRemoveRows();

    if(matchs > 1) {

        emit dataChanged(parent, parent);

    }// else

    YUMRepoManager::syncDatabases(YUMRepoManager::YUMRepoData);

    return true;

}// removeRows

bool RepoDataModel::event(QEvent *event) {

    if(event->type() == QEvent::LanguageChange) {

        this->beginResetModel();

        header.clear();

        header << "" << tr("Name") << tr("Comment") << "" << tr("Base URL") << "" << tr("Metalink") << "" << tr("GPG Check") << "" << tr("GPG Key")
               << "" << tr("Proxy") << "" << tr("Proxy Username") << "" << tr("Proxy Password") << "" << tr("Bandwidth Throttle") << "" << tr("Skip If Unavailable")
               << "" << tr("Failover Method") << "" << tr("Metadata Expire") << "" << tr("Parallel Download") << "" << tr("Include") << "" << tr("Exclude");

        this->endResetModel();

        return true;

    }// if

    QAbstractTableModel::event(event);

    return false;

}// Event

void RepoDataModel::initDataState() {

    enabledState.clear();
    enabledProxy.clear();
    enabledProxyUsername.clear();
    enabledProxyPassword.clear();
    enabledBaseURL.clear();
    enabledMetaLink.clear();
    enabledBandwidth.clear();
    enabledInclude.clear();
    enabledExclude.clear();
    enabledSkipIfUnavailable.clear();
    enabledFailoverMethod.clear();
    enabledMetadataExpire.clear();
    enabledGPGCheck.clear();
    enabledGPGKey.clear();
    enabledParallelDownload.clear();

    int repoDataListSize = DataFlow::repoDataListSize();

    for(int count = 0; count < repoDataListSize; count++) {

        if(DataFlow::getRepoDataList().at(count).repoEnabled[1] == "1") {

            enabledState.append(true);

        }// if
        else {

            enabledState.append(false);

        }// else

        if(DataFlow::getRepoDataList().at(count).repoProxy[0] == "1") {

            enabledProxy.append(true);

        }// if
        else {

            enabledProxy.append(false);

        }// else

        if(DataFlow::getRepoDataList().at(count).repoProxyUsername[0] == "1") {

            enabledProxyUsername.append(true);

        }// if
        else {

            enabledProxyUsername.append(false);

        }// else

        if(DataFlow::getRepoDataList().at(count).repoProxyPassword[0] == "1") {

            enabledProxyPassword.append(true);

        }// if
        else {

            enabledProxyPassword.append(false);

        }// else

        if(DataFlow::getRepoDataList().at(count).repoBaseURL[0] == "1") {

            enabledBaseURL.append(true);

        }// if
        else {

            enabledBaseURL.append(false);

        }// else

        if(DataFlow::getRepoDataList().at(count).repoMetaLink[0] == "1") {

            enabledMetaLink.append(true);

        }// if
        else {

            enabledMetaLink.append(false);

        }// else

        if(DataFlow::getRepoDataList().at(count).repoBandwidth[0] == "1") {

            enabledBandwidth.append(true);

        }// if
        else {

            enabledBandwidth.append(false);

        }// else

        if(DataFlow::getRepoDataList().at(count).repoInclude[0] == "1") {

            enabledInclude.append(true);

        }// if
        else {

            enabledInclude.append(false);

        }// else

        if(DataFlow::getRepoDataList().at(count).repoExclude[0] == "1") {

            enabledExclude.append(true);

        }// if
        else {

            enabledExclude.append(false);

        }// else

        if(DataFlow::getRepoDataList().at(count).repoSkipIfUnavailable[0] == "1") {

            enabledSkipIfUnavailable.append(true);

        }// if
        else {

            enabledSkipIfUnavailable.append(false);

        }// else

        if(DataFlow::getRepoDataList().at(count).repoFailoverMethod[0] == "1") {

            enabledFailoverMethod.append(true);

        }// if
        else {

            enabledFailoverMethod.append(false);

        }// else

        if(DataFlow::getRepoDataList().at(count).repoMetadataExpire[0] == "1") {

            enabledMetadataExpire.append(true);

        }// if
        else {

            enabledMetadataExpire.append(false);

        }// else

        if(DataFlow::getRepoDataList().at(count).repoGPGCheck[0] == "1") {

            enabledGPGCheck.append(true);

        }// if
        else {

            enabledGPGCheck.append(false);

        }// else

        if(DataFlow::getRepoDataList().at(count).repoGPGKey[0] == "1") {

            enabledGPGKey.append(true);

        }// if
        else {

            enabledGPGKey.append(false);

        }// else

        if(DataFlow::getRepoDataList().at(count).repoParallelDownload[0] == "1") {

            enabledParallelDownload.append(true);

        }// if
        else {

            enabledParallelDownload.append(false);

        }// else

    }// for

}// initDataState

const QColor& RepoDataModel::colorEnabled() {

    return YUMRepoManager::colorEnabled();

}// colorEnabled

const QColor& RepoDataModel::colorDisabled() {

    return YUMRepoManager::colorDisabled();

}// colorDisabled

void RepoDataModel::setColorEnabled(const QColor &color) {

    cellColor[Enabled] = color;

    updateModel();

}// setColorEnabled

void RepoDataModel::setColorDisabled(const QColor &color) {

    cellColor[Disabled] = color;

    updateModel();

}// setColorDisabled

void RepoDataModel::updateModel() {

    this->beginResetModel();

    initDataState();

    this->endResetModel();

}// updateModel
