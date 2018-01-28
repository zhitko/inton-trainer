#ifndef UTILS_H
#define UTILS_H

#include <QStringList>

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

QStringList scanDirItems(QString path, QString extension, QString relativePath);

#endif // UTILS_H
