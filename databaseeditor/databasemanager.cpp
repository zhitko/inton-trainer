#include "databasemanager.h"
#include "defines.h"
#include "utils.h"

#include <QDebug>
#include <QApplication>
#include <QPoint>
#include <QMenu>
#include <QAction>
#include <QTreeView>
#include <QFile>
#include <QFileInfo>
#include <QModelIndex>
#include <QInputDialog>
#include <QMessageBox>
#include <QHeaderView>
#include <QDesktopServices>
#include <QUrl>
#include <QFileDialog>
#include "sandboxitemmodel.h"

DatabaseManager::DatabaseManager(QTreeView * view, QObject *parent) : QObject(parent)
{
    this->treeView = view;

    this->cutIndex = QModelIndex().parent();
    this->copyIndex = QModelIndex().parent();

    this->initUI();
    this->initDB();
}

void DatabaseManager::initUI()
{
    this->treeView->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this->treeView, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(showContextMenu(const QPoint &)));

}

void DatabaseManager::initDB()
{
    QStringList pathes;
    pathes << QApplication::applicationDirPath() + DATA_PATH_TRAINING;
    pathes << QApplication::applicationDirPath() + DATA_PATH_TEST;
    pathes << QApplication::applicationDirPath() + DATA_PATH_USER;

    QStringList filters;
    filters << WAVE_TYPE_FILTER;

    this->model = new SandBoxItemModel(this);
    this->model->setNameFilters(filters);
    this->model->populateSandBoxes(pathes);

    this->treeView->setModel(this->model);
    this->treeView->header()->hide();
    this->treeView->expandAll();

    connect(this->model, SIGNAL(directoryLoaded(QString)),
            this->treeView, SLOT(expandAll()));
    connect(this->treeView, SIGNAL(clicked(QModelIndex)),
            this, SLOT(fileSelected(QModelIndex)));
    connect(this->treeView, SIGNAL(doubleClicked(QModelIndex)),
            this, SLOT(actionOpen()));
}

void DatabaseManager::showContextMenu(const QPoint &pos)
{
    this->fileSelected(this->treeView->indexAt(pos));

    QMenu contextMenu(tr("Context menu"), this->treeView);

    QAction actionRename(tr("Rename"), this->treeView);
    QAction actionDelete(tr("Detele"), this->treeView);
    QAction actionAdd(tr("Add File"), this->treeView);
    QAction actionOpen(tr("Open"), this->treeView);
    QAction actionFileDir(tr("Open File Dir"), this->treeView);
    QAction actionOpenWith(tr("Open with ..."), this->treeView);
    QAction actionEdit(tr("Edit"), this->treeView);
    QAction actionCopy(tr("Copy File"), this->treeView);
    QAction actionCut(tr("Cut File"), this->treeView);
    QAction actionPaste(tr("Paste File"), this->treeView);
    QAction actionMkdir(tr("Make Dir"), this->treeView);
    QAction actionMarkOutByF0(tr("Mark Out File By F0"), this->treeView);
    QAction actionMarkOutByA0(tr("Mark Out File By A0"), this->treeView);
    QAction actionMarkOutByF0A0(tr("Mark Out File By F0&&A0"), this->treeView);

    if (this->isSelectedDir)
    {
        connect(&actionOpen, SIGNAL(triggered()), this, SLOT(actionOpen()));
        connect(&actionAdd, SIGNAL(triggered()), this, SLOT(actionAdd()));
        connect(&actionMkdir, SIGNAL(triggered()), this, SLOT(actionMkdir()));
        connect(&actionPaste, SIGNAL(triggered()), this, SLOT(actionPaste()));

        contextMenu.addAction(&actionOpen);
        contextMenu.addSeparator();
        contextMenu.addAction(&actionAdd);
        contextMenu.addAction(&actionPaste);
        contextMenu.addSeparator();
        contextMenu.addAction(&actionMkdir);

        if (!this->copyIndex.isValid() && !this->cutIndex.isValid())
        {
            actionPaste.setDisabled(true);
        }
    } else {
        connect(&actionOpen, SIGNAL(triggered()), this, SLOT(actionOpen()));
        connect(&actionFileDir, SIGNAL(triggered()), this, SLOT(actionFileDir()));
        connect(&actionOpenWith, SIGNAL(triggered()), this, SLOT(actionOpenWith()));
        connect(&actionEdit, SIGNAL(triggered()), this, SLOT(actionEdit()));
        connect(&actionCopy, SIGNAL(triggered()), this, SLOT(actionCopy()));
        connect(&actionCut, SIGNAL(triggered()), this, SLOT(actionCut()));
        connect(&actionMarkOutByF0, SIGNAL(triggered()), this, SLOT(actionMarkOutByF0()));
        connect(&actionMarkOutByA0, SIGNAL(triggered()), this, SLOT(actionMarkOutByA0()));
        connect(&actionMarkOutByF0A0, SIGNAL(triggered()), this, SLOT(actionMarkOutByF0A0()));

        contextMenu.addAction(&actionOpen);
        contextMenu.addAction(&actionFileDir);
//        contextMenu.addAction(&actionOpenWith);
//        contextMenu.addAction(&actionEdit);
        contextMenu.addAction(&actionMarkOutByF0);
        contextMenu.addAction(&actionMarkOutByA0);
        contextMenu.addAction(&actionMarkOutByF0A0);
        contextMenu.addSeparator();
        contextMenu.addAction(&actionCopy);
        contextMenu.addAction(&actionCut);
        contextMenu.addSeparator();
    }

    if (!this->isRootDir)
    {
        connect(&actionRename, SIGNAL(triggered()), this, SLOT(actionRename()));
        connect(&actionDelete, SIGNAL(triggered()), this, SLOT(actionDelete()));

        contextMenu.addAction(&actionRename);
        contextMenu.addAction(&actionDelete);
    }

    contextMenu.exec(this->treeView->mapToGlobal(pos));
}

