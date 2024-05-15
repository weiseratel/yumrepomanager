/*
    YUMRepoManager, a manager for the configuration files of YUM and his repositories.
    Copyright (C) 2011-2014  Josué V. Herrera

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

#include "yumrepomanager.h"
#include "ui_yumrepomanager.h"
#include "globaloptions.h"
#include "createrepowizard.h"
#include "createrepoarguments.h"
#include "preferences.h"
#include "yumplugin.h"
#include "editrepo.h"
#include "about.h"

#include <QSettings>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QCheckBox>
#include <QFont>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QDropEvent>
#include <QUrl>
#include <QRgb>
#include <QTimer>
#include <QSize>

RepoDataModel *YUMRepoManager::repoDataModel;
RepoDataProxyModel *YUMRepoManager::proxyModel;

QTranslator *YUMRepoManager::currentTranslator;

QString YUMRepoManager::systemLocale = "";
QString YUMRepoManager::preferredLanguage = "";

QString YUMRepoManager::createRepoArgs = "";

int YUMRepoManager::filterColumn = 0;
QString YUMRepoManager::filterRow = "ShowAll";

bool YUMRepoManager::dataChanged = false;

QColor YUMRepoManager::cellColor[2];

QList<RepoData> YUMRepoManager::repoDataListBackUp;
QMap<QString, QString> YUMRepoManager::yumConfDataBackUp;

bool YUMRepoManager::automaticallyDeleteOldPackagesVersions;

YUMRepoManager::YUMRepoManager(QWidget *parent) : QMainWindow(parent), ui(new Ui::YUMRepoManager)
{

    ui->setupUi(this);

    setStatusBarMessage(tr("Loading"));

    //
    //  Loading yum data.
    //
    dataFlow = new DataFlow(this);

    dataFlow->fillRepoDataList();
    dataFlow->fillYumConfData();
    dataFlow->fillPluginDataList();

    //
    //  Setting the default table colors.
    //
    cellColor[Enabled].setRgb(111, 166, 99, 255);
    cellColor[Disabled].setRgb(226, 150, 112, 255);

    //
    //  Setting system locale
    //
    systemLocale = QLocale::system().name().split("_").at(0);

    //
    //  Setting language configuration.
    //
    translationsPath = ":/Language/Translations/";
    currentTranslator = new QTranslator(this);

    //
    //  Some table configurations.
    //
    //ui->tableView->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    ui->tableView->setDragDropMode(QAbstractItemView::DragDrop);
    ui->tableView->setDragEnabled(true);

    //
    //  Setting the model and the proxy model (sorting and filtering)
    //
    repoDataModel = new RepoDataModel(this);

    proxyModel = new RepoDataProxyModel(this);
    proxyModel->setSourceModel(repoDataModel);
    proxyModel->setDynamicSortFilter(true);

    ui->tableView->setModel(proxyModel);

    //
    //  Setting some sorting configurations.
    //
    ui->tableView->horizontalHeader()->setSortIndicator(1, Qt::AscendingOrder);
    ui->tableView->sortByColumn(1);
    ui->tableView->setSortingEnabled(true);

    //
    //  Setting the delegate.
    //
    repoDataDelegate = new RepoDataDelegate(this);

    ui->tableView->setItemDelegate(repoDataDelegate);

    //
    //  Setting column filter labels.
    //
    columnFilterLabels << tr("None") << tr("Name") << tr("Base URL") << tr("Metalink") << tr("GPG Check") << tr("GPG Key")
                       << tr("Proxy") << tr("Bandwidth Throttle") << tr("Skip If Unavailable") << tr("Failover Method")
                       << tr("Metadata Expire") << tr("Parallel Download") << tr("Include") << tr("Exclude");

    columnFilterList = new QStringListModel(this);
    columnFilterList->setStringList(columnFilterLabels);

    ui->comboBoxColumnFilter->setModel(columnFilterList);

    //
    //  Setting the arrow state (direction) of the filter button.
    //
    arrowState = true;

    //
    //  Setting the filter animation.
    //
    groupAnimations = new QParallelAnimationGroup(this);
    animationSlideBar = new QPropertyAnimation(ui->slideBar, "maximumSize", this);
    animationSlideBar->setDuration(300);
    animationTableView = new QPropertyAnimation(ui->tableView, "geometry", this);
    animationTableView->setDuration(300);
    groupAnimations->addAnimation(animationSlideBar);
    groupAnimations->addAnimation(animationTableView);

    //
    // Set default createrepo arguments
    //
    createRepoArgs = " --database --checksum sha256 --pretty --unique-md-filenames --update --workers 2 --cachedir /tmp/ --groupfile ";

    //
    //  Loading saved configurations.
    //
    loadSettings();

    //
    //  Loading interface language.
    //
    loadTranslations();

    //
    //  Applying column filter
    //
    on_comboBoxColumnFilter_activated(ui->comboBoxColumnFilter->currentIndex());

    //
    //  Setting a copy of the repository data as well as the yum options,
    //  this will be used at program quit to know if has been any data change.
    //
    repoDataListBackUp = DataFlow::getRepoDataList();
    yumConfDataBackUp = DataFlow::getYumConfData();

    //
    //  Set Accept Drops
    //
    setAcceptDrops(true);

    //
    //  Connections
    //
    connect(ui->tableView, SIGNAL(tableChanged()), repoDataModel, SLOT(updateModel()));
    connect(ui->tableView, SIGNAL(tableChanged()), this, SLOT(dataChangeOcurred()));
    connect(ui->tableView, SIGNAL(deleteEventOcurred()), this, SLOT(on_actionRemove_triggered()));
    connect(ui->tableView, SIGNAL(selectionChanged(YUMRepoManager::ToolBarActions,bool)), this, SLOT(toolBarActionsState(YUMRepoManager::ToolBarActions,bool)));

    connect(groupAnimations, SIGNAL(finished()), this, SLOT(animationFinished()));

    connect(this, SIGNAL(colorChanged()), repoDataModel, SLOT(updateModel()));

    connect(repoDataModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(repoDataModelDataChangeOcurred(QModelIndex,QModelIndex)));

    connect(this, SIGNAL(safeAll(YUMRepoManager::DataBases)), this, SLOT(saveData(YUMRepoManager::DataBases)));

    setStatusBarMessage(tr("Ready"));

}// Constructor

YUMRepoManager::~YUMRepoManager()
{

    delete ui;

}// Destructor

int YUMRepoManager::showMessage(QString messageType, QString message) {

    QMessageBox myMessage;

    int buttonPressed;

    if(messageType == "Information") {

        myMessage.setWindowTitle(tr("Information"));
        myMessage.setIcon(QMessageBox::Information);
        myMessage.setText(message);
        myMessage.setStandardButtons(QMessageBox::Ok);

    }// if
    else if(messageType == "Question") {

        myMessage.setWindowTitle(tr("Question"));
        myMessage.setIcon(QMessageBox::Question);
        myMessage.setText(message);
        myMessage.setStandardButtons(QMessageBox::Yes | QMessageBox::No);

    }// else if
    else if(messageType == "Warning") {

        myMessage.setWindowTitle(tr("Warning"));
        myMessage.setIcon(QMessageBox::Warning);
        myMessage.setText(message);
        myMessage.setStandardButtons(QMessageBox::Ok | QMessageBox::No);

    }// else if
    else if(messageType == "Error") {

        myMessage.setWindowTitle(tr("Error"));
        myMessage.setIcon(QMessageBox::Critical);
        myMessage.setText(message);
        myMessage.setStandardButtons(QMessageBox::Ok);

    }// else if

    buttonPressed = myMessage.exec();

    return buttonPressed;

}// showMessage

int YUMRepoManager::showMessage(QString messageType, QString message, QString informativeMessage) {

    QMessageBox myMessage;

    int buttonPressed;

    if(messageType == "Information") {

        myMessage.setWindowTitle(tr("Information"));
        myMessage.setIcon(QMessageBox::Information);
        myMessage.setText(message);
        myMessage.setInformativeText(informativeMessage);
        myMessage.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);

    }// if
    else if(messageType == "Question") {

        myMessage.setWindowTitle(tr("Question"));
        myMessage.setIcon(QMessageBox::Question);
        myMessage.setText(message);
        myMessage.setInformativeText(informativeMessage);
        myMessage.setStandardButtons(QMessageBox::Yes | QMessageBox::No);

    }// else if
    else if(messageType == "Warning") {

        myMessage.setWindowTitle(tr("Warning"));
        myMessage.setIcon(QMessageBox::Warning);
        myMessage.setText(message);
        myMessage.setInformativeText(informativeMessage);
        myMessage.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);

    }// else if
    else if(messageType == "Error") {

        myMessage.setWindowTitle(tr("Error"));
        myMessage.setIcon(QMessageBox::Critical);
        myMessage.setText(message);
        myMessage.setInformativeText(informativeMessage);
        myMessage.setStandardButtons(QMessageBox::Ok);

    }// else if

    buttonPressed = myMessage.exec();

    return buttonPressed;

}// showMessage

void YUMRepoManager::on_actionImport_repository_triggered()
{

    QStringList repositories;

    repositories = QFileDialog::getOpenFileNames(this, tr("Select The Repository Files"), "/home/", tr("YUM Repository Files (*.repo)"));

    if(!repositories.isEmpty()) {

        DataFlow dataFlow;

        dataFlow.fillRepoDataList(repositories);

        repoDataModel->updateModel();

    }// if

}// Import Repository

void YUMRepoManager::on_actionExport_Repository_triggered()
{

    QModelIndexList indexList = ui->tableView->selectionModel()->selectedRows();

    if (indexList.size() == 0) {

        showMessage("Information", tr("You have to select a repository definition first."));

        return;

    }// if

    QString exportPath = QFileDialog::getExistingDirectory(this, "Select Folder", QDir::homePath(), QFileDialog::ShowDirsOnly);

    if(exportPath.isEmpty()) {

        return;

    }// if

    for(int count(0); count < indexList.size(); ++count) {

        QModelIndex index = proxyModel->mapToSource(indexList.at(count));

        RepoData node = DataFlow::getRepoDataList().at(index.row());

        QFile myFile(node.repoFileName);

        if(!myFile.open(QIODevice::ReadOnly | QIODevice::Text)) {

            myFile.close();

            return;

        }// if

        if(!exportPath.endsWith("/")) {

            exportPath.append("/");

        }// if

        QStringList absoluteFileNameSplit = node.repoFileName.split('/');

        QString fileName = absoluteFileNameSplit.at(absoluteFileNameSplit.size() - 1);

        myFile.copy(node.repoFileName, exportPath + fileName);

        myFile.close();

    }// for

}// Export Repository

void YUMRepoManager::on_actionAdd_triggered()
{

    addRepository = new AddRepository(this);
    addRepository->setModal(true);
    addRepository->exec();

    repoDataModel->updateModel();

    //proxyModel->invalidate();

}// Add Triggered

void YUMRepoManager::on_actionRemove_triggered()
{

    QModelIndexList indexList = ui->tableView->selectionModel()->selectedRows();

    if(indexList.size() == 0) {

        showMessage("Information", tr("You have to select a repository definition first."));

        return;

    }// if
    else if(indexList.size() == repoDataModel->rowCount(QModelIndex())) {

        for(int count(0); count < indexList.size(); ++count) {

            repoDataModel->removeRow(0, QModelIndex());

        }// for

        return;

    }// else if

    while(indexList.size() != 0) {

        repoDataModel->removeRow(proxyModel->mapToSource(indexList.at(0)).row(), QModelIndex());

        indexList.clear();

        indexList = ui->tableView->selectionModel()->selectedRows();

    }// while

    toolBarActionsState(ActionRemove, false);

}// Remove Triggered

void YUMRepoManager::on_actionSave_triggered()
{

    saveData(AllDatabases);

}// Save Triggered

bool YUMRepoManager::saveYumConfData() {

    /*
    ##
    ## Global Configuration
    ##
    */

    QFile myFile(DataFlow::getYumConf());

    if(!myFile.open(QIODevice::Truncate | QIODevice::WriteOnly | QIODevice::Text)) {

        YUMRepoManager::showMessage("Error", tr("Access deny when trying to open: %1").arg(DataFlow::getYumConf()));

        return false;

    }// if

    QTextStream outFlow(&myFile);

    QMapIterator<QString, QString> yumConfDataIterator(DataFlow::getYumConfData());

    outFlow << QString("[main]") + "\n";

    while(yumConfDataIterator.hasNext()) {

        yumConfDataIterator.next();

        outFlow << yumConfDataIterator.key() + "=" + yumConfDataIterator.value() + "\n";

    }// while

    outFlow << "\n";

    outFlow.flush();

    myFile.flush();
    myFile.close();

    return true;

}// saveYumConfData

