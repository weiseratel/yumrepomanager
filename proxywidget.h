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

#ifndef PROXYWIDGET_H
#define PROXYWIDGET_H

#include <QWidget>
#include <QComboBox>
#include <QLineEdit>
#include <QLabel>
#include <QSpinBox>

class ProxyWidget : public QWidget
{

    Q_OBJECT

public:

    explicit ProxyWidget(QWidget *parent = 0);

    inline const QString& ipAddressText() const {

        return proxyIpAddressText;

    }// ipAddressText

    inline const QString& portValue() const {

        return proxyPortValue;

    }// portValue

    inline const QString& portMinimumValue() const {

        return proxyPortMinimumValue;

    }// portMinimumValue

    inline const QString& portMaximumValue() const {

        return proxyPortMaximumValue;

    }// portMaximumValue

public slots:

    inline void setIpAddressText(const QString &text) {

        proxyIpAddressText = text;

        ipAddress->setText(proxyIpAddressText);

        emit ipAddressTextChanged(proxyIpAddressText);

    }// setIpAddressText

    inline void setPortValue(const QString &value) {

        proxyPortValue = value;

        port->setValue(proxyPortValue.toInt());

        emit portValueChanged(proxyPortValue);

    }// setPortValue

    inline void setPortMinimumValue(const QString &minimumValue) {

        proxyPortMinimumValue = minimumValue;

        port->setMinimum(proxyPortMinimumValue.toInt());

    }// setPortMinimumValue

    inline void setPortMaximumValue(const QString &maximumValue) {

        proxyPortMaximumValue = maximumValue;

        port->setMaximum(proxyPortMaximumValue.toInt());

    }// setPortMaximumValue

signals:

    void ipAddressTextChanged(const QString &text);
    void portValueChanged(const QString &value);

private:

    QLineEdit *ipAddress;
    QLabel *separator;
    QSpinBox *port;

    QString proxyIpAddressText;
    QString proxyPortValue;

    QString proxyPortMinimumValue;
    QString proxyPortMaximumValue;

};

#endif // PROXYWIDGET_H
