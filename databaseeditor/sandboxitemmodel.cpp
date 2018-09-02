#include "sandboxitemmodel.h"

#include <QApplication>
#include <QStyle>
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QDirIterator>
#include <QFileIconProvider>

#include "dataprocessing.h"
#include "settingsdialog.h"

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
//    qDebug() << "SandBoxItemModel::createDirectoryItem" << LOG_DATA;
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


WaveFile * markOutFileByF0(SimpleGraphData *data);
WaveFile * markOutFileByA0(SimpleGraphData *data);
WaveFile * markOutFileByF0A0(SimpleGraphData *data);

QModelIndex SandBoxItemModel::markOutFile(QModelIndex index, int mode)
{
    qDebug() << "SandBoxItemModel::markOutFile" << LOG_DATA;
    if (!index.isValid()) return QModelIndex().parent();

    QFileInfo info = this->fileInfo(index);
    QString fileName = info.fileName();
    QString filePath = info.filePath();

    qDebug() << "fileName " << fileName << LOG_DATA;
    qDebug() << "filePath " << filePath << LOG_DATA;

    SimpleGraphData *data = SimpleProcWave2Data(filePath, true);

    WaveFile * newWaveFile = NULL;
    QString newFileName;

    if (mode == MARKOUT_MODE_F0)
    {
        newWaveFile = markOutFileByF0(data);
        newFileName = MARKOUT_PREFIX_F0 + fileName;
    } else if (mode == MARKOUT_MODE_A0) {
        newWaveFile = markOutFileByA0(data);
        newFileName = MARKOUT_PREFIX_A0 + fileName;
    } else if (mode == MARKOUT_MODE_F0A0) {
        newWaveFile = markOutFileByF0A0(data);
        newFileName = MARKOUT_PREFIX_F0A0 + fileName;
    } else if (mode == MARKOUT_MODE_TEST) {
        newWaveFile = waveCloneFile(data->file_data);
        newFileName = "_TEST_" + fileName;
    }else if (mode == MARKOUT_MODE_TEST_SAVE) {
        newWaveFile = data->file_data;
        newFileName = "_SAVE_" + fileName;
    }

    QString newFilePath = info.dir().absoluteFilePath(newFileName);

    qDebug() << "newFileName " << newFileName << LOG_DATA;
    qDebug() << "newFilePath " << newFilePath << LOG_DATA;

    if (newWaveFile != NULL)
    {
        qDebug() << "makeWaveFileFromRawData " << LOG_DATA;
        saveWaveFile(newWaveFile, newFilePath.toLocal8Bit().data());
        qDebug() << "saveWaveFile " << LOG_DATA;
        if (mode != MARKOUT_MODE_TEST_SAVE)
            waveCloseFile(newWaveFile);
        qDebug() << "waveCloseFile newWaveFile" << LOG_DATA;

        QStandardItem* item = this->itemFromIndex(index);
        QStandardItem* newItem = this->createFileItem(QFileInfo(newFilePath));
        item->parent()->appendRow(newItem);
    } else {
        qDebug() << "ERROR makeWaveFileFromRawData is empty" << LOG_DATA;
    }

    freeSimpleGraphData(data);
    qDebug() << "freeSimpleGraphData" << LOG_DATA;

    return index;
}

struct Points {
    uint32_t pointsCount = 0;
    uint32_t *pointsOffset = NULL;
    uint32_t *pointsLenght = NULL;
    char **pointsLabels = NULL;
};

Points addLabel(Points points, int index = 1)
{
    if (points.pointsLabels == NULL)
    {
        points.pointsLabels = (char **) malloc(sizeof(char *)*points.pointsCount);
    } else {
        points.pointsLabels = (char **) realloc(points.pointsLabels, sizeof(char *) * points.pointsCount);
    }
    points.pointsLabels[points.pointsCount - 1] = (char *) malloc(sizeof(char)*3);
    points.pointsLabels[points.pointsCount - 1][0] = MARK_NUCLEUS;
    points.pointsLabels[points.pointsCount - 1][1] = index + '0';
    points.pointsLabels[points.pointsCount - 1][2] = 0;
    return points;
}

