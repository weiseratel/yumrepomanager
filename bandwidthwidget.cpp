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

#include "bandwidthwidget.h"

#include <QHBoxLayout>

BandwidthWidget::BandwidthWidget(QWidget *parent) : MyWidget(parent)
{

    QHBoxLayout *layout = new QHBoxLayout(this);

    spinBoxAmount = new QSpinBox;
    comboBoxBandwidth = new QComboBox;

    spinBoxAmount->setMinimumSize(60, 23);
    spinBoxAmount->setMaximumSize(60, 23);

    comboBoxBandwidth->setMinimumSize(95, 23);
    comboBoxBandwidth->setMaximumSize(95, 23);

    comboBoxBandwidth->addItem("Gigabytes");
    comboBoxBandwidth->addItem("Megabytes");
    comboBoxBandwidth->addItem("Kilobytes");

    bandwidthThrottle = "Gigabytes";

    layout->addWidget(spinBoxAmount);
    layout->addWidget(comboBoxBandwidth);

    connect(spinBoxAmount, SIGNAL(valueChanged(QString)), this, SLOT(setSpinBoxValue(QString)));
    connect(comboBoxBandwidth, SIGNAL(activated(QString)), this, SLOT(setComboBoxValue(QString)));

}// Constructor

void BandwidthWidget::setComboBoxValue(const QString &value) {

    if(value == "Gigabytes") {

        bandwidthThrottle = "Gigabytes";

        comboBoxBandwidth->clear();

        comboBoxBandwidth->addItem("Gigabytes");
        comboBoxBandwidth->addItem("Megabytes");
        comboBoxBandwidth->addItem("Kilobytes");

    }// if
    else if(value == "Megabytes") {

        bandwidthThrottle = "Megabytes";

        comboBoxBandwidth->clear();

        comboBoxBandwidth->addItem("Megabytes");
        comboBoxBandwidth->addItem("Kilobytes");
        comboBoxBandwidth->addItem("Gigabytes");

    }// else if
    else if(value == "Kilobytes") {

        bandwidthThrottle = "Kilobytes";

        comboBoxBandwidth->clear();

        comboBoxBandwidth->addItem("Kilobytes");
        comboBoxBandwidth->addItem("Gigabytes");
        comboBoxBandwidth->addItem("Megabytes");

    }// else if

    emit comboBoxValueChanged(bandwidthThrottle);

}// setBandwidth
