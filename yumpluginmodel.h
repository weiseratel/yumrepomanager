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

#ifndef YUMPLUGINMODEL_H
#define YUMPLUGINMODEL_H

#include <QAbstractTableModel>
#include <QColor>

class YUMPluginModel : public QAbstractTableModel
{

    Q_OBJECT

public:

    explicit YUMPluginModel(QObject *parent = 0);

    int rowCount(const QModelIndex &parent) const;

    int columnCount(const QModelIndex &parent) const;

    Qt::ItemFlags flags(const QModelIndex &index) const;

    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);

signals:

    void stateChanged(int index, bool state);

};

#endif // YUMPLUGINMODEL_H