Points addOffset(Points points, uint32_t offset)
{
    if (points.pointsOffset == NULL)
    {
        points.pointsOffset = (uint32_t *) malloc(sizeof(uint32_t));
    } else {
        points.pointsOffset = (uint32_t *) realloc(points.pointsOffset, sizeof(uint32_t) * points.pointsCount);
    }
    points.pointsOffset[points.pointsCount - 1] = offset;

    return points;
}

Points addLenght(Points points, uint32_t lenght)
{
    if (points.pointsLenght == NULL)
    {
        points.pointsLenght = (uint32_t *) malloc(sizeof(uint32_t));
    } else {
        points.pointsLenght = (uint32_t *) realloc(points.pointsLenght, sizeof(uint32_t) * points.pointsCount);
    }
    points.pointsLenght[points.pointsCount - 1] = lenght;

    return points;
}

Points mergePoints(Points thisPoints, Points thatPoints)
{
    Points points;

    for (int i=0; i < thisPoints.pointsCount; i++)
    {
        for (int j=0; j < thatPoints.pointsCount; j++)
        {
            uint32_t thisPointStart = thisPoints.pointsOffset[i];
            uint32_t thisPointEnd = thisPoints.pointsOffset[i] + thisPoints.pointsLenght[i];
            uint32_t thatPointStart = thatPoints.pointsOffset[j];
            uint32_t thatPointEnd = thatPoints.pointsOffset[j] + thatPoints.pointsLenght[j];
            uint32_t newPointStart = 0;
            uint32_t newPointEnd = 0;
            uint32_t newPointLenght = 0;

            if (thisPointEnd > thatPointStart && thisPointStart < thatPointEnd)
            {
                if (thisPointStart < thatPointStart) newPointStart = thatPointStart;
                else newPointStart = thisPointStart;

                if (thisPointEnd < thatPointEnd) newPointEnd = thisPointEnd;
                else newPointEnd = thatPointEnd;

                newPointLenght = newPointEnd - newPointStart;

                if (newPointLenght > 0)
                {
                    points.pointsCount++;

                    points = addLabel(points, points.pointsCount);
                    points = addOffset(points, newPointStart);
                    points = addLenght(points, newPointLenght);
                }
            }
        }
    }

    return points;
}

Points getPoints(int dataSize, vector data, double scaleFactor, double limit)
{
    Points points;

    uint8_t cut = 0.3;

    uint8_t gotIt = 0;
    uint32_t lenght = 0;
    for (uint32_t i=0; i<dataSize; i++)
    {
        if ((data.v[i] < limit ||  i==(dataSize-1)) && gotIt == 1)
        {
            gotIt = 0;
            lenght *= scaleFactor;
            points = addLenght(points, lenght - cut*lenght);

            points.pointsOffset[points.pointsCount - 1] += cut*lenght;

            points = addLabel(points, points.pointsCount);
        }
        if (data.v[i] >= limit && gotIt == 1)
        {
            lenght++;
        }
        if (data.v[i] >= limit && gotIt == 0)
        {
            gotIt = 1;
            points.pointsCount++;
            lenght = 0;

            points = addOffset(points, i * scaleFactor);
        }
    }

    qDebug() << "pointsOffset size " << points.pointsCount << LOG_DATA;

    return points;
}

