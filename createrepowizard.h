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

#ifndef CREATEREPOWIZARD_H
#define CREATEREPOWIZARD_H

#include <QWizard>
#include <QProcess>
#include <QTextCursor>

namespace Ui {

    class CreateRepoWizard;

}

class CreateRepoWizard : public QWizard
{

    Q_OBJECT

public:
    explicit CreateRepoWizard(QWidget *parent = 0);

    ~CreateRepoWizard();

    static void rpmFolderProcess();

    static QString rpmFolder;
    static QString yumCache;
    static QString yumGlobalCache;

    static bool errorFlag;

    QTextCursor cursor;

    QMovie *progress;

public slots:

    void updateTitles(const QString &title, const QString &subTitle);
    void updateProgressMessage(const QString &message);
    void updateProgressImage(const QString &image);
    void updateLogs(const QString &message);
    void updateCursor();

private slots:

    void on_pushButtonFolder_clicked();

    void on_pushButtonCache_clicked();

    void on_pushButtonGlobalCache_clicked();

    void on_checkBoxAutomaticallyDeleteOldPackagesVersions_clicked(bool checked);

    void rejected();

private:
    Ui::CreateRepoWizard *ui;

    QString homePath;

};

#endif // CREATEREPOWIZARD_H