bool YUMRepoManager::saveRepoData() {

    /*
    ##
    ## Save Repositories Data
    ##
    */

    int Flag = 0;

    QString lastFileName = "";

    for(int index = 0; index < DataFlow::repoDataListSize(); ++index) {

        RepoData node = DataFlow::getRepoDataList().at(index);

        //
        //  If this node doesn't have a file name, we create one using the node name.
        //

        if(node.repoFileName.isEmpty()) {

            node.repoFileName = DataFlow::getRepoPath() + node.repoName + ".repo";

        }// if

        QFile myFile(node.repoFileName);

        if(node.repoFileName == DataFlow::getYumConf()) {

            Flag = 1;

        }// if
        else {

            if(lastFileName != node.repoFileName) {

                Flag = 0;

                lastFileName = node.repoFileName;

            }// if
            else
                Flag = 1;

        }// else

        if(Flag == 0) {

            if(!myFile.open(QIODevice::Truncate | QIODevice::WriteOnly | QIODevice::Text)) {

                YUMRepoManager::showMessage("Error", tr("Access deny when trying to open: %1").arg(node.repoFileName));

                return false;

            }// if

        }//if
        else if(Flag == 1) {

            if(!myFile.open(QIODevice::Append | QIODevice::WriteOnly | QIODevice::Text)) {

                YUMRepoManager::showMessage("Error", tr("Access deny when trying to open: %1").arg(node.repoFileName));

                return false;

            }// if

        }// else if

        QTextStream outFlow(&myFile);

        outFlow.setCodec("UTF-8");

        outFlow << "[" + node.repoName + "]\n";

        //
        //// Comment
        //

        outFlow << node.repoComment[0] + "=" + node.repoComment[1] + "\n";

        //
        //// Failover Method
        //

        if(!node.repoFailoverMethod[0].isEmpty()) {

            if(node.repoFailoverMethod[0] == "1") {

                outFlow << node.repoFailoverMethod[1] + "=" + node.repoFailoverMethod[2] + "\n";

            }// if
            else if(node.repoFailoverMethod[0] == "0") {

                outFlow << "#" + node.repoFailoverMethod[1] + "=" + node.repoFailoverMethod[2] + "\n";

            }// else if

        }// if

        //
        //// Skip If Unavailable
        //

        if(!node.repoSkipIfUnavailable[0].isEmpty()) {

            if(node.repoSkipIfUnavailable[0] == "1") {

                outFlow << node.repoSkipIfUnavailable[1] + "=" + node.repoSkipIfUnavailable[2] + "\n";

            }// if
            else if(node.repoSkipIfUnavailable[0] == "0") {

                outFlow << "#" + node.repoSkipIfUnavailable[1] + "=" + node.repoSkipIfUnavailable[2] + "\n";

            }// else if

        }// if

        //
        //// Repo BaseURL & MetaLink
        //

        if(node.repoBaseURL[2].isEmpty() && node.repoMetaLink[2].isEmpty()) {

            showMessage("Error", tr("You have to specify the address of your repository, whether local or remote. According to your needs, fill only one of the following fields: \"Metalink\" or \"Base URL\"."));

            myFile.remove();

            return false;

        }// if

        if(!node.repoBaseURL[2].isEmpty()) {

            if(node.repoBaseURL[0] == "1") {

                outFlow << node.repoBaseURL[1] + "=" + node.repoBaseURL[2] + "\n";

            }// if
            else if(node.repoBaseURL[0] == "0") {

                outFlow << "#" + node.repoBaseURL[1] + "=" + node.repoBaseURL[2] + "\n";

            }// else

        }// if

        if(!node.repoMetaLink[2].isEmpty()) {

            if(node.repoMetaLink[0] == "1") {

                outFlow << node.repoMetaLink[1] + "=" + node.repoMetaLink[2] + "\n";

            }// if
            else if(node.repoMetaLink[0] == "0") {

                outFlow << "#" + node.repoMetaLink[1] + "=" + node.repoMetaLink[2] + "\n";

            }// else

        }// if

        //
        //// Proxy
        //

        if(node.repoProxy[0] == "1" && !node.repoProxy[2].isEmpty()) {

            outFlow << node.repoProxy[1] + "=" + node.repoProxy[2] + "\n";

        }// if
        else if(node.repoProxy[0] == "0" && !node.repoProxy[2].isEmpty()) {

            outFlow << "#" + node.repoProxy[1] + "=" + node.repoProxy[2] + "\n";

        }// else if

        if(node.repoProxyUsername[0] == "1" && !node.repoProxyUsername[2].isEmpty()) {

            outFlow << node.repoProxyUsername[1] + "=" + node.repoProxyUsername[2] + "\n";

        }// if
        else if(node.repoProxyUsername[0] == "0" && !node.repoProxyUsername[2].isEmpty()) {

            outFlow << "#" + node.repoProxyUsername[1] + "=" + node.repoProxyUsername[2] + "\n";

        }// else if

        if(node.repoProxyPassword[0] == "1" && !node.repoProxyPassword[2].isEmpty()) {

            outFlow << node.repoProxyPassword[1] + "=" + node.repoProxyPassword[2] + "\n";

        }// if
        else if(node.repoProxyPassword[0] == "0" && !node.repoProxyPassword[2].isEmpty()) {

            outFlow << "#" + node.repoProxyPassword[1] + "=" + node.repoProxyPassword[2] + "\n";

        }// else if

        //
        //// Bandwidth
        //

        if(!node.repoBandwidth[0].isEmpty()) {

            if(node.repoBandwidth[0] == "1") {

                outFlow << node.repoBandwidth[1] + "=" + node.repoBandwidth[2] + "\n";

            }// if
            else if(node.repoBandwidth[0] == "0") {

                outFlow << "#" + node.repoBandwidth[1] + "=" + node.repoBandwidth[2] + "\n";

            }// else if

        }// if

        //
        //// Include Packages
        //

        if(!node.repoInclude[0].isEmpty()) {

            if(node.repoInclude[0] == "1") {

                outFlow << node.repoInclude[1] + "=" + node.repoInclude[2] + "\n";

            }// if
            else if(node.repoInclude[0] == "0") {

                outFlow << "#" + node.repoInclude[1] + "=" + node.repoInclude[2] + "\n";

            }// else if

        }// if

        //
        //// Exclude Packages
        //

        if(!node.repoExclude[0].isEmpty()) {

            if(node.repoExclude[0] == "1") {

                outFlow << node.repoExclude[1] + "=" + node.repoExclude[2] + "\n";

            }// if
            else if(node.repoExclude[0] == "0") {

                outFlow << "#" + node.repoExclude[1] + "=" + node.repoExclude[2] + "\n";

            }// else if

        }// if

        //
        //// Enabled
        //

        if(!node.repoEnabled[0].isEmpty()) {

            outFlow << node.repoEnabled[0] + "=" + node.repoEnabled[1] + "\n";

        }// if
        else {

            qDebug() << "Yehaaaa!!!";

            outFlow << "enabled" << "=" << "1" << "\n";

        }// else

        //
        //// Metadata Expire
        //

        if(!node.repoMetadataExpire[0].isEmpty()) {

            if(node.repoMetadataExpire[0] == "1") {

                outFlow << node.repoMetadataExpire[1] + "=" + node.repoMetadataExpire[2] + "\n";

            }// if
            else if(node.repoMetadataExpire[0] == "0") {

                outFlow << "#" + node.repoMetadataExpire[1] + "=" + node.repoMetadataExpire[2] + "\n";

            }// else if

        }// if

        //
        //// GPG Check
        //

        if(!node.repoGPGCheck[0].isEmpty()) {

            if(node.repoGPGCheck[0] == "1") {

                outFlow << node.repoGPGCheck[1] + "=" + node.repoGPGCheck[2] + "\n";

            }// if
            else if(node.repoGPGCheck[0] == "0") {

                outFlow << "#"<< node.repoGPGCheck[1] + "=" + node.repoGPGCheck[2] + "\n";

            }// else if

        }// if

        //
        //// GPG Key
        //

        if(!node.repoGPGKey[0].isEmpty()) {

            if(node.repoGPGKey[0] == "1") {

                outFlow << node.repoGPGKey[1] + "=" + node.repoGPGKey[2] + "\n";

            }// if
            else if(node.repoGPGKey[0] == "0") {

                outFlow << "#" << node.repoGPGKey[1] + "=" + node.repoGPGKey[2] + "\n";

            }// else if

        }// if

        //
        //// Parallel Download
        //

        if(!node.repoParallelDownload[0].isEmpty()) {

            if(node.repoParallelDownload[0] == "1") {

                outFlow << node.repoParallelDownload[1] + "=" + node.repoParallelDownload[2] + "\n";

            }// if
            else if(node.repoParallelDownload[0] == "0") {

                outFlow << "#"<< node.repoParallelDownload[1] + "=" + node.repoParallelDownload[2] + "\n";

            }// else if

        }// if

        //
        //// The end.
        //

        outFlow << "\n";

        outFlow.flush();

        myFile.flush();
        myFile.close();

    }// for

    return true;

}// saveRepoData

