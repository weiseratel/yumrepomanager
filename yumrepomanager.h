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

#ifndef YUMREPOMANAGER_H
#define YUMREPOMANAGER_H

#include "repodata.h"
#include "dataflow.h"
#include "helperthread.h"
#include "repodataproxymodel.h"
#include "repodatamodel.h"
#include "repodatadelegate.h"
#include "addrepository.h"

#include <QMainWindow>
#include <QMetaType>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QParallelAnimationGroup>
#include <QTranslator>
#include <QPropertyAnimation>
#include <QHash>
#include <QSettings>
#include <QColor>
#include <QDebug>

namespace Ui {

    class YUMRepoManager;

}

class YUMRepoManager : public QMainWindow
{

    Q_OBJECT

public:

    explicit YUMRepoManager(QWidget *parent = 0);

    ~YUMRepoManager();

    enum DataBases { YUMRepoData, YUMConfData, AllDatabases };

    enum ToolBarActions { ActionRemove, ActionSave, ActionExport};

    static bool dataChanged;

    static RepoDataModel *repoDataModel;
    static RepoDataProxyModel *proxyModel;

    static int filterColumn;
    static QString filterRow;

    static int showMessage(QString messageType, QString message);
    static int showMessage(QString messageType, QString message, QString informativeMessage);

    static QTranslator *currentTranslator;

    static QString systemLocale;
    static QString preferredLanguage;

    static QString createRepoArgs;

    static const QString translate(const QString context, const QString sourceText);

    static bool automaticallyDeleteOldPackagesVersions;

    inline static const QColor& colorEnabled() {

        return cellColor[Enabled];

    }// colorEnabled

    inline static const QColor& colorDisabled() {

        return cellColor[Disabled];

    }// colorDisabled

    inline static bool anyChangeProduced(DataBases dataBases) {

        if(dataBases == YUMRepoData) {

            if(repoDataListBackUp != DataFlow::getRepoDataList()) {

                return true;

            }// if

        }// if
        else if(dataBases == YUMConfData) {

            if(yumConfDataBackUp != DataFlow::getYumConfData()) {

                return true;

            }// if

        }// else if

        return false;

    }// anyChangeProduced

    inline static void syncDatabases(DataBases dataBases) {

        if(dataBases == YUMRepoData) {

            repoDataListBackUp = DataFlow::getRepoDataList();

        }// if
        else if(dataBases == YUMConfData) {

            yumConfDataBackUp = DataFlow::getYumConfData();

        }// else if

    }// syncDatabases

signals:

    void safeAll(YUMRepoManager::DataBases databases);

    void colorChanged();

    void colorDisabledChanged(const QColor &color);

public slots:

    void saveData(YUMRepoManager::DataBases database);

    inline void setColorEnabled(const QColor &color) {

        cellColor[Enabled] = color;

        emit colorChanged();

    }// setColorEnabled

    inline void setColorDisabled(const QColor &color) {

        cellColor[Disabled] = color;

        emit colorChanged();

    }// setColorDisabled

    void setStatusBarMessage(QString message);

private slots:

    void switchTranslator(const QString &locale);

    void toolBarActionsState(YUMRepoManager::ToolBarActions action, bool state);

    void animationFinished();

    void dataChangeOcurred();

    void repoDataModelDataChangeOcurred(const QModelIndex &topLeft, const QModelIndex &bottomRight);

    void on_actionQuit_triggered();

    void on_actionGlobal_Options_triggered();

    void on_actionRemove_triggered();

    void on_actionAdd_triggered();

    void on_actionAbout_Qt_triggered();

    void on_actionAbout_YUMRepoManager_triggered();

    void on_actionCreate_triggered();

    void on_actionSave_triggered();

    void on_actionPreferences_triggered();

    void on_actionPlugins_triggered();

    void on_actionFilter_triggered();

    void on_comboBoxColumnFilter_activated(int index);

    void on_actionImport_repository_triggered();

    void on_actionCreaterepoArguments_triggered();

    void on_actionUpdateRepositoryList_triggered();

    void on_actionExport_Repository_triggered();

    void on_comboBoxRowFilter_activated(int index);

private:

    Ui::YUMRepoManager *ui;

    inline void anyChangeProduced() {

        if( (repoDataListBackUp != DataFlow::getRepoDataList()) || (yumConfDataBackUp != DataFlow::getYumConfData()) ) {

            if(showMessage("Question", tr("There is unsaved changes."), tr("Do you want to save it before to quit?")) == QMessageBox::Yes) {

                saveData(YUMRepoData);
                saveData(YUMConfData);

            }// if

        }// if

    }// anyChangeProduced

    bool event(QEvent *event);

    void changeEvent(QEvent *event);

    enum RowState { ShowAll, ShowEnables, ShowDisables };

    void setFilterRowState(QString state);

    void loadTranslations();

    bool saveYumConfData();
    bool saveRepoData();

    void loadSettings();
    void saveSettings();

    DataFlow *dataFlow;
    AddRepository *addRepository;

    QStringList columnFilterLabels;

    QParallelAnimationGroup *groupAnimations;
    QPropertyAnimation *animationSlideBar;
    QPropertyAnimation *animationTableView;

    RepoDataDelegate *repoDataDelegate;

    HelperThread *helperThread;

    QHash<QString, QTranslator *> translators;
    QString translationsPath;

    static QList<RepoData> repoDataListBackUp;
    static QMap<QString, QString> yumConfDataBackUp;

    QStringListModel *columnFilterList;

    enum { Enabled, Disabled };

    bool arrowState;

    static QColor cellColor[2];

};

//Q_DECLARE_METATYPE(YUMRepoManager::DataBases)

#endif // YUMREPOMANAGER_H
