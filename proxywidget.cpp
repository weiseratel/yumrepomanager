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

#include "proxywidget.h"

#include <QHBoxLayout>

ProxyWidget::ProxyWidget(QWidget *parent) : QWidget(parent)
{

    QHBoxLayout *layout = new QHBoxLayout(this);

    ipAddress = new QLineEdit(this);
    separator = new QLabel(":", this);
    port = new QSpinBox(this);

    ipAddress->setMinimumSize(135, 23);
    ipAddress->setMaximumSize(135, 23);

    port->setMinimumSize(70, 23);
    port->setMaximumSize(70, 23);

    layout->addWidget(ipAddress);
    layout->addWidget(separator);
    layout->addWidget(port);

    connect(ipAddress, SIGNAL(textChanged(QString)), this, SLOT(setIpAddressText(QString)));
    connect(port, SIGNAL(valueChanged(QString)), this, SLOT(setPortValue(QString)));

}// Constructor