void YUMRepoManager::saveData(YUMRepoManager::DataBases database) {

    if(database == YUMRepoData) {

        if(saveRepoData()) {

            syncDatabases(YUMRepoData);

            toolBarActionsState(ActionSave, false);

            setStatusBarMessage(tr("Saved"));

        }// if
        else {

            setStatusBarMessage(tr("Not saved"));

        }// else

    }// if
    else if(database == YUMConfData) {

        if(saveYumConfData()) {

            syncDatabases(YUMConfData);

            toolBarActionsState(ActionSave, false);

            setStatusBarMessage(tr("Saved"));

        }// if
        else {

            setStatusBarMessage(tr("Not saved"));

        }// else

    }// else if
    else if(database == AllDatabases) {

        if(saveRepoData() && saveYumConfData()) {

            syncDatabases(YUMRepoData);
            syncDatabases(YUMConfData);

            toolBarActionsState(ActionSave, false);

            setStatusBarMessage(tr("Saved"));

        }// if
        else {

            setStatusBarMessage(tr("Not saved"));

        }// else

    }// else if

}// saveData

void YUMRepoManager::on_actionPlugins_triggered()
{

    YUMPlugin *yumPlugin = new YUMPlugin(this);

    yumPlugin->setModal(true);
    yumPlugin->exec();

}// Plugings Triggered

