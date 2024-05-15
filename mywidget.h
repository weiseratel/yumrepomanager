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

#ifndef MYWIDGET_H
#define MYWIDGET_H

#include <QtWidgets/QWidget>

class MyWidget : public QWidget
{

    Q_OBJECT

public:
    explicit MyWidget(QWidget *parent = 0);

    virtual const QString& spinBoxValue() const = 0;

    virtual const QString& comboBoxValue() const = 0;

    virtual const QString& spinBoxMinimumValue() const = 0;

    virtual const QString& spinBoxMaximumValue() const = 0;

public slots:

    virtual void setSpinBoxValue(const QString &value) = 0;

    virtual void setComboBoxValue(const QString &value) = 0;

    virtual void setSpinBoxMinimumValue(const QString &minimumValue) = 0;

    virtual void setSpinBoxMaximumValue(const QString &maximumValue) = 0;

signals:

    void spinBoxValueChanged(const QString &value);
    void comboBoxValueChanged(const QString &value);

};

#endif // MYWIDGET_H
