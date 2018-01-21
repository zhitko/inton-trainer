#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QTreeView>

class DatabaseManager : public QObject
{
    Q_OBJECT
public:
    explicit DatabaseManager(QTreeView * view, QObject *parent);

private:
    void initDB();

    QTreeView * treeView;

signals:

public slots:
};

#endif // DATABASEMANAGER_H
