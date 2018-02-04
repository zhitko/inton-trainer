#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QModelIndex>

class QTreeView;
class QPoint;
class SandBoxItemModel;

class DatabaseManager : public QObject
{
    Q_OBJECT
public:
    explicit DatabaseManager(QTreeView * view, QObject *parent);

private:
    void initUI();
    void initDB();

    QTreeView * treeView;
    SandBoxItemModel * model;

    QString selectedFilePath;
    bool isSelectedDir;
    bool isRootDir;

    QModelIndex copyIndex;
    QModelIndex cutIndex;

signals:
    void activeFileChanged(QString);

public slots:
    void showContextMenu(const QPoint &pos);
    void fileSelected(const QModelIndex &selected);

    void actionOpen();
    void actionOpenWith();
    void actionFileDir();
    void actionEdit();
    void actionCopy();
    void actionCut();
    void actionPaste();
    void actionRename();
    void actionDelete();
    void actionMkdir();
    void actionAdd();
    void actionMarkOut();
};

#endif // DATABASEMANAGER_H