void YUMRepoManager::on_actionGlobal_Options_triggered()
{

    GlobalOptions *myGlobalOptions = new GlobalOptions(this);

    connect(myGlobalOptions, SIGNAL(saveData(YUMRepoManager::DataBases)), this, SLOT(saveData(YUMRepoManager::DataBases)));

    myGlobalOptions->setModal(true);

    int state = myGlobalOptions->exec();

    if(state == QDialog::Accepted) {

        if(yumConfDataBackUp != DataFlow::getYumConfData()) {

            toolBarActionsState(ActionSave, true);

        }// if

    }// if

}// Global Options Triggered

void YUMRepoManager::on_actionCreate_triggered()
{

    CreateRepoWizard *createrepoWizard = new CreateRepoWizard(this);

    createrepoWizard->setModal(true);
    createrepoWizard->exec();

}// Create Repository

void YUMRepoManager::on_actionCreaterepoArguments_triggered()
{

    CreaterepoArguments *createrepoArguments = new CreaterepoArguments(this);

    createrepoArguments->setModal(true);
    createrepoArguments->exec();

}// Createrepo Arguments

void YUMRepoManager::loadTranslations() {

    QDir translationsDirectory(translationsPath);

    translationsDirectory.setFilter(QDir::Files | QDir::Readable | QDir::NoDotAndDotDot);
    translationsDirectory.setNameFilters(QStringList("*_*.qm"));

    QFileInfoList translations = translationsDirectory.entryInfoList();

    for(int index = 0; index < translations.size(); ++index) {

        QFileInfo translationFile = translations.at(index);

        QTranslator *translator = new QTranslator(this);

        if(translator->load(translationFile.absoluteFilePath())) {

            QString locale = translationFile.fileName().split("_").at(1).mid(0, 2);

            translators.insert(locale, translator);

        }// if
        else {

            qDebug() << "ERROR: The language loading has failed.";

        }// else

    }// for

    if((!preferredLanguage.isEmpty()) && (preferredLanguage != "SystemLocale")) {

        switchTranslator(preferredLanguage);

    }// if
    else {

        switchTranslator(systemLocale);

    }// else

}// Load Language

