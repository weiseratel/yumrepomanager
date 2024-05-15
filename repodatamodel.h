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

#ifndef REPODATAMODEL_H
#define REPODATAMODEL_H

#include <QAbstractTableModel>
#include <QStringList>
#include <QMap>
#include <QColor>

class RepoDataModel : public QAbstractTableModel
{

    Q_OBJECT

public:

    explicit RepoDataModel(QObject *parent = 0);

    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;

    Qt::ItemFlags flags(const QModelIndex &index) const;

    QVariant data(const QModelIndex &index, int role) const;

    bool setData(const QModelIndex &index, const QVariant &value, int role);

    bool insertRow(int row, const QModelIndex &parent);

    bool removeRow(int row, const QModelIndex &parent);

    bool event(QEvent *event);

    void initDataState();

    const QColor& colorEnabled();
    const QColor& colorDisabled();

    QStringList header;

public slots:

    void updateModel();

    void setColorEnabled(const QColor &color);
    void setColorDisabled(const QColor &color);

private:

    enum { Enabled, Disabled };

    QColor cellColor[2];

    bool firstTime;

    QList<bool> enabledState;
    QList<bool> enabledProxy;
    QList<bool> enabledProxyUsername;
    QList<bool> enabledProxyPassword;
    QList<bool> enabledBaseURL;
    QList<bool> enabledMetaLink;
    QList<bool> enabledBandwidth;
    QList<bool> enabledInclude;
    QList<bool> enabledExclude;
    QList<bool> enabledSkipIfUnavailable;
    QList<bool> enabledFailoverMethod;
    QList<bool> enabledMetadataExpire;
    QList<bool> enabledGPGCheck;
    QList<bool> enabledGPGKey;
    QList<bool> enabledParallelDownload;

signals:

    void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);

    void setSaveButtonStatus(bool status);

};

#endif // REPODATAMODEL_H
