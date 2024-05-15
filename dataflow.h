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

#ifndef DATAFLOW_H
#define DATAFLOW_H

#include "repodata.h"
#include "plugindata.h"
#include "yumpluginmodel.h"

#include <QMap>
#include <QStringListModel>

class DataFlow : public QObject
{

    Q_OBJECT

public:

    explicit DataFlow(QObject *parent = 0);

    enum FileFormat {

        FileName,
        AbsoluteFileName

    };

    inline static QString getRepoPath() {

        return repoPath;

    }// getRepoPath

    inline static QString getYumConf() {

        return yumConf;

    }// getYumConf

    inline static QString getYumProtectedPath() {

        return protectedPath;

    }// getYumProtectedPath

    inline static const QList<RepoData> &getRepoDataList() {

        return repoDataList;

    }// getRepoDataList

    inline static const QMap<QString, QString> &getYumConfData() {

        return yumConfData;

    }// getYumConfData

    inline static const QList<PluginData> &getPluginDataList() {

        return pluginDataList;

    }// getPluginDataList

    inline static YUMPluginModel *getPluginModel() {

        return pluginModel;

    }// getPluginDataList

    static void setRepoDataList(int repoLabelIndex, RepoData::RepoLabel repoLabel, int valueIndex, QVariant value[3]);

    inline static void setRepoDataList(const RepoData &node) {

        repoDataList.append(node);

    }// setRepoDataList

    inline static void removeDataListAt(int index) {

        repoDataList.removeAt(index);

    }// removeDataListAt

    inline static void setPluginState(int index, bool state) {

        if(state) {

            pluginDataList[index].pluginEnabled[1] = "1";

        }// if
        else {

            pluginDataList[index].pluginEnabled[1] = "0";

        }// else

    }// setPluginState

    inline static void updateYumConfData(QString removeKey) {

        yumConfData.remove(removeKey);

    }// updateYumConfData

    inline static void updateYumConfData(QString Key, QString Value) {

        yumConfData.insert(Key, Value);

    }// updateYumConfData

    void fillRepoDataList();
    void fillRepoDataList(const QStringList &repoFiles);
    void fillYumConfData();
    void fillPluginDataList();
    void fillProtectedList();

    inline static int pluginDataListSize() {

       return pluginDataList.size();

    }// pluginDataListSize

    inline static int repoDataListSize() {

        return repoDataList.size();

    }// repoDataListSize

    static QStringListModel *protectedListModel;

    static QStringList locateFiles(const QString path, const QString fileExtension, FileFormat fileFormat);

    static void insertNewNode(const RepoData &node);

    static void setDefaultValues(RepoData &node);

private:

    static QString repoPath;
    static QString yumConf;
    static QString protectedPath;

    static QList<RepoData> repoDataList;
    static QMap<QString, QString> yumConfData;
    static QList<PluginData> pluginDataList;
    static YUMPluginModel *pluginModel;

    QStringList repoKeyList;

    QString pluginPath;

};

#endif // DATAFLOW_H
