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

#include "globaloptions.h"
#include "ui_globaloptions.h"
#include "dataflow.h"

#include <QStringListModel>
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>

GlobalOptions::GlobalOptions(QWidget *parent) : QDialog(parent), ui(new Ui::GlobalOptions)
{
    ui->setupUi(this);

    //
    //  ToolTip
    //

    ui->labelReposDir->setToolTip("<p>A list of directories where yum should look for .repo files which define repositories to use. Default is `/etc/yum.repos.d'.</p>");
    ui->checkBoxAssumeYes->setToolTip("<p>Determines whether or not yum prompts for confirmation of critical actions.</p>");
    ui->checkBoxAssumeNo->setToolTip("<p>If yum would prompt for confirmation of critical actions, assume the user chose no. This option overrides assumeyes.</p>");
    ui->checkBoxGPGCheck->setToolTip("<p>This tells yum whether or not it should perform a GPG signature check on packages.</p>");
    ui->checkBoxKeepCache->setToolTip("<p>Determines whether or not yum keeps the cache of headers and packages after successful installation.</p>");
    ui->checkBoxSkipBroken->setToolTip("<p>Resolve depsolve problems by removing packages<br/> that are causing problems from the transaction.</p>");
    ui->checkBoxPlugins->setToolTip("<p>Global switch to enable or disable yum plugins.</p>");
    ui->checkBoxDiskSpaceCheck->setToolTip("<p>This option disable or enable the checking for sufficient diskspace before a RPM transaction is run.</p>");
    ui->checkBoxTolerant->setToolTip("<p>If enabled, then yum will be tolerant of errors on the command line with regard to packages.<br/> For example: if"
                                       " you request to install foo, bar and baz and baz is installed; yum won't error out complaining that baz is already installed.</p>");
    ui->checkBoxObsoletes->setToolTip("<p>This option only has affect during an update.<br/> It enables yum's obsoletes processing logic."
                                        " Useful when doing distribution level upgrades.</p>");
    ui->checkBoxCleanRequirements->setToolTip("<p>When removing packages (by removal, update or obsoletion) go through each package's dependencies."
                                                " If any of them are no longer required by any other package then also mark them to be removed.</p>");
    ui->labelBandwidth->setToolTip("<p>Enable  bandwidth throttling for downloads. This option can be expressed as a absolute data rate in bytes/sec. An SI prefix (k, M"
                                     " or G) may be appended to the bandwidth value (eg. `5.5k' is 5.5 kilobytes/sec, `2M' is 2 Megabytes/sec).</p>");
    ui->checkBoxProxyEnable->setToolTip("<p>Global proxy that yum should use.</p>");
    ui->labelHTTPProxy->setToolTip("<p>URL to the proxy server.</p>");
    ui->labelProxyUsername->setToolTip("<p>Username to use for proxy</p>");
    ui->labelProxyPassword->setToolTip("<p>Password for this proxy</p>");
    ui->labelProxyPort->setToolTip("<p>Port to the proxy server</p>");
    ui->labelRetries->setToolTip("<p>Set the number of times any attempt to retrieve a file should retry before returning an error.</p>");
    ui->labelDebugLevel->setToolTip("<p>Debug message output level. Practical range is 0-10.</p>");
    ui->labelErrorLevel->setToolTip("<p>Error message output level. Practical range is 0-10.</p>");
    ui->labelTimeout->setToolTip("<p>Number of seconds to wait for a connection before timing out."
                                   " Defaults to 30 seconds. This may be too short of a time for extremely overloaded sites.</p>");
    ui->checkBoxExactArch->setToolTip("<p>Make yum update only update the architectures of packages that you have installed. ie: with this"
                                        " enabled yum will not install an i686 package to update an i386 package.</p>");
    ui->checkBoxMultilibProtected->setToolTip("<p>This tells yum whether or not it should perform a check to make sure that multilib packages are the"
                                                " same version. For example, if this option is off (rpm behaviour) pkgA-1.x86_64 and pkgA-2.i386 can be installed at the same time."
                                                " However this is very rarely desired.  Install only packages, like the kernel, are exempt from this check.</p>");
    ui->labelHttpCaching->setToolTip("<p>Determines how upstream HTTP caches are instructed to handle any HTTP downloads that Yum does. This option can take the  following values:</p>"
                                     "<p>All - means that all HTTP downloads should be cached.</p>"
                                     "<p>Packages - means that only RPM package downloads should be cached (but not repository metadata downloads).</p>"
                                     "<p>None - means that no HTTP downloads should be cached.</p>"
                                     "<p>The default is All. This is recommended unless you are experiencing caching related issues. Try to at least use Packages to minimize load"
                                     "on repository servers.</p>");
    ui->checkBoxUpgradeRequirements->setToolTip("<p>When installing/reinstalling/upgrading packages go through each package's installed dependencies and check for an update.</p>");
    ui->checkBoxRecheckInstalled->setToolTip("<p>When upgrading a package do we recheck any requirements that existed in the old package. Turning this on shouldn't do"
                                             "anything but slow yum depsolving down, however using rpm --nodeps etc. can break the rpmdb and then this will help.</p>");
    ui->labelIPResolve->setToolTip("<p>Determines how yum resolves host names.</p>");
    ui->labelCacheDir->setToolTip("<p>Directory where yum should store its cache and db files. The default is `/var/cache/yum'.</p>");
    ui->labelLogFile->setToolTip("<p>Full directory and file name for where yum should write its log file.</p>");
    ui->listViewExclude->setToolTip("<p>List  of  packages to exclude from updates or installs. This should be a space separated list."
                                   " Shell globs using wildcards (eg. * and ?) are allowed.</p>");
    ui->listViewExclude->setToolTip("<p>List  of  packages to exclude from updates or installs. This should be a space separated list."
                                      " Shell globs using wildcards (eg. * and ?) are allowed.</p>");
    ui->listViewProtected->setToolTip("<p>This is a list of packages that yum should never completely remove. So any packages which should be protected can do so by including a file with their package name in it.</p>");

    //
    //  WhatsThis
    //

    ui->labelReposDir->setWhatsThis("A list of directories where yum should look for .repo files which define repositories to use. Default is `/etc/yum.repos.d'.");
    ui->checkBoxAssumeYes->setWhatsThis("Determines whether or not yum prompts for confirmation of critical actions.");
    ui->checkBoxAssumeNo->setWhatsThis("<p>If yum would prompt for confirmation of critical actions, assume the user chose no. This option overrides assumeyes.</p>");
    ui->checkBoxGPGCheck->setWhatsThis("This tells yum whether or not it should perform a GPG signature check on packages.");
    ui->checkBoxKeepCache->setWhatsThis("Determines whether or not yum keeps the cache of headers and packages after successful installation.");
    ui->checkBoxSkipBroken->setWhatsThis("Resolve depsolve problems by removing packages that are causing problems from the transaction.");
    ui->checkBoxPlugins->setWhatsThis("Global switch to enable or disable yum plugins.");
    ui->checkBoxDiskSpaceCheck->setWhatsThis("<p>This option disable or enable the checking for sufficient diskspace before a RPM transaction is run.</p>");
    ui->checkBoxTolerant->setWhatsThis("If enabled, then yum will be tolerant of errors on the command line with regard to packages. For example: if"
                                       " you request to install foo, bar and baz and baz is installed; yum won't error out complaining that baz is already installed.");
    ui->checkBoxObsoletes->setWhatsThis("This option only has affect during an update. It enables yum's obsoletes processing logic."
                                        " Useful when doing distribution level upgrades.");
    ui->checkBoxCleanRequirements->setWhatsThis("When removing packages (by removal, update or obsoletion) go through each package's dependencies."
                                                " If any of them are no longer required by any other package then also mark them to be removed.");
    ui->labelBandwidth->setWhatsThis("Enable  bandwidth throttling for downloads. This option can be expressed as a absolute data rate in bytes/sec. An SI prefix (k, M"
                                     " or G) may be appended to the bandwidth value (eg. `5.5k' is 5.5 kilobytes/sec, `2M' is 2 Megabytes/sec).");
    ui->checkBoxProxyEnable->setWhatsThis("Global proxy that yum should use.");
    ui->labelHTTPProxy->setWhatsThis("URL to the proxy server.");
    ui->labelProxyUsername->setWhatsThis("Username to use for proxy");
    ui->labelProxyPassword->setWhatsThis("Password for this proxy");
    ui->labelProxyPort->setWhatsThis("Port to the proxy server");
    ui->labelRetries->setWhatsThis("Set the number of times any attempt to retrieve a file should retry before returning an error.");
    ui->labelDebugLevel->setWhatsThis("Debug message output level. Practical range is 0-10.");
    ui->labelErrorLevel->setWhatsThis("Error message output level. Practical range is 0-10.");
    ui->labelTimeout->setWhatsThis("Number of seconds to wait for a connection before timing out."
                                   " Defaults to 30 seconds. This may be too short of a time for extremely overloaded sites.");
    ui->checkBoxExactArch->setWhatsThis("Make yum update only update the architectures of packages that you have installed. ie: with this"
                                        " enabled yum will not install an i686 package to update an i386 package.");
    ui->checkBoxMultilibProtected->setWhatsThis("This tells yum whether or not it should perform a check to make sure that multilib packages are the"
                                                " same version. For example, if this option is off (rpm behaviour) pkgA-1.x86_64 and pkgA-2.i386 can be installed at the same time."
                                                " However this is very rarely desired.  Install only packages, like the kernel, are exempt from this check.");
    ui->labelHttpCaching->setWhatsThis("<p>Determines how upstream HTTP caches are instructed to handle any HTTP downloads that Yum does. This option can take the  following values:</p>"
                                       "<p>All - means that all HTTP downloads should be cached.</p>"
                                       "<p>Packages - means that only RPM package downloads should be cached (but not repository metadata downloads).</p>"
                                       "<p>None - means that no HTTP downloads should be cached.</p>"
                                       "<p>The default is All. This is recommended unless you are experiencing caching related issues. Try to at least use Packages to minimize load"
                                       "on repository servers.</p>");
    ui->checkBoxUpgradeRequirements->setWhatsThis("<p>When installing/reinstalling/upgrading packages go through each package's installed dependencies and check for an update.</p>");
    ui->checkBoxRecheckInstalled->setWhatsThis("<p>When upgrading a package do we recheck any requirements that existed in the old package. Turning this on shouldn't do"
                                             "anything but slow yum depsolving down, however using rpm --nodeps etc. can break the rpmdb and then this will help.</p>");
    ui->labelIPResolve->setWhatsThis("Determines how yum resolves host names.");
    ui->labelCacheDir->setWhatsThis("Directory where yum should store its cache and db files. The default is `/var/cache/yum'.");
    ui->labelLogFile->setWhatsThis("Full directory and file name for where yum should write its log file.");
    ui->listViewExclude->setWhatsThis("List  of  packages to exclude from updates or installs. This should be a space separated list."
                                   " Shell globs using wildcards (eg. * and ?) are allowed.");
    ui->listViewExclude->setWhatsThis("List  of  packages to exclude from updates or installs. This should be a space separated list."
                                      " Shell globs using wildcards (eg. * and ?) are allowed.");
    ui->listViewProtected->setWhatsThis("This is a list of packages that yum should never completely remove. So any packages which should be protected can do so by including a file with their package name in it.");

    //
    // Default values
    //

    ui->lineEditReposDir->setText(DataFlow::getRepoPath());

    ui->checkBoxKeepCache->setChecked(true);

    ui->checkBoxPlugins->setChecked(true);

    ui->checkBoxDiskSpaceCheck->setChecked(true);

    ui->checkBoxObsoletes->setChecked(true);

    ui->comboBoxBandwidthLimit->addItem("Gigabytes");
    ui->comboBoxBandwidthLimit->addItem("Megabytes");
    ui->comboBoxBandwidthLimit->addItem("kilobytes");

    ui->spinBoxRetries->setValue(10);
    ui->spinBoxDebugLevel->setValue(2);
    ui->spinBoxErrorLevel->setValue(2);
    ui->spinBoxTimeOut->setValue(30);

    ui->checkBoxExactArch->setChecked(true);
    ui->checkBoxMultilibProtected->setChecked(true);

    ui->checkBoxUpgradeRequirements->setChecked(false);

    ui->checkBoxRecheckInstalled->setChecked(false);

    ui->comboBoxHTTPCaching->addItem("All");
    ui->comboBoxHTTPCaching->addItem("Packages");
    ui->comboBoxHTTPCaching->addItem("None");

    ui->comboBoxIPResolve->addItem("IPv4");
    ui->comboBoxIPResolve->addItem("IPv6");

    ui->lineEditCacheDir->setText("/var/cache/yum/$basearch/$releasever");
    ui->lineEditLogFile->setText("/var/log/yum.log");

    //
    // Loading Data
    //

    QMapIterator<QString, QString> yumConfDataIterator(DataFlow::getYumConfData());

    while(yumConfDataIterator.hasNext()) {

        yumConfDataIterator.next();

        if(yumConfDataIterator.key() == "reposdir") {

            ui->lineEditReposDir->setText(yumConfDataIterator.value());

        }// if
        else if(yumConfDataIterator.key() == "cachedir") {

            ui->lineEditCacheDir->setText(yumConfDataIterator.value());

        }// else if
        else if(yumConfDataIterator.key() == "keepcache") {

            if(yumConfDataIterator.value() == "1")
                ui->checkBoxKeepCache->setChecked(true);
            else if(yumConfDataIterator.value() == "0")
                ui->checkBoxKeepCache->setChecked(false);

        }// else if
        else if(yumConfDataIterator.key() == "retries") {

            ui->spinBoxRetries->setValue(yumConfDataIterator.value().toInt());

        }// else if
        else if(yumConfDataIterator.key() == "debuglevel") {

            ui->spinBoxDebugLevel->setValue(yumConfDataIterator.value().toInt());

        }// else if
        else if(yumConfDataIterator.key() == "errorlevel") {

            ui->spinBoxErrorLevel->setValue(yumConfDataIterator.value().toInt());

        }// else if
        else if(yumConfDataIterator.key() == "logfile") {

            ui->lineEditLogFile->setText(yumConfDataIterator.value());

        }// else if
        else if(yumConfDataIterator.key() == "exactarch") {

            if(yumConfDataIterator.value() == "1")
                ui->checkBoxExactArch->setChecked(true);
            else if(yumConfDataIterator.value() == "0")
                ui->checkBoxExactArch->setChecked(false);

        }// else if
        else if(yumConfDataIterator.key() == "protected_multilib") {

            if(yumConfDataIterator.value() == "1")
                ui->checkBoxMultilibProtected->setChecked(true);
            else if(yumConfDataIterator.value() == "0")
                ui->checkBoxMultilibProtected->setChecked(false);

        }// else if
        else if(yumConfDataIterator.key() == "obsoletes") {

            if(yumConfDataIterator.value() == "1")
                ui->checkBoxObsoletes->setChecked(true);
            else if(yumConfDataIterator.value() == "0")
                ui->checkBoxObsoletes->setChecked(false);

        }// else if
        else if(yumConfDataIterator.key() == "tolerant") {

            if(yumConfDataIterator.value() == "1")
                ui->checkBoxTolerant->setChecked(true);
            else if(yumConfDataIterator.value() == "0")
                ui->checkBoxTolerant->setChecked(false);

        }// else if
        else if(yumConfDataIterator.key() == "clean_requirements_on_remove") {

            if(yumConfDataIterator.value() == "1")
                ui->checkBoxCleanRequirements->setChecked(true);
            else if(yumConfDataIterator.value() == "0")
                ui->checkBoxCleanRequirements->setChecked(false);

        }// else if
        else if(yumConfDataIterator.key() == "gpgcheck") {

            if(yumConfDataIterator.value() == "1")
                ui->checkBoxGPGCheck->setChecked(true);
            else if(yumConfDataIterator.value() == "0")
                ui->checkBoxGPGCheck->setChecked(false);

        }// else if
        else if(yumConfDataIterator.key() == "timeout") {

            ui->spinBoxTimeOut->setValue(yumConfDataIterator.value().toInt());

        }// else if
        else if(yumConfDataIterator.key() == "plugins") {

            if(yumConfDataIterator.value() == "1")
                ui->checkBoxPlugins->setChecked(true);
            else if(yumConfDataIterator.value() == "0")
                ui->checkBoxPlugins->setChecked(false);

        }// else if
        else if(yumConfDataIterator.key() == "diskspacecheck") {

            if(yumConfDataIterator.value() == "1")
                ui->checkBoxDiskSpaceCheck->setChecked(true);
            else if(yumConfDataIterator.value() == "0")
                ui->checkBoxDiskSpaceCheck->setChecked(false);

        }// else if
        else if(yumConfDataIterator.key() == "installonly_limit") {

        }// else if
        else if(yumConfDataIterator.key() == "color") {

        }// else if
        else if(yumConfDataIterator.key() == "skip_broken") {

            if(yumConfDataIterator.value() == "1")
                ui->checkBoxSkipBroken->setChecked(true);
            else if(yumConfDataIterator.value() == "0")
                ui->checkBoxSkipBroken->setChecked(false);

        }// else if
        else if(yumConfDataIterator.key() == "assumeyes") {

            if(yumConfDataIterator.value() == "1")
                ui->checkBoxAssumeYes->setChecked(true);
            else if(yumConfDataIterator.value() == "0")
                ui->checkBoxAssumeYes->setChecked(false);

        }// else if
        else if(yumConfDataIterator.key() == "assumeno") {

            if(yumConfDataIterator.value() == "1")
                ui->checkBoxAssumeNo->setChecked(true);
            else if(yumConfDataIterator.value() == "0")
                ui->checkBoxAssumeNo->setChecked(false);

        }// else if
        else if(yumConfDataIterator.key() == "http_caching") {

            ui->comboBoxHTTPCaching->clear();

            if(yumConfDataIterator.value() == "all") {

                ui->comboBoxHTTPCaching->addItem("All");
                ui->comboBoxHTTPCaching->addItem("Packages");
                ui->comboBoxHTTPCaching->addItem("None");

            }// if
            else if(yumConfDataIterator.value() == "packages") {

                ui->comboBoxHTTPCaching->addItem("Packages");
                ui->comboBoxHTTPCaching->addItem("None");
                ui->comboBoxHTTPCaching->addItem("All");

            }// else if
            else if(yumConfDataIterator.value() == "none") {

                ui->comboBoxHTTPCaching->addItem("None");
                ui->comboBoxHTTPCaching->addItem("All");
                ui->comboBoxHTTPCaching->addItem("Packages");

            }// else if

        }// else if
        else if(yumConfDataIterator.key() == "upgrade_requirements_on_install") {

            if(yumConfDataIterator.value() == "1" || yumConfDataIterator.value() == "True" || yumConfDataIterator.value() == "yes")
                ui->checkBoxUpgradeRequirements->setChecked(true);
            else if(yumConfDataIterator.value() == "0" || yumConfDataIterator.value() == "False" || yumConfDataIterator.value() == "no")
                ui->checkBoxUpgradeRequirements->setChecked(false);

        }// else if
        else if(yumConfDataIterator.key() == "recheck_installed_requires") {

            if(yumConfDataIterator.value() == "1" || yumConfDataIterator.value() == "True" || yumConfDataIterator.value() == "yes")
                ui->checkBoxRecheckInstalled->setChecked(true);
            else if(yumConfDataIterator.value() == "0" || yumConfDataIterator.value() == "False" || yumConfDataIterator.value() == "no")
                ui->checkBoxRecheckInstalled->setChecked(false);

        }// else if
        else if(yumConfDataIterator.key() == "ip_resolve") {

            ui->comboBoxIPResolve->clear();

            if(yumConfDataIterator.value() == "IPv4") {

                ui->comboBoxIPResolve->addItem("IPv4");
                ui->comboBoxIPResolve->addItem("IPv6");

            }// if
            else if(yumConfDataIterator.value() == "IPv6") {

                ui->comboBoxIPResolve->addItem("IPv6");
                ui->comboBoxIPResolve->addItem("IPv4");

            }// else if

        }// else if
        else if(yumConfDataIterator.key() == "throttle") {

            QString Value, Prefix;

            int Size = yumConfDataIterator.value().size();

            for(int count = 0; count < Size; count++) {

                if(yumConfDataIterator.value().at(count).isDigit())
                    Value.append(yumConfDataIterator.value().at(count));
                else if(yumConfDataIterator.value().at(count).isLetter())
                    Prefix.append(yumConfDataIterator.value().at(count));

            }// for

            ui->doubleSpinBoxBandwidthLimit->setValue(Value.toDouble());

            if(Prefix == "G") {

                ui->comboBoxBandwidthLimit->clear();

                ui->comboBoxBandwidthLimit->addItem("Gigabytes");
                ui->comboBoxBandwidthLimit->addItem("Megabytes");
                ui->comboBoxBandwidthLimit->addItem("kilobytes");

            }// if
            else if(Prefix == "M") {

                ui->comboBoxBandwidthLimit->clear();

                ui->comboBoxBandwidthLimit->addItem("Megabytes");
                ui->comboBoxBandwidthLimit->addItem("kilobytes");
                ui->comboBoxBandwidthLimit->addItem("Gigabytes");

            }// else if
            else if(Prefix == "k") {

                ui->comboBoxBandwidthLimit->clear();

                ui->comboBoxBandwidthLimit->addItem("kilobytes");
                ui->comboBoxBandwidthLimit->addItem("Gigabytes");
                ui->comboBoxBandwidthLimit->addItem("Megabytes");

            }// else if

        }// else if
        else if(yumConfDataIterator.key() == "proxy") {

                ui->checkBoxProxyEnable->setChecked(true);

                ui->labelHTTPProxy->setEnabled(true);
                ui->labelProxyUsername->setEnabled(true);
                ui->labelProxyPassword->setEnabled(true);

                ui->lineEditHTTPProxy->setEnabled(true);
                ui->lineEditProxyUsername->setEnabled(true);
                ui->lineEditProxyPassword->setEnabled(true);

                ui->labelProxyPort->setEnabled(true);
                ui->spinBoxProxyPort->setEnabled(true);

                QStringList myProxy = yumConfDataIterator.value().split(":");

                ui->lineEditHTTPProxy->setText(myProxy.at(1).mid(2));

                if(myProxy.at(2).endsWith("/"))
                    ui->spinBoxProxyPort->setValue(myProxy.at(2).mid(0, (myProxy.at(2).size() - 1)).toInt());
                else
                    ui->spinBoxProxyPort->setValue(myProxy.at(2).toInt());

        }// else if
        else if(yumConfDataIterator.key() == "#proxy") {

                QStringList myProxy = yumConfDataIterator.value().split(":");

                ui->lineEditHTTPProxy->setText(myProxy.at(1).mid(2));

                if(myProxy.at(2).endsWith("/"))
                    ui->spinBoxProxyPort->setValue(myProxy.at(2).mid(0, (myProxy.at(2).size() - 1)).toInt());
                else
                    ui->spinBoxProxyPort->setValue(myProxy.at(2).toInt());

        }// else if
        else if(yumConfDataIterator.key() == "proxy_username" || yumConfDataIterator.key() == "#proxy_username") {

            ui->lineEditProxyUsername->setText(yumConfDataIterator.value());

        }// else if
        else if(yumConfDataIterator.key() == "proxy_password" || yumConfDataIterator.key() == "#proxy_password") {

            ui->lineEditProxyPassword->setText(yumConfDataIterator.value());

        }// else if
        else if(yumConfDataIterator.key() == "exclude") {

            if(!yumConfDataIterator.value().isEmpty()) {

                QString Value;

                int Size = yumConfDataIterator.value().size();

                for(int count = 0; count < Size; count++) {

                    if(yumConfDataIterator.value().at(count).isSpace()) {

                        if((count + 1) == Size)
                            continue;
                        else if(yumConfDataIterator.value().at(count + 1).isSpace())
                            continue;

                    }// if

                    Value.append(yumConfDataIterator.value().at(count));

                }// for

                excludeList = Value.split(" ");

            }// if

        }// else if

    }// while

    excludeListModel = new QStringListModel(this);
    excludeListModel->setStringList(excludeList);

    ui->listViewExclude->setModel(excludeListModel);
    ui->listViewExclude->setEditTriggers(QAbstractItemView::DoubleClicked);

    DataFlow dataFlow;
    dataFlow.fillProtectedList();

    ui->listViewProtected->setModel(DataFlow::protectedListModel);
    ui->listViewProtected->setEditTriggers(QAbstractItemView::DoubleClicked);

    protectedListModelChanges = 0;

    connect(DataFlow::protectedListModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(protectedListModelChanged(QModelIndex,QModelIndex)));

}// Constructor