WaveFile * markOutFileByF0(SimpleGraphData *data)
{
    WaveFile * waveFile = data->file_data;
    qDebug() << "waveOpenHFile" << LOG_DATA;

    int size = littleEndianBytesToUInt32(waveFile->dataChunk->chunkDataSize);
    qDebug() << "file size " << size << LOG_DATA;

    int pitchLogSize = data->d_pitch_log.x;
    qDebug() << "pitch size " << pitchLogSize << LOG_DATA;

    double scaleFactor = 1.0 * size / CHAR_BIT_RECORD / pitchLogSize;

    Points points = getPoints(pitchLogSize, data->d_pitch_log, scaleFactor, MASK_LIMIT);

    int waveDataSize = littleEndianBytesToUInt32(waveFile->dataChunk->chunkDataSize);
    char* waveData = (char*) malloc(waveDataSize);
    memcpy(waveData, waveFile->dataChunk->waveformData, waveDataSize);

    qDebug() << "waveData " << waveData << LOG_DATA;
    qDebug() << "waveDataSize " << waveDataSize << LOG_DATA;
    qDebug() << "NUMBER_OF_CHANNELS " << NUMBER_OF_CHANNELS << LOG_DATA;
    qDebug() << "RECORD_FREQ " << RECORD_FREQ << LOG_DATA;
    qDebug() << "SIGNIFICANT_BITS_PER_SAMPLE " << SIGNIFICANT_BITS_PER_SAMPLE << LOG_DATA;
    qDebug() << "pointsCount " << points.pointsCount << LOG_DATA;
    qDebug() << "pointsOffset " << points.pointsOffset << LOG_DATA;
    qDebug() << "pointsLenght " << points.pointsLenght << LOG_DATA;
    qDebug() << "pointsLabel " << points.pointsLabels << LOG_DATA;

    return makeWaveFileFromRawData(
                waveData,
                waveDataSize,
                NUMBER_OF_CHANNELS,
                RECORD_FREQ,
                SIGNIFICANT_BITS_PER_SAMPLE,
                points.pointsCount,
                points.pointsOffset,
                points.pointsLenght,
                points.pointsLabels
    );
}

WaveFile * markOutFileByA0(SimpleGraphData *data)
{
    SPTK_SETTINGS * sptk_settings = SettingsDialog::getSPTKsettings();

    WaveFile * waveFile = data->file_data;
    qDebug() << "waveOpenHFile" << LOG_DATA;

    int size = littleEndianBytesToUInt32(waveFile->dataChunk->chunkDataSize);
    qDebug() << "file size " << size << LOG_DATA;

    int intensiveSize = data->d_intensive_norm.x;
    qDebug() << "intensive size " << intensiveSize << LOG_DATA;

    double scaleFactor = 1.0 * size / CHAR_BIT_RECORD / intensiveSize;
    qDebug() << "scaleFactor " << scaleFactor << LOG_DATA;

    double intensiveLimit = 1.0 * sptk_settings->dp->markoutA0limit / 100;
    qDebug() << "intensiveLimit " << intensiveLimit << LOG_DATA;

    Points points = getPoints(intensiveSize, data->d_intensive_norm, scaleFactor, intensiveLimit);

    int waveDataSize = littleEndianBytesToUInt32(waveFile->dataChunk->chunkDataSize);
    char* waveData = (char*) malloc(waveDataSize);
    memcpy(waveData, waveFile->dataChunk->waveformData, waveDataSize);

    qDebug() << "waveData " << waveData << LOG_DATA;
    qDebug() << "waveDataSize " << waveDataSize << LOG_DATA;
    qDebug() << "NUMBER_OF_CHANNELS " << NUMBER_OF_CHANNELS << LOG_DATA;
    qDebug() << "RECORD_FREQ " << RECORD_FREQ << LOG_DATA;
    qDebug() << "SIGNIFICANT_BITS_PER_SAMPLE " << SIGNIFICANT_BITS_PER_SAMPLE << LOG_DATA;
    qDebug() << "pointsCount " << points.pointsCount << LOG_DATA;
    qDebug() << "pointsOffset " << points.pointsOffset << LOG_DATA;
    qDebug() << "pointsLenght " << points.pointsLenght << LOG_DATA;
    qDebug() << "pointsLabel " << points.pointsLabels << LOG_DATA;

    return makeWaveFileFromRawData(
                waveData,
                waveDataSize,
                NUMBER_OF_CHANNELS,
                RECORD_FREQ,
                SIGNIFICANT_BITS_PER_SAMPLE,
                points.pointsCount,
                points.pointsOffset,
                points.pointsLenght,
                points.pointsLabels
    );
}

