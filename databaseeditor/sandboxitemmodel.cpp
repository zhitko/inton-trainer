#include "sandboxitemmodel.h"

#include <QApplication>
#include <QStyle>
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QDirIterator>
#include <QFileIconProvider>

#include "dataprocessing.h"

SandBoxItemModel::SandBoxItemModel(QObject *parent)
    :QStandardItemModel(parent)
{
    qDebug() << "SandBoxItemModel::SandBoxItemModel" << LOG_DATA;
    this->rootItem = this->invisibleRootItem();
    this->dirIcon = QApplication::style()->standardIcon(QStyle::SP_DirIcon);
    this->iconProvider = new QFileIconProvider();
}

SandBoxItemModel::~SandBoxItemModel()
{
    qDebug() << "SandBoxItemModel::~SandBoxItemModel" << LOG_DATA;
    delete this->iconProvider;
}

void SandBoxItemModel::populateSandBoxes(const QStringList &names)
{
    qDebug() << "SandBoxItemModel::populateSandBoxes" << LOG_DATA;
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
    qDebug() << "SandBoxItemModel::createDirectoryItem" << LOG_DATA;
    QDir dir(dirName);
    QFileInfo item;
    QStandardItem* child;
    QFileInfoList subFoldersAndFiles = dir.entryInfoList(this->filters, QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot);
    foreach (item, subFoldersAndFiles)
    {
        if(item.isFile())
        {
            child = this->createFileItem(item);
        }
        else
        {
            child = this->createDirItem(item);
        }
        parentItem->appendRow(child);
        createDirectoryItem(item.filePath(), child);
    }
}

QFileInfo SandBoxItemModel::fileInfo(QModelIndex index)
{
    qDebug() << "SandBoxItemModel::fileInfo" << LOG_DATA;
    QStandardItem* item = this->itemFromIndex(index);
    return QFileInfo(item->accessibleDescription());
}

QString SandBoxItemModel::fileName(QModelIndex index)
{
    qDebug() << "SandBoxItemModel::fileName" << LOG_DATA;
    if (!index.isValid()) return "";

    QFileInfo info = this->fileInfo(index);
    return info.fileName();
}

QString SandBoxItemModel::filePath(QModelIndex index)
{
    qDebug() << "SandBoxItemModel::filePath" << LOG_DATA;
    if (!index.isValid()) return "";

    QFileInfo info = this->fileInfo(index);
    return info.filePath();
}

bool SandBoxItemModel::rmdir(QModelIndex index)
{
    qDebug() << "SandBoxItemModel::rmdir" << LOG_DATA;
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
    qDebug() << "SandBoxItemModel::remove" << LOG_DATA;
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
    qDebug() << "SandBoxItemModel::rename" << LOG_DATA;
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
    qDebug() << "SandBoxItemModel::isDir" << LOG_DATA;
    if (!index.isValid()) return false;

    QFileInfo info = this->fileInfo(index);
    return info.isDir();
}

bool SandBoxItemModel::isFile(QModelIndex index)
{
    qDebug() << "SandBoxItemModel::isFile" << LOG_DATA;
    if (!index.isValid()) return false;

    QFileInfo info = this->fileInfo(index);
    return info.isFile();
}

QModelIndex SandBoxItemModel::mkdir(QModelIndex index, QString name)
{
    qDebug() << "SandBoxItemModel::mkdir" << LOG_DATA;
    if (!index.isValid()) return QModelIndex().parent();

    QFileInfo info = this->fileInfo(index);
    QStandardItem* parentItem = this->itemFromIndex(index);
    if (info.isDir())
    {
        QDir dir(parentItem->accessibleDescription());
        qDebug() << parentItem->accessibleDescription();
        if (dir.mkdir(name))
        {
            QStandardItem* child = this->createDirItem(QDir(dir.absoluteFilePath(name)));
            parentItem->appendRow(child);

            return this->indexFromItem(child);
        }
    }

    return QModelIndex().parent();
}

