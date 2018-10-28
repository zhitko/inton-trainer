#include "filemarkout.h"

#include <QDebug>

#include "settingsdialog.h"

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