void YUMRepoManager::switchTranslator(const QString &locale) {

    if(systemLocale != locale) {

        preferredLanguage = locale;

    }// if
    else {

        preferredLanguage = "SystemLocale";

    }// else

    if(currentTranslator) {

        qApp->removeTranslator(currentTranslator);

        currentTranslator = new QTranslator(this);

    }// if

    if(locale != "en") {

        currentTranslator = translators.value(locale, 0);

        if(currentTranslator) {

            qApp->installTranslator(currentTranslator);

        }// if

    }// if

}// Switch Translator

const QString YUMRepoManager::translate(const QString context, const QString sourceText) {

    if(YUMRepoManager::currentTranslator->translate(context.toUtf8().constData(), sourceText.toUtf8().constData()).isEmpty()) {

        return sourceText;

    }// if
    else {

        return YUMRepoManager::currentTranslator->translate(context.toUtf8().constData(), sourceText.toUtf8().constData());

    }// else

}// Translate

void YUMRepoManager::on_actionFilter_triggered()
{

    if(arrowState) {

        // SlideBar going down.

        ui->slideBar->setMinimumSize(0, ui->slideBar->geometry().height());

        animationSlideBar->setStartValue(QSize(16777215, 0));
        animationSlideBar->setEndValue(QSize(16777215, 60));

        QRect startRectTableView(QPoint(ui->tableView->x(), ui->tableView->y()), QSize(ui->tableView->width(), ui->tableView->height()));
        QRect endRectTableView(QPoint(ui->tableView->x(), ui->tableView->y() + 60), QSize(ui->tableView->width(), (ui->tableView->height() - 60)));

        animationTableView->setStartValue(startRectTableView);
        animationTableView->setEndValue(endRectTableView);

        groupAnimations->start();

    }// if
    else {

        // SlideBar going up.

        ui->slideBar->setMinimumSize(0, 0);

        animationSlideBar->setStartValue(QSize(16777215, 60));
        animationSlideBar->setEndValue(QSize(16777215, 0));

        QRect startRectTableView(QPoint(ui->tableView->x(), ui->tableView->y()), QSize(ui->tableView->width(), ui->tableView->height()));
        QRect endRectTableView(QPoint(ui->tableView->x(), ui->tableView->y() - 60), QSize(ui->tableView->width(), (ui->tableView->height() + 60)));

        animationTableView->setStartValue(startRectTableView);
        animationTableView->setEndValue(endRectTableView);

        groupAnimations->start();

    }// else

    arrowState = !arrowState;

}// Filter Triggered

