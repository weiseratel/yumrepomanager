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

#ifndef METADATAEXPIREWIDGET_H
#define METADATAEXPIREWIDGET_H

#include "mywidget.h"

#include <QSpinBox>
#include <QComboBox>

class MetadataExpireWidget : public MyWidget
{

    Q_OBJECT

public:

    explicit MetadataExpireWidget(QWidget *parent = 0);

    inline const QString& spinBoxValue() const {

        return metadataExpireAmount;

    }// amount

    inline const QString& comboBoxValue() const {

        return metadataExpireFrequency;

    }// frequency

    inline const QString& spinBoxMinimumValue() const {

        return metadataExpireSpinBoxMinimumValue;

    }// spinBoxMinimumValue

    inline const QString& spinBoxMaximumValue() const {

        return metadataExpireSpinBoxMaximumValue;

    }// spinBoxMaximumValue

public slots:

    inline void setSpinBoxValue(const QString &value) {

        metadataExpireAmount = value;

        spinBoxAmount->setValue(metadataExpireAmount.toInt());

        emit spinBoxValueChanged(metadataExpireAmount);

    }// setAmount

    void setComboBoxValue(const QString &value);

    inline void setSpinBoxMinimumValue(const QString &minimumValue) {

        metadataExpireSpinBoxMinimumValue = minimumValue;

        spinBoxAmount->setMinimum(metadataExpireSpinBoxMinimumValue.toInt());

    }// setSpinBoxAmountMinimumValue

    inline void setSpinBoxMaximumValue(const QString &maximumValue) {

        metadataExpireSpinBoxMaximumValue = maximumValue;

        spinBoxAmount->setMaximum(metadataExpireSpinBoxMaximumValue.toInt());

    }// setSpinBoxAmountMaximumValue

private:

    QSpinBox *spinBoxAmount;
    QComboBox *comboBoxFrequency;

    QString metadataExpireAmount;
    QString metadataExpireFrequency;

    QString metadataExpireSpinBoxMinimumValue;
    QString metadataExpireSpinBoxMaximumValue;

};

#endif // METADATAEXPIREWIDGET_H