GlobalOptions::~GlobalOptions()
{

    delete ui;

}// Destructor

void GlobalOptions::on_pushButtonExcludeAdd_clicked()
{

    int Rows = excludeListModel->rowCount();

    excludeListModel->insertRows(Rows, 1);

    QModelIndex excludeListRowIndex = excludeListModel->index(Rows);

    ui->listViewExclude->setCurrentIndex(excludeListRowIndex);

    ui->listViewExclude->edit(excludeListRowIndex);

}// Exclude Add Clicked

void GlobalOptions::on_pushButtonExcludeRemove_clicked()
{

    excludeListModel->removeRows(ui->listViewExclude->currentIndex().row(), 1);

}// Exclude Remove Clicked

void GlobalOptions::on_pushButtonAddProtected_clicked()
{

    int rows = DataFlow::protectedListModel->rowCount();

    DataFlow::protectedListModel->insertRows(rows, 1);

    QModelIndex protectedListRowIndex = DataFlow::protectedListModel->index(rows);

    ui->listViewProtected->setCurrentIndex(protectedListRowIndex);

    ui->listViewProtected->edit(protectedListRowIndex);

}// Protected Add Clicked

void GlobalOptions::on_pushButtonEdit_clicked()
{
    QItemSelectionModel *selection = ui->listViewProtected->selectionModel();

    if(selection->hasSelection()) {

        textEditor = new TextEditor(DataFlow::protectedListModel->data(ui->listViewProtected->currentIndex(), Qt::DisplayRole).toString(), "protected", this);
        textEditor->setModal(true);
        textEditor->exec();

    }// if
    else {

        YUMRepoManager::showMessage("Information", tr("You have to select a protected package file first."));

    }// else

}// Protected Edit Clicked

