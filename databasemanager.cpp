#include "databasemanager.h"
#include "defines.h"
#include "utils.h"

#include <QDebug>
#include <QApplication>

#include <QFileSystemModel>

DatabaseManager::DatabaseManager(QTreeView * view, QObject *parent) : QObject(parent)
{
    this->treeView = view;

    this->initDB();
}

void DatabaseManager::initDB()
{
    QString databasePath = QApplication::applicationDirPath() + DATA_PATH_TRAINING;

    QFileSystemModel *model = new QFileSystemModel;
    model->setRootPath(databasePath);
    model->setFilter( QDir::NoDotAndDotDot | QDir::Files | QDir::AllDirs );

    QStringList filters;
    filters << WAVE_TYPE_FILTER;

    model->setNameFilters(filters);
    model->setNameFilterDisables(false);


    this->treeView->setModel(model);
    this->treeView->setRootIndex(model->index(databasePath));
    this->treeView->hideColumn(1);
    this->treeView->hideColumn(2);
    this->treeView->hideColumn(3);

    connect(model, SIGNAL(directoryLoaded(QString)), this->treeView, SLOT(expandAll()));
}
