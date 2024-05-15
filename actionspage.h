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

#ifndef ACTIONSPAGE_H
#define ACTIONSPAGE_H

#include <QWizardPage>
#include <QPushButton>

class ActionsPage : public QWizardPage
{
    Q_OBJECT
public:
    explicit ActionsPage(QWidget *parent = 0);

    void initializePage();

    bool isComplete() const;

    void rpmFolderClick();
    void yumCacheClick();
    void yumGlobalCacheClick();



signals:

public slots:
    void disableButton();

private:

};

#endif // ACTIONSPAGE_H
