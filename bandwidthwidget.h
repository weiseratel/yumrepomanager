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

#ifndef BANDWIDTHWIDGET_H
#define BANDWIDTHWIDGET_H

#include "mywidget.h"

#include <QSpinBox>
#include <QComboBox>

class BandwidthWidget : public MyWidget
{

public:

    explicit BandwidthWidget(QWidget *parent = 0);

    inline const QString& spinBoxValue() const {

        return bandwidthAmount;

    }// amount

    inline const QString& comboBoxValue() const {

        return bandwidthThrottle;

    }// frequency

    inline const QString& spinBoxMinimumValue() const {

        return bandwidthSpinBoxMinimumValue;

    }// spinBoxMinimumValue

    inline const QString& spinBoxMaximumValue() const {

        return bandwidthSpinBoxMaximumValue;

    }// spinBoxMaximumValue

public slots:

    inline void setSpinBoxValue(const QString &amount) {

        bandwidthAmount = amount;

        spinBoxAmount->setValue(bandwidthAmount.toInt());

        emit spinBoxValueChanged(bandwidthAmount);

    }// setAmount

    void setComboBoxValue(const QString &throttle);

    inline void setSpinBoxMinimumValue(const QString &minimumValue) {

        bandwidthSpinBoxMinimumValue = minimumValue;

        spinBoxAmount->setMinimum(bandwidthSpinBoxMinimumValue.toInt());

    }// setSpinBoxAmountMinimumValue

    inline void setSpinBoxMaximumValue(const QString &maximumValue) {

        bandwidthSpinBoxMaximumValue = maximumValue;

        spinBoxAmount->setMaximum(bandwidthSpinBoxMaximumValue.toInt());

    }// setSpinBoxAmountMaximumValue

private:

    QSpinBox *spinBoxAmount;
    QComboBox *comboBoxBandwidth;

    QString bandwidthAmount;
    QString bandwidthThrottle;

    QString bandwidthSpinBoxMinimumValue;
    QString bandwidthSpinBoxMaximumValue;

};

#endif // BANDWIDTHWIDGET_H
