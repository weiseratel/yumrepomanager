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

#ifndef GLOBALOPTIONS_H
#define GLOBALOPTIONS_H

#include "texteditor.h"
#include "yumrepomanager.h"

#include <QDialog>
#include <QStringListModel>

namespace Ui {

    class GlobalOptions;

}

class GlobalOptions : public QDialog
{

    Q_OBJECT

public:

    explicit GlobalOptions(QWidget *parent = 0);

    ~GlobalOptions();

private slots:

    void protectedListModelChanged(const QModelIndex topLeft, const QModelIndex bottomRight);

    void on_pushButtonExcludeAdd_clicked();

    void on_pushButtonExcludeRemove_clicked();

    void on_checkBoxProxyEnable_stateChanged(int arg1);

    void on_buttonBoxOK_accepted();

    void on_toolButtonCacheDir_clicked();

    void on_toolButtonReposDir_clicked();

    void on_buttonBoxOK_rejected();

    void on_pushButtonAddProtected_clicked();

    void on_pushButtonRemoveProtected_clicked();

    void on_pushButtonEdit_clicked();

signals:

    void saveData(YUMRepoManager::DataBases database);

private:

    Ui::GlobalOptions *ui;

    QStringList excludeList;

    QStringListModel *excludeListModel;

    TextEditor *textEditor;

    int protectedListModelChanges;

};

#endif // GLOBALOPTIONS_H