WaveFile * markOutFileByF0A0(SimpleGraphData *data)
{
    SPTK_SETTINGS * sptk_settings = SettingsDialog::getSPTKsettings();

    WaveFile * waveFile = data->file_data;
    qDebug() << "waveOpenHFile" << LOG_DATA;

    int size = littleEndianBytesToUInt32(waveFile->dataChunk->chunkDataSize);
    qDebug() << "file size " << size << LOG_DATA;

    int pitchLogSize = data->d_pitch_log.x;
    qDebug() << "pitch size " << pitchLogSize << LOG_DATA;

    int intensiveSize = data->d_intensive_norm.x;
    qDebug() << "intensive size " << intensiveSize << LOG_DATA;

    double intensiveScaleFactor = 1.0 * size / CHAR_BIT_RECORD / intensiveSize;
    qDebug() << "intensiveScaleFactor " << intensiveScaleFactor << LOG_DATA;

    double pitchLogScaleFactor = 1.0 * size / CHAR_BIT_RECORD / pitchLogSize;
    qDebug() << "pitchLogScaleFactor " << pitchLogScaleFactor << LOG_DATA;

    double intensiveLimit = 1.0 * sptk_settings->dp->markoutA0limit / 100;
    qDebug() << "intensive_limit " << intensiveLimit << LOG_DATA;

    Points intensivePoints = getPoints(intensiveSize, data->d_intensive_norm, intensiveScaleFactor, intensiveLimit);
    Points pitchLogPoints = getPoints(pitchLogSize, data->d_pitch_log, pitchLogScaleFactor, MASK_LIMIT);

    int waveDataSize = littleEndianBytesToUInt32(waveFile->dataChunk->chunkDataSize);
    char* waveData = (char*) malloc(waveDataSize);
    memcpy(waveData, waveFile->dataChunk->waveformData, waveDataSize);

    qDebug() << "waveData " << waveData << LOG_DATA;
    qDebug() << "waveDataSize " << waveDataSize << LOG_DATA;
    qDebug() << "NUMBER_OF_CHANNELS " << NUMBER_OF_CHANNELS << LOG_DATA;
    qDebug() << "RECORD_FREQ " << RECORD_FREQ << LOG_DATA;
    qDebug() << "SIGNIFICANT_BITS_PER_SAMPLE " << SIGNIFICANT_BITS_PER_SAMPLE << LOG_DATA;

    qDebug() << "intensivePointsCount " << intensivePoints.pointsCount << LOG_DATA;
    qDebug() << "intensivePointsOffset " << intensivePoints.pointsOffset << LOG_DATA;
    qDebug() << "intensivePointsLenght " << intensivePoints.pointsLenght << LOG_DATA;
    qDebug() << "intensivePointsLabel " << intensivePoints.pointsLabels << LOG_DATA;

    qDebug() << "pitchLogPointsCount " << pitchLogPoints.pointsCount << LOG_DATA;
    qDebug() << "pitchLogPointsOffset " << pitchLogPoints.pointsOffset << LOG_DATA;
    qDebug() << "pitchLogPointsLenght " << pitchLogPoints.pointsLenght << LOG_DATA;
    qDebug() << "pitchLogPointsLabel " << pitchLogPoints.pointsLabels << LOG_DATA;

    Points points = mergePoints(intensivePoints, pitchLogPoints);

    qDebug() << "pointsCount " << points.pointsCount << LOG_DATA;
    qDebug() << "pointsOffset " << points.pointsOffset << LOG_DATA;
    qDebug() << "pointsLenght " << points.pointsLenght << LOG_DATA;
    qDebug() << "pointsLabel " << points.pointsLabels << LOG_DATA;

    return makeWaveFileFromRawData(
                waveData,
                waveDataSize,
                NUMBER_OF_CHANNELS,
                RECORD_FREQ,
                SIGNIFICANT_BITS_PER_SAMPLE,
                points.pointsCount,
                points.pointsOffset,
                points.pointsLenght,
                points.pointsLabels
    );
}
