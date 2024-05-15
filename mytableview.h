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

#ifndef MYTABLEVIEW_H
#define MYTABLEVIEW_H

#include "yumrepomanager.h"

#include <QTableView>

class MyTableView : public QTableView
{

    Q_OBJECT

public:

    explicit MyTableView(QWidget *parent = 0);

protected:

    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    void keyPressEvent(QKeyEvent *event);

    void selectionChanged(const QItemSelection & selected, const QItemSelection & deselected);

private:

    void performDrag();

    QPoint startPos;

signals:

    void tableChanged();
    void deleteEventOcurred();
    void selectionChanged(YUMRepoManager::ToolBarActions action, bool state);

};

#endif // MYTABLEVIEW_H
