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

#ifndef HELPERTHREAD_H
#define HELPERTHREAD_H

#include <QThread>
#include <QHash>

class HelperThread : public QThread
{

    Q_OBJECT

public:

    explicit HelperThread(const QString &mode, const QString &destination, QObject *parent = 0);
    explicit HelperThread(const QString &mode, QObject *parent = 0);

signals:

    void logsChanged(const QString &message);
    void changeTitleAndImage(const QString status);

private:

    void run();

    QString mode;
    QString destination;

};

#endif // HELPERTHREAD_H