void YUMRepoManager::animationFinished() {

    ui->slideBar->setMinimumSize(0, ui->slideBar->geometry().height());

}// Animation Finished

void YUMRepoManager::on_comboBoxColumnFilter_activated(int index)
{

    filterColumn = index;

    proxyModel->invalidate();

    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

}// ComboBox Column Filter Activated

void YUMRepoManager::on_comboBoxRowFilter_activated(int index)
{

    Q_UNUSED(index);

    filterRow = ui->comboBoxRowFilter->currentText().split(" ").join(QString());

    proxyModel->invalidate();

    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

}// ComboBox Row Filter Activated

void YUMRepoManager::setFilterRowState(QString state) {

    if(state == "ShowAll") {

        ui->comboBoxRowFilter->addItem("Show All");
        ui->comboBoxRowFilter->addItem("Show Enables");
        ui->comboBoxRowFilter->addItem("Show Disables");

        filterRow = state;

        proxyModel->invalidate();

        ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    }// if
    else if(state == "ShowEnables") {

        ui->comboBoxRowFilter->addItem("Show Enables");
        ui->comboBoxRowFilter->addItem("Show Disables");
        ui->comboBoxRowFilter->addItem("Show All");

        filterRow = state;

        proxyModel->invalidate();

        ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    }// else if
    else if(state == "ShowDisables") {

        ui->comboBoxRowFilter->addItem("Show Disables");
        ui->comboBoxRowFilter->addItem("Show All");
        ui->comboBoxRowFilter->addItem("Show Enables");

        filterRow = state;

        proxyModel->invalidate();

        ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    }// else if
    else {

        ui->comboBoxRowFilter->addItem("Show All");
        ui->comboBoxRowFilter->addItem("Show Enables");
        ui->comboBoxRowFilter->addItem("Show Disables");

        filterRow = state;

        proxyModel->invalidate();

        ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    }// else

}// Set Filter State

