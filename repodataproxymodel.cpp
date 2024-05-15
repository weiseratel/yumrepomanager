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

#include "repodataproxymodel.h"
#include "dataflow.h"
#include "yumrepomanager.h"

RepoDataProxyModel::RepoDataProxyModel(QObject *parent) : QSortFilterProxyModel(parent)
{

}// Constructor

bool RepoDataProxyModel::filterAcceptsColumn(int source_column, const QModelIndex &source_parent) const {

    if(YUMRepoManager::filterColumn != 0) {

        if(YUMRepoManager::filterColumn == 1) {

            if(source_column == 0 || source_column == 1 || source_column == 2) {

                return true;

            }// if

            return false;

        }// if

        if(YUMRepoManager::filterColumn == 6) {

            if(source_column == 11 || source_column == 12 || source_column == 13 || source_column == 14 || source_column == 15 || source_column == 16) {

                return true;

            }// if

            return false;

        }// if

        int mapToColumn;

        if(YUMRepoManager::filterColumn > 6) {

            mapToColumn = ((YUMRepoManager::filterColumn * 2) + 4);

        }// if
        else {

            mapToColumn = (YUMRepoManager::filterColumn * 2);

        }// else

        if(source_column == (mapToColumn - 1) || source_column == mapToColumn) {

            return true;

        }// if

        return false;

    }// if
    else {

        return QSortFilterProxyModel::filterAcceptsColumn(source_column, source_parent);

    }// else

}// filterAcceptsColumn

bool RepoDataProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const {

    if(YUMRepoManager::filterRow == "ShowEnables") {

        if((DataFlow::getRepoDataList().at(source_row).repoEnabled[1] == "1") || DataFlow::getRepoDataList().at(source_row).repoEnabled[1].isEmpty()) {

            return true;

        }// if

        return false;

    }// if
    else if(YUMRepoManager::filterRow == "ShowDisables") {

        if(DataFlow::getRepoDataList().at(source_row).repoEnabled[1] == "0") {

            return true;

        }// if

        return false;

    }// else if
    else {

        return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);

    }// else

}// filterAcceptsRow
