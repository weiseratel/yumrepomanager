#ifndef EDITREPO_H
#define EDITREPO_H

#include <QDialog>
#include <QStringListModel>
#include <QDataWidgetMapper>

#include "repodata.h"
#include "repodatamodel.h"

namespace Ui {
    class EditRepo;
}

class EditRepo : public QDialog
{
    Q_OBJECT

public:
    explicit EditRepo(QWidget *parent = 0);
    explicit EditRepo(RepoDataModel *repoDataModel, int nodeIndex, QWidget *parent = 0);
    ~EditRepo();

private slots:


private:
    Ui::EditRepo *ui;

    QDataWidgetMapper *dataMapper;
/*
    bool nameState, commentState, proxyState, pathState;
    QString repoName, repoComment, repoProxy[2], repoBaseURL[2], repoMirrorList[2];

    int nodeIndex;

    RepoData Node;

    QString repoAddressUrl;
    QString repoAddressMirror;

    QStringListModel *repoAddressModel;

    QStringListModel *listModel;

    QStringList includeList;
    QStringList excludeList;
*/
};

#endif // EDITREPO_H
