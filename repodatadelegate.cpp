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

#include "repodatadelegate.h"
#include "proxywidget.h"
#include "mywidget.h"
#include "bandwidthwidget.h"
#include "metadataexpirewidget.h"
#include "yumrepomanager.h"

#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QRgb>
#include <QDebug>

RepoDataDelegate::RepoDataDelegate(QObject *parent) : QStyledItemDelegate(parent)
{

    proxyURL = new QUrl;

}// Constructor

RepoDataDelegate::~RepoDataDelegate() {

    delete proxyURL;

}// Destructor

QWidget *RepoDataDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {

    QString columnHeader = YUMRepoManager::proxyModel->headerData(index.column(), Qt::Horizontal, Qt::DisplayRole).toString();

    if(columnHeader.isEmpty()) {

        return QStyledItemDelegate::createEditor(parent, option, index);

    }// if

    if(columnHeader == "GPG Check" || columnHeader == YUMRepoManager::currentTranslator->translate("RepoDataModel", "GPG Check")) {

        QComboBox *comboBox = new QComboBox(parent);

        return comboBox;

    }// if
    else if(columnHeader == "Proxy" || columnHeader == YUMRepoManager::currentTranslator->translate("RepoDataModel", "Proxy")) {

        ProxyWidget *proxy = new ProxyWidget(parent);

        proxy->setPortMinimumValue("1");
        proxy->setPortMaximumValue("65535");

        return proxy;

    }// else if
    else if(columnHeader == "Proxy Password" || columnHeader == YUMRepoManager::currentTranslator->translate("RepoDataModel", "Proxy Password")) {

        QLineEdit *lineEditPassword = new QLineEdit(parent);

        return lineEditPassword;

    }// else if
    else if(columnHeader == "Bandwidth Throttle" || columnHeader == YUMRepoManager::currentTranslator->translate("RepoDataModel", "Bandwidth Throttle")) {

        MyWidget *bandwidth = new BandwidthWidget(parent);

        bandwidth->setSpinBoxMaximumValue("65535");

        return bandwidth;

    }// else if
    else if(columnHeader == "Skip If Unavailable" || columnHeader == YUMRepoManager::currentTranslator->translate("RepoDataModel", "Skip If Unavailable")) {

        QComboBox *comboBox = new QComboBox(parent);

        return comboBox;

    }// else if
    else if(columnHeader == "Failover Method" || columnHeader == YUMRepoManager::currentTranslator->translate("RepoDataModel", "Failover Method")) {

        QComboBox *comboBox = new QComboBox(parent);

        return comboBox;

    }// else if
    else if(columnHeader == "Metadata Expire" || columnHeader == YUMRepoManager::currentTranslator->translate("RepoDataModel", "Metadata Expire")) {

        MyWidget *metadataExpire = new MetadataExpireWidget(parent);

        metadataExpire->setSpinBoxMaximumValue("65535");

        return metadataExpire;

    }// else if
    else if(columnHeader == "Parallel Download" || columnHeader == YUMRepoManager::currentTranslator->translate("RepoDataModel", "Parallel Download")) {

        QComboBox *comboBox = new QComboBox(parent);

        return comboBox;

    }// else if
    else {

        return QStyledItemDelegate::createEditor(parent, option, index);

    }// else

}// createEditor

void RepoDataDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {

    QString columnHeader = YUMRepoManager::proxyModel->headerData(index.column(), Qt::Horizontal, Qt::DisplayRole).toString();

    if(columnHeader.isEmpty()) {

        QStyledItemDelegate::setEditorData(editor, index);

        return;

    }// if

    if(columnHeader == "GPG Check" || columnHeader == YUMRepoManager::currentTranslator->translate("RepoDataModel", "GPG Check")) {

        QString value = index.model()->data(index, Qt::EditRole).toString();

        QComboBox *comboBox = static_cast<QComboBox*>(editor);

        if(value == "Yes" || value == YUMRepoManager::currentTranslator->translate("RepoDataDelegate", "Yes")) {

            comboBox->addItem(tr("Yes"));
            comboBox->addItem(tr("No"));

        }// if
        else {

            comboBox->addItem(tr("No"));
            comboBox->addItem(tr("Yes"));

        }// else if

    }// else if
    else if(columnHeader == "Proxy" || columnHeader == YUMRepoManager::currentTranslator->translate("RepoDataModel", "Proxy")) {

        ProxyWidget *proxy = static_cast<ProxyWidget*>(editor);

        proxyURL->setUrl(index.model()->data(index, Qt::DisplayRole).toString());

        if(!proxyURL->isEmpty()) {

            proxy->setIpAddressText(proxyURL->host());

            proxy->setPortValue(QString("%1").arg(proxyURL->port()));

        }// if
        else {

            proxy->setPortValue("3128");

        }// else

    }// else if
    else if(columnHeader == "Proxy Password" || columnHeader == YUMRepoManager::currentTranslator->translate("RepoDataModel", "Proxy Password")) {

        QLineEdit *lineEditPassword = static_cast<QLineEdit*>(editor);

        lineEditPassword->setEchoMode(QLineEdit::Password);

    }// else if
    else if(columnHeader == "Bandwidth Throttle" || columnHeader == YUMRepoManager::currentTranslator->translate("RepoDataModel", "Bandwidth Throttle")) {

        MyWidget *bandwidth = static_cast<BandwidthWidget*>(editor);

        QString value = index.model()->data(index, Qt::DisplayRole).toString();

        if(!value.isEmpty()) {

            QStringList amountAndBandwidth = value.split(" ");

            bandwidth->setSpinBoxValue(amountAndBandwidth.at(0));
            bandwidth->setComboBoxValue(amountAndBandwidth.at(1));

        }// if

    }// else if
    else if(columnHeader == "Skip If Unavailable" || columnHeader == YUMRepoManager::currentTranslator->translate("RepoDataModel", "Skip If Unavailable")) {

        QString value = index.model()->data(index, Qt::EditRole).toString();

        QComboBox *comboBox = static_cast<QComboBox*>(editor);

        if(value == "Yes" || value == YUMRepoManager::currentTranslator->translate("RepoDataDelegate", "Yes")) {

            comboBox->addItem(tr("Yes"));
            comboBox->addItem(tr("No"));

        }// if
        else {

            comboBox->addItem(tr("No"));
            comboBox->addItem(tr("Yes"));

        }// else if

    }// else if
    else if(columnHeader == "Failover Method" || columnHeader == YUMRepoManager::currentTranslator->translate("RepoDataModel", "Failover Method")) {

        QString value = index.model()->data(index, Qt::EditRole).toString();

        QComboBox *comboBox = static_cast<QComboBox*>(editor);

        if(value == "roundrobin") {

            comboBox->addItem("roundrobin");
            comboBox->addItem("priority");

        }// if
        else {

            comboBox->addItem("priority");
            comboBox->addItem("roundrobin");

        }// else if

    }// else if
    else if(columnHeader == "Metadata Expire" || columnHeader == YUMRepoManager::currentTranslator->translate("RepoDataModel", "Metadata Expire")) {

        MyWidget *metadataExpire = static_cast<MetadataExpireWidget*>(editor);

        QString value = index.model()->data(index, Qt::DisplayRole).toString();

        if(!value.isEmpty()) {

            QStringList amountAndFrequency = value.split(" ");

            metadataExpire->setSpinBoxValue(amountAndFrequency.at(0));
            metadataExpire->setComboBoxValue(amountAndFrequency.at(1));

        }// if

    }// else if
    else if(columnHeader == "Parallel Download" || columnHeader == YUMRepoManager::currentTranslator->translate("RepoDataModel", "Parallel Download")) {

        QString value = index.model()->data(index, Qt::EditRole).toString();

        QComboBox *comboBox = static_cast<QComboBox*>(editor);

        if(value == "Yes" || value == YUMRepoManager::currentTranslator->translate("RepoDataDelegate", "Yes")) {

            comboBox->addItem(tr("Yes"));
            comboBox->addItem(tr("No"));

        }// if
        else {

            comboBox->addItem(tr("No"));
            comboBox->addItem(tr("Yes"));

        }// else if

    }// else if
    else {

        QStyledItemDelegate::setEditorData(editor, index);

    }// else

}// setEditorData

void RepoDataDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {

    QString columnHeader = YUMRepoManager::proxyModel->headerData(index.column(), Qt::Horizontal, Qt::DisplayRole).toString();

    if(columnHeader.isEmpty()) {

        QStyledItemDelegate::setModelData(editor, model, index);

        return;

    }// if

    if(columnHeader == "GPG Check" || columnHeader == YUMRepoManager::currentTranslator->translate("RepoDataModel", "GPG Check")) {

        QComboBox *comboBox = static_cast<QComboBox*>(editor);

        QString value = comboBox->currentText();

        model->setData(index, value, Qt::EditRole);

    }// else if
    else if(columnHeader == "Proxy" || columnHeader == YUMRepoManager::currentTranslator->translate("RepoDataModel", "Proxy")) {

        ProxyWidget *proxy = static_cast<ProxyWidget*>(editor);

        QString value = "";

        if(!proxy->ipAddressText().isEmpty()) {

            value = "http://" + proxy->ipAddressText() + ":" + proxy->portValue() + proxyURL->path();

            model->setData(index, value, Qt::EditRole);

        }// if
        else {

            value.clear();

            model->setData(index, value, Qt::EditRole);

        }// else

    }// else if
    else if(columnHeader == "Proxy Password" || columnHeader == YUMRepoManager::currentTranslator->translate("RepoDataModel", "Proxy Password")) {

        QLineEdit *lineEditPassword = static_cast<QLineEdit*>(editor);

        QString value = lineEditPassword->text();

        model->setData(index, value, Qt::EditRole);

    }// else if
    else if(columnHeader == "Bandwidth Throttle" || columnHeader == YUMRepoManager::currentTranslator->translate("RepoDataModel", "Bandwidth Throttle")) {

        MyWidget *bandwidth = static_cast<BandwidthWidget*>(editor);

        QString value;

        if(!bandwidth->spinBoxValue().isEmpty()) {

            value = bandwidth->spinBoxValue() + " " + bandwidth->comboBoxValue();

            model->setData(index, value, Qt::EditRole);

        }// if

    }// else if
    else if(columnHeader == "Skip If Unavailable" || columnHeader == YUMRepoManager::currentTranslator->translate("RepoDataModel", "Skip If Unavailable")) {

        QComboBox *comboBox = static_cast<QComboBox*>(editor);

        QString value = comboBox->currentText();

        model->setData(index, value, Qt::EditRole);

    }// else if
    else if(columnHeader == "Failover Method" || columnHeader == YUMRepoManager::currentTranslator->translate("RepoDataModel", "Failover Method")) {

        QComboBox *comboBox = static_cast<QComboBox*>(editor);

        QString value = comboBox->currentText();

        model->setData(index, value, Qt::EditRole);

    }// else if
    else if(columnHeader == "Metadata Expire" || columnHeader == YUMRepoManager::currentTranslator->translate("RepoDataModel", "Metadata Expire")) {

        MyWidget *metadataExpire = static_cast<MetadataExpireWidget*>(editor);

        QString value;

        if(!metadataExpire->spinBoxValue().isEmpty()) {

            value = metadataExpire->spinBoxValue() + " " + metadataExpire->comboBoxValue();

            model->setData(index, value, Qt::EditRole);

        }// if

    }// else if
    else if(columnHeader == "Parallel Download" || columnHeader == YUMRepoManager::currentTranslator->translate("RepoDataModel", "Parallel Download")) {

        QComboBox *comboBox = static_cast<QComboBox*>(editor);

        QString value = comboBox->currentText();

        model->setData(index, value, Qt::EditRole);

    }// else if
    else {

        QStyledItemDelegate::setModelData(editor, model, index);

    }// else

}// setModelData

void RepoDataDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const {

    QString columnHeader = YUMRepoManager::proxyModel->headerData(index.column(), Qt::Horizontal, Qt::DisplayRole).toString();

    if(columnHeader.isEmpty()) {

        QStyledItemDelegate::updateEditorGeometry(editor, option, index);

        return;

    }// if

    if(columnHeader == "GPG Check" || columnHeader == YUMRepoManager::currentTranslator->translate("RepoDataModel", "GPG Check")) {

        editor->setGeometry(option.rect);

    }// else if
    else if(columnHeader == "Proxy" || columnHeader == YUMRepoManager::currentTranslator->translate("RepoDataModel", "Proxy")) {

        editor->setGeometry(option.rect);

    }// else if
    else if(columnHeader == "Proxy Password" || columnHeader == YUMRepoManager::currentTranslator->translate("RepoDataModel", "Proxy Password")) {

        editor->setGeometry(option.rect);

    }// else if
    else if(columnHeader == "Bandwidth Throttle" || columnHeader == YUMRepoManager::currentTranslator->translate("RepoDataModel", "Bandwidth Throttle")) {

        editor->setGeometry(option.rect);

    }// else if
    else if(columnHeader == "Skip If Unavailable" || columnHeader == YUMRepoManager::currentTranslator->translate("RepoDataModel", "Skip If Unavailable")) {

        editor->setGeometry(option.rect);

    }// else if
    else if(columnHeader == "Failover Method" || columnHeader == YUMRepoManager::currentTranslator->translate("RepoDataModel", "Failover Method")) {

        editor->setGeometry(option.rect);

    }// else if
    else if(columnHeader == "Metadata Expire" || columnHeader == YUMRepoManager::currentTranslator->translate("RepoDataModel", "Metadata Expire")) {

        editor->setGeometry(option.rect);

    }// else if
    else if(columnHeader == "Parallel Download" || columnHeader == YUMRepoManager::currentTranslator->translate("RepoDataModel", "Parallel Download")) {

        editor->setGeometry(option.rect);

    }// else if
    else {

        QStyledItemDelegate::updateEditorGeometry(editor, option, index);

    }// else

}// updateEditorGeometry

QSize RepoDataDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {

    QString columnHeader = YUMRepoManager::proxyModel->headerData(index.column(), Qt::Horizontal, Qt::DisplayRole).toString();

    if(columnHeader.isEmpty()) {

        return QStyledItemDelegate::sizeHint(option, index);

    }// if

    if(columnHeader == "Proxy" || columnHeader == YUMRepoManager::currentTranslator->translate("RepoDataModel", "Proxy")) {

        ProxyWidget proxy;

        return proxy.sizeHint();

    }// if
    else if(columnHeader == "Metadata Expire" || columnHeader == YUMRepoManager::currentTranslator->translate("RepoDataModel", "Metadata Expire")) {

        MetadataExpireWidget metadataExpire;

        return metadataExpire.sizeHint();

    }// else if
    else {

        return QStyledItemDelegate::sizeHint(option, index);

    }// else

}// sizeHint
