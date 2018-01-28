#include "sandboxitemmodel.h"

#include <QApplication>
#include <QStyle>
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QDirIterator>
#include <QFileIconProvider>

SandBoxItemModel::SandBoxItemModel(QObject *parent)
    :QStandardItemModel(parent)
{
    this->rootItem = this->invisibleRootItem();
    this->dirIcon = QApplication::style()->standardIcon(QStyle::SP_DirIcon);
    this->iconProvider = new QFileIconProvider();
}

SandBoxItemModel::~SandBoxItemModel()
{
    delete this->iconProvider;
}

void SandBoxItemModel::populateSandBoxes(const QStringList &names)
{
    QString name;
    QStandardItem* parent;
    foreach (name, names) {
        if(!name.isEmpty())
        {
            name.remove("\r");
            QFileInfo info(name);
            parent = new QStandardItem(dirIcon, info.fileName());
            parent->setEditable(false);
            parent->setAccessibleDescription(name);
            rootItem->appendRow(parent);
            createDirectoryItem(name, parent);
        }
        emit directoryLoaded(name);
    }
}

void SandBoxItemModel::createDirectoryItem(QString dirName, QStandardItem *parentItem)
{
    QDir dir(dirName);
    QFileInfo item;
    QStandardItem* child;
    QFileInfoList subFoldersAndFiles = dir.entryInfoList(this->filters, QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot);
    foreach (item, subFoldersAndFiles)
    {
        if(item.isFile())
        {
            child = new QStandardItem(iconProvider->icon(item), item.fileName());
            child->setEditable(false);
            child->setAccessibleDescription(item.filePath());
        }
        else
        {
            child = new QStandardItem(dirIcon, item.fileName());
            child->setEditable(false);
            child->setAccessibleDescription(item.filePath());
        }
        parentItem->appendRow(child);
        createDirectoryItem(item.filePath(), child);
    }
}

QFileInfo SandBoxItemModel::fileInfo(QModelIndex index)
{
    QStandardItem* item = this->itemFromIndex(index);
    return QFileInfo(item->accessibleDescription());
}

QString SandBoxItemModel::fileName(QModelIndex index)
{
    if (!index.isValid()) return "";

    QFileInfo info = this->fileInfo(index);
    return info.fileName();
}

QString SandBoxItemModel::filePath(QModelIndex index)
{
    if (!index.isValid()) return "";

    QFileInfo info = this->fileInfo(index);
    return info.filePath();
}

bool SandBoxItemModel::rmdir(QModelIndex index)
{
    if (!index.isValid()) return false;

    QFileInfo info = this->fileInfo(index);
    QStandardItem* item = this->itemFromIndex(index);
    if (info.isDir())
    {
        QDir dir(item->accessibleDescription());
        if (dir.removeRecursively())
        {
            item->parent()->removeRow(item->row());
            return true;
        }
    }
    return false;
}

bool SandBoxItemModel::remove(QModelIndex index)
{
    if (!index.isValid()) return false;

    QFileInfo info = this->fileInfo(index);
    QStandardItem* item = this->itemFromIndex(index);
    if (info.isFile())
    {
        if (info.dir().remove(info.fileName()))
        {
            item->parent()->removeRow(item->row());
            return true;
        }
    }
    return false;
}

bool SandBoxItemModel::rename(QModelIndex index, QString newName)
{
    if (!index.isValid()) return false;

    QFileInfo info = this->fileInfo(index);
    QStandardItem* item = this->itemFromIndex(index);
    QString oldName = this->fileName(index);
    if (info.dir().rename(oldName, newName))
    {
        item->setAccessibleDescription(info.dir().absoluteFilePath(newName));
        item->setText(newName);
        return true;
    }
    return false;
}

bool SandBoxItemModel::isDir(QModelIndex index)
{
    if (!index.isValid()) return false;

    QFileInfo info = this->fileInfo(index);
    return info.isDir();
}

bool SandBoxItemModel::isFile(QModelIndex index)
{
    if (!index.isValid()) return false;

    QFileInfo info = this->fileInfo(index);
    return info.isFile();
}

QModelIndex SandBoxItemModel::mkdir(QModelIndex index, QString name)
{
    if (!index.isValid()) return QModelIndex().parent();

    QFileInfo info = this->fileInfo(index);
    QStandardItem* parentItem = this->itemFromIndex(index);
    if (info.isDir())
    {
        QDir dir(parentItem->accessibleDescription());
        qDebug() << parentItem->accessibleDescription();
        if (dir.mkdir(name))
        {
            QStandardItem* child = new QStandardItem(this->dirIcon, name);
            child->setEditable(false);
            child->setAccessibleDescription(dir.absoluteFilePath(name));
            parentItem->appendRow(child);

            return this->indexFromItem(child);
        }
    }

    return QModelIndex().parent();
}

void SandBoxItemModel::setNameFilters(QStringList filters)
{
    this->filters = filters;
}

bool SandBoxItemModel::copy(QString from, QModelIndex to)
{
    if (from.isEmpty() || !to.isValid()) return false;

    QFileInfo info(from);
    QString fileName = info.fileName();
    QString fromPath = info.filePath();
    QString toPath = QDir(this->filePath(to)).absoluteFilePath(fileName);

    QFile file(fromPath);
    if (file.copy(toPath))
    {
        QStandardItem* toItem = this->itemFromIndex(to);

        QFileInfo info(toPath);

        QStandardItem* child = new QStandardItem(iconProvider->icon(info), fileName);
        child->setEditable(false);
        child->setAccessibleDescription(toPath);
        toItem->appendRow(child);
        return true;
    }

    return false;
}

bool SandBoxItemModel::copy(QModelIndex from, QModelIndex to)
{
    if (!from.isValid() || !to.isValid()) return false;

    QString fromPath = this->filePath(from);
    return this->copy(fromPath, to);
}

bool SandBoxItemModel::move(QModelIndex from, QModelIndex to)
{
    if (!from.isValid() || !to.isValid()) return false;

    QString fileName = this->fileName(from);
    QString fromPath = this->filePath(from);
    QString toPath = QDir(this->filePath(to)).absoluteFilePath(fileName);

    qDebug() << fileName;
    qDebug() << fromPath;
    qDebug() << toPath;

    QFile file(fromPath);
    if (file.rename(toPath))
    {
        QStandardItem* fromItem = this->itemFromIndex(from);
        QStandardItem* toItem = this->itemFromIndex(to);

        fromItem->parent()->removeRow(fromItem->row());

        QFileInfo info(toPath);

        QStandardItem* child = new QStandardItem(iconProvider->icon(info), fileName);
        child->setEditable(false);
        child->setAccessibleDescription(toPath);
        toItem->appendRow(child);

        return true;
    }

    return false;
}

void SandBoxItemModel::setEnabled(QModelIndex index, bool enabled)
{
    if (!index.isValid()) return;
    QStandardItem* item = this->itemFromIndex(index);
    item->setEnabled(enabled);
}