void GlobalOptions::on_pushButtonRemoveProtected_clicked()
{

    if(YUMRepoManager::showMessage("Question", tr("Are you sure you want to delete this file?")) == QMessageBox::Yes) {

        QFile file(DataFlow::protectedListModel->data(ui->listViewProtected->currentIndex(), Qt::DisplayRole).toString());

        if(!file.open(QFile::WriteOnly | QFile::Text)) {

            YUMRepoManager::showMessage("Error", tr("Access denied when trying to delete protected packages file."));

            file.close();

            return;

        }// if

        file.remove();

        file.close();

        DataFlow::protectedListModel->removeRows(ui->listViewProtected->currentIndex().row(), 1);

    }// if

}// Protected Remove Clicked

void GlobalOptions::on_checkBoxProxyEnable_stateChanged(int arg1)
{

    Q_UNUSED(arg1);

    if(this->isVisible()) {

        if(ui->checkBoxProxyEnable->isChecked()) {

            ui->labelHTTPProxy->setEnabled(true);
            ui->labelProxyUsername->setEnabled(true);
            ui->labelProxyPassword->setEnabled(true);
            ui->lineEditHTTPProxy->setEnabled(true);
            ui->lineEditProxyUsername->setEnabled(true);
            ui->lineEditProxyPassword->setEnabled(true);
            ui->labelProxyPort->setEnabled(true);
            ui->spinBoxProxyPort->setEnabled(true);

        }// if
        else {

            ui->labelHTTPProxy->setEnabled(false);
            ui->labelProxyUsername->setEnabled(false);
            ui->labelProxyPassword->setEnabled(false);
            ui->lineEditHTTPProxy->setEnabled(false);
            ui->lineEditProxyUsername->setEnabled(false);
            ui->lineEditProxyPassword->setEnabled(false);
            ui->labelProxyPort->setEnabled(false);
            ui->spinBoxProxyPort->setEnabled(false);

        }// else

    }// if

}// CheckBox Proxy State Changed