bool YUMRepoManager::event(QEvent *event) {

    if(event->type() == QEvent::Close) {

        anyChangeProduced();

        saveSettings();

        exit(0);

    }// if

    QMainWindow::event(event);

    return false;

}// Event

void YUMRepoManager::changeEvent(QEvent *event) {

    if(event->type() == QEvent::LanguageChange) {

        ui->retranslateUi(this);

    }// if

    QMainWindow::changeEvent(event);

}// changeEvent

void YUMRepoManager::loadSettings() {

    QSettings mySettings("RatelSoft", "YUMRepoManager");

    mySettings.beginGroup("YUMRepoManager");

    //
    //  Geometry
    //

    QRect myRect;

    if(mySettings.value("Geometry").toInt() == 0)
        myRect = mySettings.value("Geometry", QRect(0, 0, 800, 500)).toRect();
    else
        myRect = mySettings.value("Geometry").toRect();

    setGeometry(myRect);

    //
    //  Window State
    //

    restoreState(mySettings.value("WindowState").toByteArray());

    //
    //  Filter Column State
    //

    int index = 0;

    if(!mySettings.value("FilterColumnState").toString().isEmpty()) {

        index = columnFilterLabels.indexOf(QRegExp(mySettings.value("FilterColumnState").toString()));

    }// if

    ui->comboBoxColumnFilter->setCurrentIndex(index);

    //
    //  Filter Row State
    //

    if(!mySettings.value("FilterRowState").toString().isEmpty()) {

        QString state = mySettings.value("FilterRowState").toString();

        setFilterRowState(state);

    }// if
    else {

        setFilterRowState("ShowAll");

    }// else

    //
    //  Preferred Language
    //

    preferredLanguage = mySettings.value("PreferredLanguage").toString();

    //
    //  Color
    //

    if(mySettings.contains("EnabledColor")) {

        QColor enabledColor;

        enabledColor.setRgba(mySettings.value("EnabledColor").toUInt());

        setColorEnabled(enabledColor);

    }// if

    if(mySettings.contains("DisabledColor")) {

        QColor disabledColor;

        disabledColor.setRgba(mySettings.value("DisabledColor").toUInt());

        setColorDisabled(disabledColor);

    }// if

    //
    // CreateRepo
    //

    if(mySettings.contains("CreateRepo")) {

        if(!mySettings.value("CreateRepo").toString().isEmpty()) {

            createRepoArgs = mySettings.value("CreateRepo").toString();

        }// if

    }// if

    //
    // Automatically Delete Old Packages Versions
    //

    if(mySettings.contains("AutomaticallyDeleteOldPackagesVersions")) {

        automaticallyDeleteOldPackagesVersions = mySettings.value("AutomaticallyDeleteOldPackagesVersions").toBool();

    }// if

    mySettings.endGroup();

}// loadSettings