void DatabaseManager::fileSelected(const QModelIndex &selected)
{
    QString path = QApplication::applicationDirPath() + DATA_PATH;

    this->selectedFilePath = this->model->filePath(selected);
    this->isSelectedDir = this->model->isDir(selected);
    this->isRootDir = !selected.parent().isValid();
    emit activeFileChanged(this->selectedFilePath);
}

void DatabaseManager::actionOpen()
{
    QModelIndex index = this->treeView->currentIndex();
    QDesktopServices::openUrl(QUrl::fromLocalFile(this->model->filePath(index)));
}

void DatabaseManager::actionFileDir()
{
    QModelIndex index = this->treeView->currentIndex().parent();
    if (this->model->isDir(index))
    {
        QDesktopServices::openUrl(QUrl::fromLocalFile(this->model->filePath(index)));
    }
}

void DatabaseManager::actionOpenWith()
{

}

void DatabaseManager::actionEdit()
{

}

void DatabaseManager::actionAdd()
{
    QModelIndex index = this->treeView->currentIndex();
    QString fileName = QFileDialog::getOpenFileName(this->treeView, tr("Open Wave File"), "", tr(WAVE_TYPE_FILTER));
    qDebug() << fileName;
    if (!fileName.isEmpty())
    {
        this->model->copy(fileName, index);
    }
}

void DatabaseManager::actionCopy()
{
    this->model->setEnabled(this->copyIndex);
    this->model->setEnabled(this->cutIndex);

    this->copyIndex = this->treeView->currentIndex();
    this->cutIndex = QModelIndex().parent();
    this->model->setEnabled(this->copyIndex, false);
}

void DatabaseManager::actionCut()
{
    this->model->setEnabled(this->copyIndex);
    this->model->setEnabled(this->cutIndex);

    this->cutIndex = this->treeView->currentIndex();
    this->copyIndex = QModelIndex().parent();
    this->model->setEnabled(this->cutIndex, false);
}

void DatabaseManager::actionPaste()
{
    QModelIndex index = this->treeView->currentIndex();

    if (this->copyIndex.isValid())
    {
        this->model->copy(this->copyIndex, index);
    } else if (this->cutIndex.isValid())
    {
        this->model->move(this->cutIndex, index);
    }

    this->model->setEnabled(this->copyIndex);
    this->model->setEnabled(this->cutIndex);
    this->copyIndex = QModelIndex().parent();
    this->cutIndex = QModelIndex().parent();
}

void DatabaseManager::actionRename()
{
    QModelIndex index = this->treeView->currentIndex();
    if ( !index.isValid() )
    {
        return;
    }

    QString name = QInputDialog::getText(
                this->treeView,
                tr("Rename"),
                tr("Name"),
                QLineEdit::Normal,
                model->fileName(index)
    );
    if ( !name.isEmpty() )
    {
        if ( model->isFile(index) && !name.endsWith(WAVE_TYPE) )
        {
            name += WAVE_TYPE;
        }
        if ( !model->rename(index, name) )
        {
            QMessageBox::information(this->treeView, tr("Rename failed..."), tr("Failed to rename"));
        }
    }
}

void DatabaseManager::actionDelete()
{
    QModelIndex index = this->treeView->currentIndex();
    if ( !index.isValid() ) {
        return;
    }

    bool ok;
    QFileInfo info = this->model->fileInfo(index);
    if ( QMessageBox::question(this->treeView, tr("Are you sure to delete this file?"), info.fileName()) == QMessageBox::Yes )
    {
        if ( info.isDir() ) {
            ok = this->model->rmdir(index);
        } else {
            ok = this->model->remove(index);
        }

        if ( !ok ) {
            QMessageBox::information(this->treeView, tr("Removed"), tr("Filed to remove %1").arg(model->fileName(index)));
        }
    }
}

void DatabaseManager::actionMkdir()
{
    QModelIndex index = this->treeView->currentIndex();
    if ( !index.isValid() ) {
        return;
    }

    QString name = QInputDialog::getText(this->treeView, tr("Create directory"), tr("Directory name"));
    if ( !name.isEmpty() ) {
        if ( !model->isDir(index) ) {
            index = model->parent(index);
        }
        if ( !model->mkdir(index, name).isValid() ) {
            QMessageBox::information(this->treeView, tr("Create directory failed..."), tr("Failed to create directory"));
        }
    }
}

void DatabaseManager::actionMarkOutByF0()
{
    QModelIndex index = this->treeView->currentIndex();
    if ( !index.isValid() ) {
        return;
    }

    model->markOutFile(index, MARKOUT_MODE_F0);
}

void DatabaseManager::actionMarkOutByA0()
{
    QModelIndex index = this->treeView->currentIndex();
    if ( !index.isValid() ) {
        return;
    }

    model->markOutFile(index, MARKOUT_MODE_A0);
}

void DatabaseManager::actionMarkOutByF0A0()
{
    QModelIndex index = this->treeView->currentIndex();
    if ( !index.isValid() ) {
        return;
    }

    model->markOutFile(index, MARKOUT_MODE_F0A0);
}
