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

#include "yumpluginmodel.h"
#include "dataflow.h"
#include "yumrepomanager.h"

#include <QDebug>

YUMPluginModel::YUMPluginModel(QObject *parent) :  QAbstractTableModel(parent)
{

}// Constructor

int YUMPluginModel::rowCount(const QModelIndex &parent) const {

    Q_UNUSED(parent);

    return DataFlow::pluginDataListSize();

}// rowCount

int YUMPluginModel::columnCount(const QModelIndex &parent) const {

    Q_UNUSED(parent);

    return 1;

}// columnCount

Qt::ItemFlags YUMPluginModel::flags(const QModelIndex &index) const {

    if(!index.isValid()) {

        return 0;

    }// if

    if(index.column() == 0) {

        return QAbstractItemModel::flags(index) | Qt::ItemIsUserCheckable;

    }// if

    return QAbstractTableModel::flags(index);

}// flags

QVariant YUMPluginModel::data(const QModelIndex &index, int role) const {

    if(!index.isValid()) {

        return QVariant();

    }// if

    if(role == Qt::DisplayRole) {

        if(index.column() == 0) {

            return DataFlow::getPluginDataList().at(index.row()).pluginName;

        }// if

    }// if

    if(role == Qt::CheckStateRole) {

        if(index.column() == 0) {

            if(DataFlow::getPluginDataList().at(index.row()).pluginEnabled[1] == "1") {

                return Qt::Checked;

            }// if

            if(DataFlow::getPluginDataList().at(index.row()).pluginEnabled[1] == "0") {

                return Qt::Unchecked;

            }// if

        }// if

    }// if

    if(role == Qt::BackgroundRole) {

        if(DataFlow::getPluginDataList().at(index.row()).pluginEnabled[1] == "1") {

            return YUMRepoManager::colorEnabled();

        }// if

        return YUMRepoManager::colorDisabled();

    }// if

    return QVariant();

}// data

bool YUMPluginModel::setData(const QModelIndex &index, const QVariant &value, int role) {

    if(!index.isValid()) {

        return false;

    }// if

    if(role == Qt::CheckStateRole) {

        if(index.column() == 0) {

            if(value == Qt::Checked) {

                emit stateChanged(index.row(), true);

            }// if

            if(value == Qt::Unchecked) {

                emit stateChanged(index.row(), false);

            }// if

            emit dataChanged(index, index);

            return true;

        }// if

    }// if

    return false;

}// setData