void GlobalOptions::on_toolButtonCacheDir_clicked()
{

    QString cacheDir = QFileDialog::getExistingDirectory(this, tr("Select The Cache's Directory"), "/var/cache/yum/");

    if (!cacheDir.isEmpty()) {

      ui->lineEditCacheDir->setText(cacheDir);

    }//if

}// ToolButtonCacheDir Clicked

void GlobalOptions::on_toolButtonReposDir_clicked()
{

    QString reposDir;

    if(!ui->lineEditReposDir->text().isEmpty())
        reposDir = QFileDialog::getExistingDirectory(this, tr("Select The Repositories Directory"), ui->lineEditReposDir->text());
    else
        reposDir = QFileDialog::getExistingDirectory(this, tr("Select The Repositories Directory"), "/etc/yum.repos.d/");

    if (!reposDir.isEmpty()) {

      ui->lineEditReposDir->setText(reposDir);

    }//if

}// ToolButtonReposDir Clicked

void GlobalOptions::protectedListModelChanged(const QModelIndex topLeft, const QModelIndex bottomRight) {

    Q_UNUSED(topLeft);
    Q_UNUSED(bottomRight);

    protectedListModelChanges++;

    if(protectedListModelChanges == 1) {

        QModelIndex currentIndex = ui->listViewProtected->currentIndex();

        QVariant rowData = DataFlow::protectedListModel->data(currentIndex, Qt::DisplayRole);

        QString formatedRowData = rowData.toString();

        if(!formatedRowData.isEmpty() && !formatedRowData.endsWith(".conf")) {

            formatedRowData.append(".conf");

            rowData = formatedRowData;

            DataFlow::protectedListModel->setData(topLeft, rowData);

            QFile file(DataFlow::getYumProtectedPath() + formatedRowData);

            if(!file.open(QFile::ReadWrite | QFile::Text)) {

                YUMRepoManager::showMessage("Error", tr("Access denied when trying to create protected packages file."));

                DataFlow::protectedListModel->removeRows(currentIndex.row(), 1);

            }// if

            file.close();

        }// if
        else if(formatedRowData.isEmpty()) {

            YUMRepoManager::showMessage("Error", tr("You have to specify a name for the protected packages file, using the following syntax: name.conf"));

            DataFlow::protectedListModel->removeRows(currentIndex.row(), 1);

        }// else if

        protectedListModelChanges = 0;

    }// if

}// protectedListModel Changed

