#include "utils.h"
#include "defines.h"

#include <QDir>
#include <QDirIterator>
#include <QDebug>

QStringList scanDirItems(QString initialPath, QString extension, QString relativePath)
{
    QDir fullDir(initialPath);
    QDir dir(relativePath);
    QStringList files;
    QString fullPath = fullDir.absolutePath();
    QString path = dir.absolutePath();
    qDebug() << "Search in " << path << LOG_DATA;
    QDirIterator iterator(fullPath, QDirIterator::Subdirectories);
    while (iterator.hasNext()) {
        iterator.next();
        if (!iterator.fileInfo().isDir()) {
            QString filename = iterator.filePath();
            if (filename.endsWith(extension))
                files.append(filename.remove(path));
        }
    }
    return files;
}