void SandBoxItemModel::setNameFilters(QStringList filters)
{
    qDebug() << "SandBoxItemModel::setNameFilters" << LOG_DATA;
    this->filters = filters;
}

bool SandBoxItemModel::copy(QString from, QModelIndex to)
{
    qDebug() << "SandBoxItemModel::copy" << LOG_DATA;
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

        QStandardItem* child = this->createFileItem(info);
        toItem->appendRow(child);
        return true;
    }

    return false;
}

bool SandBoxItemModel::copy(QModelIndex from, QModelIndex to)
{
    qDebug() << "SandBoxItemModel::copy" << LOG_DATA;
    if (!from.isValid() || !to.isValid()) return false;

    QString fromPath = this->filePath(from);
    return this->copy(fromPath, to);
}

bool SandBoxItemModel::move(QModelIndex from, QModelIndex to)
{
    qDebug() << "SandBoxItemModel::move" << LOG_DATA;
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

        QStandardItem* child = this->createFileItem(info);
        toItem->appendRow(child);

        return true;
    }

    return false;
}

void SandBoxItemModel::setEnabled(QModelIndex index, bool enabled)
{
    qDebug() << "SandBoxItemModel::setEnabled" << LOG_DATA;
    if (!index.isValid()) return;
    QStandardItem* item = this->itemFromIndex(index);
    item->setEnabled(enabled);
}

