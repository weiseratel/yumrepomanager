/*
    YUMRepoManager, a manager for the configuration files of YUM and his repositories.
    Copyright (C) 2011-2014  Josué V. Herrera

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

#include "mytableview.h"
#include "dataflow.h"

#include <QApplication>
#include <QDrag>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QUrl>
#include <QDebug>

MyTableView::MyTableView(QWidget *parent) : QTableView(parent) {

}// Constructor

void MyTableView::dragEnterEvent(QDragEnterEvent *event)
{

    if(event->mimeData()->hasFormat("text/uri-list")) {

        event->acceptProposedAction();

    }// if
    else {

        event->ignore();

    }// else

}// dragEnterEvent

void MyTableView::dragMoveEvent(QDragMoveEvent *event) {

    event->acceptProposedAction();

}// dragMoveEvent

void MyTableView::dropEvent(QDropEvent *event) {

    Q_UNUSED(event);

    if(event->mimeData()->hasUrls()) {

        event->acceptProposedAction();

        const QMimeData *mimeData = event->mimeData();

        int size = mimeData->urls().size();

        QStringList absolutePath;

        for(int count = 0; count < size; count++) {

            QString repoFile = mimeData->urls().at(count).toLocalFile();

            if(!repoFile.contains(QRegExp("^*.repo$"))) {

                YUMRepoManager::showMessage("Error", tr("File not allowed: %1").arg(repoFile));

                continue;

            }// if

            absolutePath.append(repoFile);

        }// for

        DataFlow dataFlow;

        dataFlow.fillRepoDataList(absolutePath);

        emit tableChanged();

        event->acceptProposedAction();

    }// if
    else {

        event->ignore();

    }// else

}// dropEvent

void MyTableView::mousePressEvent(QMouseEvent *event) {

    if ((event->button() == Qt::LeftButton)) {

        startPos = event->pos();

    }// if

    QTableView::mousePressEvent(event);

}// mousePressEvent

void MyTableView::mouseMoveEvent(QMouseEvent *event) {

    if(!selectedIndexes().isEmpty()) {

        if (event->buttons() & Qt::LeftButton) {

            int distance = (event->pos() - startPos).manhattanLength();

            if (distance >= QApplication::startDragDistance()) {

                performDrag();

            }// if

        }// if

        QTableView::mouseMoveEvent(event);

    }// if

}// mouseMoveEvent

void MyTableView::performDrag() {

    QList<QUrl> urlList;

    int rowCount = ( selectedIndexes().size() / (YUMRepoManager::repoDataModel->columnCount(QModelIndex())) );

    int listIndex = 0;
    bool flag = false;

    for(int count = 0; count < rowCount; ++count) {

        QModelIndex index = selectedIndexes().at(listIndex);

        QString targetFile = DataFlow::getRepoDataList().at(YUMRepoManager::proxyModel->mapToSource(index).row()).repoFileName;

        targetFile.insert(0, "file://");

        QUrl myUrl(targetFile);

        foreach(QUrl url, urlList) {

            if(url.toString() == myUrl.toString()) {

                listIndex += 29;

                flag = true;

                break;

            }// if

        }// foreach

        if(flag) {

            flag = false;

            continue;

        }// if

        urlList.append(myUrl);

        listIndex += 29;

    }// for

    QMimeData *mimeData = new QMimeData;

    mimeData->setUrls(urlList);

    QDrag *drag = new QDrag(this);

    drag->setMimeData(mimeData);

    drag->exec(Qt::CopyAction);

}// performDrag

void MyTableView::keyPressEvent(QKeyEvent *event) {

    if(event->key() == Qt::Key_Delete) {

        emit deleteEventOcurred();

    }// if

    QTableView::keyPressEvent(event);

}// keyPressEvent

void MyTableView::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected) {

    if(!selected.isEmpty()) {

        emit selectionChanged(YUMRepoManager::ActionRemove, true);

    }// if
    else {

        emit selectionChanged(YUMRepoManager::ActionRemove, false);

    }// else

    QTableView::selectionChanged(selected, deselected);

}// void