void YUMRepoManager::saveSettings() {

    QSettings mySettings("RatelSoft", "YUMRepoManager");

    mySettings.beginGroup("YUMRepoManager");

    mySettings.setValue("Geometry", this->geometry());
    mySettings.setValue("WindowState", saveState());
    mySettings.setValue("FilterColumnState", ui->comboBoxColumnFilter->currentText());
    mySettings.setValue("FilterRowState", filterRow);
    mySettings.setValue("PreferredLanguage", preferredLanguage);
    mySettings.setValue("EnabledColor", repoDataModel->colorEnabled().rgba());
    mySettings.setValue("DisabledColor", repoDataModel->colorDisabled().rgba());
    mySettings.setValue("CreateRepo", createRepoArgs);
    mySettings.setValue("AutomaticallyDeleteOldPackagesVersions", automaticallyDeleteOldPackagesVersions);

    mySettings.endGroup();

}// saveSettings

void YUMRepoManager::on_actionPreferences_triggered()
{

    Preferences *myPreferences = new Preferences(this);

    connect(myPreferences, SIGNAL(colorEnabledChanged(QColor)), this, SLOT(setColorEnabled(QColor)));
    connect(myPreferences, SIGNAL(colorDisabledChanged(QColor)), this, SLOT(setColorDisabled(QColor)));
    connect(myPreferences, SIGNAL(languageChangeOcurred(QString)), this, SLOT(switchTranslator(QString)));

    myPreferences->show();

}// Preferences

void YUMRepoManager::repoDataModelDataChangeOcurred(const QModelIndex &topLeft, const QModelIndex &bottomRight) {

    Q_UNUSED(topLeft);
    Q_UNUSED(bottomRight);

    dataChangeOcurred();

}// repoDataModelDataChangeOcurred

void YUMRepoManager::dataChangeOcurred() {

    if(anyChangeProduced(YUMRepoData)) {

        toolBarActionsState(ActionSave, true);

    }// if
    else if(anyChangeProduced(YUMConfData)) {

        toolBarActionsState(ActionSave, true);

    }// else if
    else {

        toolBarActionsState(ActionSave, false);

    }// else

}// dataChangeOcurred

void YUMRepoManager::on_actionUpdateRepositoryList_triggered()
{

    dataFlow->fillRepoDataList();

    repoDataModel->updateModel();

}// Update Repository List Triggered

void YUMRepoManager::toolBarActionsState(YUMRepoManager::ToolBarActions action, bool state) {

    for(int count(0); count < ui->mainToolBar->actions().size(); ++count) {

        if(action == ActionRemove) {

            if(ui->mainToolBar->actions().at(count)->objectName() == "actionRemove") {

                ui->mainToolBar->actions().at(count)->setEnabled(state);

            }// if
            else {

                continue;

            }// else

        }// if

        if(action == ActionSave) {

            if(ui->mainToolBar->actions().at(count)->objectName() == "actionSave") {

                ui->mainToolBar->actions().at(count)->setEnabled(state);

            }// if
            else {

                continue;

            }// else

        }// if

        if(action == ActionExport) {

            if(ui->mainToolBar->actions().at(count)->objectName() == "actionExport_Repository") {

                ui->mainToolBar->actions().at(count)->setEnabled(state);

            }// if
            else {

                continue;

            }// else

        }// if

    }// for

}// toolBarActionsState

void YUMRepoManager::setStatusBarMessage(QString message) {

    ui->statusBar->showMessage(message, 0);

}// setStatusBarMessage

void YUMRepoManager::on_actionAbout_YUMRepoManager_triggered()
{

    About *aboutYUMRepoManager = new About(this);

    aboutYUMRepoManager->setModal(true);
    aboutYUMRepoManager->exec();

}// About YUMRepoManager

void YUMRepoManager::on_actionAbout_Qt_triggered()
{

    QMessageBox::aboutQt(this, "About Qt");

}// About Qt

void YUMRepoManager::on_actionQuit_triggered()
{

    anyChangeProduced();

    saveSettings();

    exit(0);

}// Quit Triggered
