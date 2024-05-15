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

#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QDialog>
#include <QColorDialog>

namespace Ui {

    class Preferences;

}

class Preferences : public QDialog
{

    Q_OBJECT

public:

    explicit Preferences(QWidget *parent = 0);

    ~Preferences();

signals:

    void colorEnabledChanged(const QColor &color);
    void colorDisabledChanged(const QColor &color);

    void languageChangeOcurred(const QString& locale);

private slots:

    void on_pushButtonEnabledSelect_clicked();

    void on_pushButtonDisabledSelect_clicked();

    void setColorEnabled(const QColor &color);

    void setColorDisabled(const QColor &color);

    void on_pushButtonRestoreDefault_clicked();

    void on_checkBoxLanguage_clicked();

    void on_comboBoxLanguage_currentIndexChanged(const QString &language);

    void on_tabWidget_currentChanged(int index);

private:

    Ui::Preferences *ui;

    void changeEvent(QEvent *event);

    void restoreDefaultStatus();

    void setActiveFlag(const QString &activeFlag);

    enum { Enabled, Disabled };

    QColorDialog *colorDialog[2];

    QColor cellColor[2];
    QColor defaultCellColor[2];

};

#endif // PREFERENCES_H