void GlobalOptions::on_buttonBoxOK_accepted()
{

    //
    /// Repos Directory
    //

    if(!ui->lineEditReposDir->text().isEmpty())
        DataFlow::updateYumConfData("reposdir", ui->lineEditReposDir->text());
    else
        DataFlow::updateYumConfData("reposdir", DataFlow::getRepoPath());

    //
    /// Assume Yes
    //

    if(ui->checkBoxAssumeYes->isChecked())
        DataFlow::updateYumConfData("assumeyes", "1");
    else
        DataFlow::updateYumConfData("assumeyes", "0");

    //
    /// Assume No
    //

    if(ui->checkBoxAssumeNo->isChecked())
        DataFlow::updateYumConfData("assumeno", "1");
    else
        DataFlow::updateYumConfData("assumeno", "0");

    //
    /// GPG Check
    //

    if(ui->checkBoxGPGCheck->isChecked()) {

        DataFlow::updateYumConfData("gpgcheck", "1");

        for(int index = 0; index < DataFlow::repoDataListSize(); ++index) {

            QVariant value[3];

            value[2] = "1";

            DataFlow::setRepoDataList(index, RepoData::GPGCheck, 2, value);

        }// for

    }// if
    else {

        DataFlow::updateYumConfData("gpgcheck", "0");

        for(int index = 0; index < DataFlow::repoDataListSize(); ++index) {

            QVariant value[3];

            value[2] = "0";

            DataFlow::setRepoDataList(index, RepoData::GPGCheck, 2, value);

        }// for

    }// else

    //
    /// KeepCache
    //

    if(ui->checkBoxKeepCache->isChecked())
        DataFlow::updateYumConfData("keepcache", "1");
    else
        DataFlow::updateYumConfData("keepcache", "0");

    //
    /// Skip Broken
    //

    if(ui->checkBoxSkipBroken->isChecked())
        DataFlow::updateYumConfData("skip_broken", "1");
    else
        DataFlow::updateYumConfData("skip_broken", "0");

    //
    /// Plugins
    //

    if(ui->checkBoxPlugins->isChecked())
        DataFlow::updateYumConfData("plugins", "1");
    else
        DataFlow::updateYumConfData("plugins", "0");

    //
    /// Disk Space Check
    //

    if(ui->checkBoxDiskSpaceCheck->isChecked())
        DataFlow::updateYumConfData("diskspacecheck", "1");
    else
        DataFlow::updateYumConfData("diskspacecheck", "0");

    //
    /// Tolerant
    //

    if(ui->checkBoxTolerant->isChecked())
        DataFlow::updateYumConfData("tolerant", "1");
    else
        DataFlow::updateYumConfData("tolerant", "0");

    //
    /// Obsoletes
    //

    if(ui->checkBoxObsoletes->isChecked())
        DataFlow::updateYumConfData("obsoletes", "1");
    else
        DataFlow::updateYumConfData("obsoletes", "0");

    //
    /// Clean Requirements On Remove
    //

    if(ui->checkBoxCleanRequirements->isChecked())
        DataFlow::updateYumConfData("clean_requirements_on_remove", "1");
    else
        DataFlow::updateYumConfData("clean_requirements_on_remove", "0");

    //
    /// Throttle
    //

    if(ui->comboBoxBandwidthLimit->currentText() == "Gigabytes") {

        DataFlow::updateYumConfData("throttle", QString("%1").arg(ui->doubleSpinBoxBandwidthLimit->value()) + "G");

    }// if
    else if(ui->comboBoxBandwidthLimit->currentText() == "Megabytes") {

        DataFlow::updateYumConfData("throttle", QString("%1").arg(ui->doubleSpinBoxBandwidthLimit->value()) + "M");

    }// else if
    else if(ui->comboBoxBandwidthLimit->currentText() == "kilobytes") {

        DataFlow::updateYumConfData("throttle", QString("%1").arg(ui->doubleSpinBoxBandwidthLimit->value()) + "k");

    }// else if

    //
    /// Proxy
    //

    if(ui->checkBoxProxyEnable->isChecked()) {

        if(ui->lineEditHTTPProxy->text().isEmpty()) {

            YUMRepoManager::showMessage("Error", tr("The Proxy's address is empty."));

            return;

        }// if

        if(ui->spinBoxProxyPort->value() == 0) {

            YUMRepoManager::showMessage("Error", tr("The Proxy's port is empty."));

            return;

        }// if

        if(ui->lineEditProxyUsername->text().isEmpty() && !ui->lineEditProxyPassword->text().isEmpty()) {

            YUMRepoManager::showMessage("Error", tr("You have to specified a username."));

            return;

        }// if

        if(!ui->lineEditProxyUsername->text().isEmpty() && ui->lineEditProxyPassword->text().isEmpty()) {

            YUMRepoManager::showMessage("Error", tr("You have to specified a password."));

            return;

        }// if

        DataFlow::updateYumConfData("#proxy");

        DataFlow::updateYumConfData("proxy", QString("http://%1:%2").arg(ui->lineEditHTTPProxy->text()).arg(ui->spinBoxProxyPort->value()));

    }// if
    else if(!ui->lineEditHTTPProxy->text().isEmpty() && ui->spinBoxProxyPort->value() != 0) {

        DataFlow::updateYumConfData("proxy");

        DataFlow::updateYumConfData("#proxy", QString("http://%1:%2").arg(ui->lineEditHTTPProxy->text()).arg(ui->spinBoxProxyPort->value()));

    }// else if
    else {

        DataFlow::updateYumConfData("proxy");
        DataFlow::updateYumConfData("#proxy");

    }// else

    //
    /// Proxy Username
    //

    if(ui->checkBoxProxyEnable->isChecked() && !ui->lineEditProxyUsername->text().isEmpty()) {

        DataFlow::updateYumConfData("#proxy_username");

        DataFlow::updateYumConfData("proxy_username", ui->lineEditProxyUsername->text());

    }// if
    else if(!ui->checkBoxProxyEnable->isChecked() && !ui->lineEditProxyUsername->text().isEmpty()) {

        DataFlow::updateYumConfData("proxy_username");

        DataFlow::updateYumConfData("#proxy_username", ui->lineEditProxyUsername->text());

    }// else if
    else {

        DataFlow::updateYumConfData("proxy_username");
        DataFlow::updateYumConfData("#proxy_username");

    }// else


    //
    /// Proxy Password
    //

    if(ui->checkBoxProxyEnable->isChecked() && !ui->lineEditProxyPassword->text().isEmpty()) {

        DataFlow::updateYumConfData("#proxy_password");

        DataFlow::updateYumConfData("proxy_password", ui->lineEditProxyPassword->text());

    }// if
    else if(!ui->checkBoxProxyEnable->isChecked() && !ui->lineEditProxyPassword->text().isEmpty()) {

        DataFlow::updateYumConfData("proxy_password");

        DataFlow::updateYumConfData("#proxy_password", ui->lineEditProxyPassword->text());

    }// else if
    else {

        DataFlow::updateYumConfData("proxy_password");
        DataFlow::updateYumConfData("#proxy_password");

    }// else

    //
    /// Retries
    //

    if(ui->spinBoxRetries->value() != 0) {

        DataFlow::updateYumConfData("#retries");
        DataFlow::updateYumConfData("retries", QString("%1").arg(ui->spinBoxRetries->value()));

    }// if
    else {

        DataFlow::updateYumConfData("retries");
        DataFlow::updateYumConfData("#retries", "10");

    }// else

    //
    /// Debug Level
    //

    if(ui->spinBoxDebugLevel->value() != 0) {

        DataFlow::updateYumConfData("#debuglevel");
        DataFlow::updateYumConfData("debuglevel", QString("%1").arg(ui->spinBoxDebugLevel->value()));

    }// if
    else {

        DataFlow::updateYumConfData("debuglevel");
        DataFlow::updateYumConfData("#debuglevel", "2");

    }// else

    //
    /// Error Level
    //

    if(ui->spinBoxErrorLevel->value() != 0) {

        DataFlow::updateYumConfData("#errorlevel");
        DataFlow::updateYumConfData("errorlevel", QString("%1").arg(ui->spinBoxErrorLevel->value()));

    }// if
    else {

        DataFlow::updateYumConfData("errorlevel");
        DataFlow::updateYumConfData("#errorlevel", "2");

    }// else

    //
    /// Timeout
    //

    if(ui->spinBoxTimeOut->value() != 0) {

        DataFlow::updateYumConfData("#timeout");
        DataFlow::updateYumConfData("timeout", QString("%1").arg(ui->spinBoxTimeOut->value()));

    }// if
    else {

        DataFlow::updateYumConfData("timeout");
        DataFlow::updateYumConfData("#timeout", "30");

    }// else

    //
    /// Exact Arch
    //

    if(ui->checkBoxExactArch->isChecked()) {

        DataFlow::updateYumConfData("exactarch", "1");

    }// if
    else {

        DataFlow::updateYumConfData("exactarch", "0");

    }// else

    //
    /// Multilib Protected
    //

    if(ui->checkBoxMultilibProtected->isChecked()) {

        DataFlow::updateYumConfData("protected_multilib", "1");

    }// if
    else {

        DataFlow::updateYumConfData("protected_multilib", "0");

    }// else

    //
    /// HTTP Caching
    //

    QString httpCachingValue;

    if(ui->comboBoxHTTPCaching->currentText() == "All") {

        httpCachingValue = "all";

    }// if
    else if(ui->comboBoxHTTPCaching->currentText() == "Packages") {

        httpCachingValue = "packages";

    }// else if
    else if(ui->comboBoxHTTPCaching->currentText() == "None") {

        httpCachingValue = "none";

    }// else if

    DataFlow::updateYumConfData("http_caching", httpCachingValue);

    //
    /// IP Resolve
    //

    if(ui->comboBoxIPResolve->currentText() == "IPv4") {

        DataFlow::updateYumConfData("ip_resolve", "IPv4");

    }// if
    else if(ui->comboBoxIPResolve->currentText() == "IPv6") {

        DataFlow::updateYumConfData("ip_resolve", "IPv6");

    }// else if

    //
    /// Upgrade Requirements On Install
    //

    if(ui->checkBoxUpgradeRequirements->isChecked()) {

        DataFlow::updateYumConfData("upgrade_requirements_on_install", "1");

    }// if
    else {

        DataFlow::updateYumConfData("upgrade_requirements_on_install", "0");

    }// else

    //
    /// Recheck Installed Requires
    //

    if(ui->checkBoxRecheckInstalled->isChecked()) {

        DataFlow::updateYumConfData("recheck_installed_requires", "1");

    }// if
    else {

        DataFlow::updateYumConfData("recheck_installed_requires", "0");

    }// else

    //
    /// Cachedir
    //

    if(!ui->lineEditCacheDir->text().isEmpty())
        DataFlow::updateYumConfData("cachedir", ui->lineEditCacheDir->text());
    else
        DataFlow::updateYumConfData("cachedir", "/var/cache/yum");

    //
    /// Logfile
    //

    if(!ui->lineEditLogFile->text().isEmpty())
        DataFlow::updateYumConfData("logfile", ui->lineEditLogFile->text());
    else
        DataFlow::updateYumConfData("logfile", "/var/log/yum.log");

    //
    /// Exclude
    //

    int Rows = excludeListModel->rowCount();

    if(Rows > 0) {

        QString toExclude;

        for(int count = 0; count < Rows; count++)
            toExclude.append(ui->listViewExclude->model()->data(ui->listViewExclude->model()->index(count, 0)).toString() + " ");

        DataFlow::updateYumConfData("exclude", toExclude);

    }// if
    else {

        DataFlow::updateYumConfData("exclude");

    }// if

    emit saveData(YUMRepoManager::YUMConfData);

    this->accept();

}// Accepted

void GlobalOptions::on_buttonBoxOK_rejected()
{

    this->close();

}// ButtonBoxOK Rejected
