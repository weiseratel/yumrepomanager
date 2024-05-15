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

#ifndef CREATEREPOARGUMENTS_H
#define CREATEREPOARGUMENTS_H

#include <QDialog>
#include <QHash>

namespace Ui {

    class CreaterepoArguments;

}

class CreaterepoArguments : public QDialog
{

    Q_OBJECT
    
public:

    explicit CreaterepoArguments(QWidget *parent = 0);

    ~CreaterepoArguments();
    
private slots:

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    void on_toolButton_clicked();

    void on_pushButtonRestoreDefault_clicked();

    void on_checkBoxNoDatabase_clicked();

    void on_checkBoxCheckTimestamps_clicked();

    void on_checkBoxUpdate_clicked();

    void on_checkBoxIncludeGroups_clicked();

    void on_checkBoxDeltas_clicked();

    void on_checkBoxIgnoreSymlinks_clicked();

    void on_spinBoxWorkers_valueChanged(int arg1);

    void on_lineEditCacheDir_textChanged(const QString &arg1);

    void on_checkBoxXZCompression_clicked();

    void on_checkBoxPrettyFormat_clicked();

private:

    Ui::CreaterepoArguments *ui;

    void loadArguments();

    void restoreDefault();

};

#endif // CREATEREPOARGUMENTS_H