QModelIndex SandBoxItemModel::markOutFile(QModelIndex index)
{
    qDebug() << "SandBoxItemModel::markOutFile" << LOG_DATA;
    if (!index.isValid()) return QModelIndex().parent();

    QFileInfo info = this->fileInfo(index);
    QString fileName = info.fileName();
    QString filePath = info.filePath();
    QString newFileName = "+" + fileName;
    QString newFilePath = info.dir().absoluteFilePath(newFileName);

    qDebug() << "fileName " << fileName << LOG_DATA;
    qDebug() << "filePath " << filePath << LOG_DATA;
    qDebug() << "newFileName " << newFileName << LOG_DATA;
    qDebug() << "newFilePath " << newFilePath << LOG_DATA;

    SimpleGraphData *data = SimpleProcWave2Data(filePath, true);

    WaveFile * waveFile = data->file_data;
    qDebug() << "waveOpenHFile" << LOG_DATA;

    int size = littleEndianBytesToUInt32(waveFile->dataChunk->chunkDataSize);
    qDebug() << "file size " << size << LOG_DATA;

    int pitchLogSize = data->d_pitch_log.x;
    qDebug() << "pitch size " << pitchLogSize << LOG_DATA;

    double scaleFactor = 1.0 * size / CHAR_BIT_RECORD / pitchLogSize;

    uint32_t pointsCount = 0;
    uint32_t *pointsOffset = NULL;
    uint32_t *pointsLenght = NULL;
    char **pointsLabels = NULL;

    // TODO: process WAV file and add marks
    uint8_t limit = 0.1;
    uint8_t cut = 0.3;

    uint8_t gotIt = 0;
    uint32_t lenght = 0;
    for (uint32_t i=0; i<pitchLogSize; i++)
    {
        if (data->d_pitch_log.v[i] < limit && gotIt == 1)
        {
            gotIt = 0;
            if (pointsLenght == NULL)
            {
                pointsLenght = (uint32_t *) malloc(sizeof(uint32_t));
            } else {
                pointsLenght = (uint32_t *) realloc(pointsLenght, sizeof(uint32_t) * pointsCount);
            }
            lenght *= scaleFactor;
            pointsLenght[pointsCount - 1] = lenght - cut*lenght;
            pointsOffset[pointsCount - 1] += cut*lenght;

            if (pointsLabels == NULL)
            {
                pointsLabels = (char **) malloc(sizeof(char *)*pointsCount);
            } else {
                pointsLabels = (char **) realloc(pointsLabels, sizeof(char *) * pointsCount);
            }
            pointsLabels[pointsCount - 1] = (char *) malloc(sizeof(char)*2);
            pointsLabels[pointsCount - 1][0] = MARK_NUCLEUS;
            pointsLabels[pointsCount - 1][1] = 0;
        }
        if (data->d_pitch_log.v[i] >= limit && gotIt == 1)
        {
            lenght++;
        }
        if (data->d_pitch_log.v[i] >= limit && gotIt == 0)
        {
            gotIt = 1;
            pointsCount++;
            lenght = 0;
            if (pointsOffset == NULL)
            {
                pointsOffset = (uint32_t *) malloc(sizeof(uint32_t));
            } else {
                pointsOffset = (uint32_t *) realloc(pointsOffset, sizeof(uint32_t) * pointsCount);
            }
            pointsOffset[pointsCount - 1] = i * scaleFactor;
        }
    }

    int waveDataSize = littleEndianBytesToUInt32(waveFile->dataChunk->chunkDataSize);
    char* waveData = (char*) malloc(waveDataSize);
    memcpy(waveData, waveFile->dataChunk->waveformData, waveDataSize);

    if (0)
    {
        pointsCount = 3;
        pointsOffset = (uint32_t *) malloc(sizeof(uint32_t)*pointsCount);
        pointsOffset[0] = 1968;
        pointsOffset[1] = 2160;
        pointsOffset[2] = 3552;

        pointsLenght = (uint32_t *) malloc(sizeof(uint32_t)*pointsCount);
        pointsLenght[0] = 152;
        pointsLenght[1] = 1280;
        pointsLenght[2] = 472;

        pointsLabels = (char **) malloc(sizeof(char *)*pointsCount);
        pointsLabels[0] = (char *) malloc(sizeof(char)*2);
        pointsLabels[1] = (char *) malloc(sizeof(char)*2);
        pointsLabels[2] = (char *) malloc(sizeof(char)*2);
        pointsLabels[0][0] = 'P';
        pointsLabels[1][0] = 'N';
        pointsLabels[2][0] = 'T';
        pointsLabels[0][1] = 0;
        pointsLabels[1][1] = 0;
        pointsLabels[2][1] = 0;
    }

    WaveFile * newWaveFile = makeWaveFileFromRawData(
                waveData,
                waveDataSize,
                NUMBER_OF_CHANNELS,
                RECORD_FREQ,
                SIGNIFICANT_BITS_PER_SAMPLE,
                pointsCount,
                pointsOffset,
                pointsLenght,
                pointsLabels
    );
    saveWaveFile(newWaveFile, newFilePath.toLocal8Bit().data());

    freeSimpleGraphData(data);
    qDebug() << "freeSimpleGraphData" << LOG_DATA;
    waveCloseFile(newWaveFile);
    qDebug() << "waveCloseFile newWaveFile" << LOG_DATA;

    QStandardItem* item = this->itemFromIndex(index);
    QStandardItem* newItem = this->createFileItem(QFileInfo(newFilePath));
    item->parent()->appendRow(newItem);

    return index;
}

QStandardItem * SandBoxItemModel::createFileItem(QFileInfo info)
{
    QStandardItem* item = new QStandardItem(iconProvider->icon(info), info.fileName());
    item->setEditable(false);
    item->setAccessibleDescription(info.filePath());

    return item;
}

QStandardItem * SandBoxItemModel::createDirItem(QFileInfo info)
{
    QStandardItem* item = new QStandardItem(this->dirIcon, info.fileName());
    item->setEditable(false);
    item->setAccessibleDescription(info.filePath());

    return item;
}

QStandardItem * SandBoxItemModel::createDirItem(QDir dir)
{
    QStandardItem* item = new QStandardItem(this->dirIcon, dir.dirName());
    item->setEditable(false);
    item->setAccessibleDescription(dir.absolutePath());

    return item;
}
