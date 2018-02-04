#ifndef SANDBOXITEMMODEL_H
#define SANDBOXITEMMODEL_H

#include <QObject>

#include <QStandardItemModel>
#include <QStandardItem>

class QFileInfo;
class QDir;
class QModelIndex;
class QFileIconProvider;

class SandBoxItemModel : public QStandardItemModel
{
    Q_OBJECT

public:
    explicit SandBoxItemModel(QObject* parent = 0);
    ~SandBoxItemModel();

    void populateSandBoxes(const QStringList &names);
    void createDirectoryItem(QString dirName, QStandardItem *parentItem = NULL);

    QFileInfo fileInfo(QModelIndex index);
    QString fileName(QModelIndex index);
    QString filePath(QModelIndex index);
    bool rmdir(QModelIndex index);
    bool remove(QModelIndex index);
    bool rename(QModelIndex index, QString newName);
    bool isDir(QModelIndex index);
    bool isFile(QModelIndex index);
    QModelIndex mkdir(QModelIndex index, QString name);
    void setNameFilters(QStringList filters);
    bool copy(QModelIndex from, QModelIndex to);
    bool copy(QString from, QModelIndex to);
    bool move(QModelIndex from, QModelIndex to);

    void setEnabled(QModelIndex index, bool disable = true);

    QModelIndex markOutFile(QModelIndex index);

private:
    QStandardItem *rootItem;
    QIcon dirIcon;
    QFileIconProvider * iconProvider;

    QStringList filters;

    QStandardItem * createFileItem(QFileInfo);
    QStandardItem * createDirItem(QFileInfo);
    QStandardItem * createDirItem(QDir);

signals:
    void directoryLoaded(QString dirName);

public slots:

};

#endif // SANDBOXITEMMODEL_H
