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

#ifndef YUMPLUGIN_H
#define YUMPLUGIN_H

#include "texteditor.h"
#include "dataflow.h"

#include <QDialog>
#include <QList>

namespace Ui {

    class YUMPlugin;

}

class YUMPlugin : public QDialog
{

    Q_OBJECT

public:

    explicit YUMPlugin(QWidget *parent = 0);

    ~YUMPlugin();

private:

    Ui::YUMPlugin *ui;

    TextEditor *textEditor;

private slots:

    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

    inline void pluginStateChanged(int index, bool state) {

        DataFlow::setPluginState(index, state);

    }// pluginStateChanged

    void on_pushButtonEdit_clicked();
    void on_listView_doubleClicked(const QModelIndex &index);

};

#endif // YUMPLUGIN_H
