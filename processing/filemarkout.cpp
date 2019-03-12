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

#define DEFAULT_RELATIVE 100.0

/*
 * Re-markout segment by relative limit of segment max value
 */
Points relativeAlignSement(Points points, vector data, int relativeLimit, double scaleFactor)
{
    int pointIndex = points.pointsCount - 1;
    qDebug() << "relativeAlignSement" << LOG_DATA;
    uint32_t startPosition = points.pointsOffset[pointIndex] / scaleFactor;
    uint32_t stopPosition = startPosition + points.pointsLenght[pointIndex] / scaleFactor;

    qDebug() << "startPosition: " << startPosition << LOG_DATA;
    qDebug() << "stopPosition: " << stopPosition << LOG_DATA;

    // Look for max value
    double maxValue = 0.0;
    for (int i = startPosition; i<=stopPosition; i++)
    {
        double value = getv(data, i);
        if (maxValue < value)
            maxValue = value;
    }
    qDebug() << "maxValue: " << maxValue << LOG_DATA;

    // Calculate new absolute limit
    double limit = maxValue * relativeLimit / 100.0;
    qDebug() << "limit: " << limit << LOG_DATA;

    // Recalculate start segments border
    uint32_t newSegmentOffset = -1;
    for (uint32_t i = startPosition; i<=stopPosition || newSegmentOffset == -1; i++)
    {
        double value = getv(data, i);
        if (value > limit)
        {
            newSegmentOffset = i;
            break;
        }
    }

    // Recalculate stop segments border
    uint32_t newSegmentLenght = -1;
    for (uint32_t i = stopPosition; i>=startPosition || newSegmentLenght == -1; i--)
    {
        double value = getv(data, i);
        if (value > limit)
        {
            newSegmentLenght = i - newSegmentOffset;
            break;
        }
    }

    qDebug() << "pointsOffset: " << points.pointsOffset[pointIndex] << LOG_DATA;
    qDebug() << "pointsLenght: " << points.pointsLenght[pointIndex] << LOG_DATA;
    points.pointsOffset[pointIndex] = newSegmentOffset * scaleFactor;
    points.pointsLenght[pointIndex] = newSegmentLenght * scaleFactor;
    qDebug() << "pointsOffset: " << points.pointsOffset[pointIndex] << LOG_DATA;
    qDebug() << "pointsLenght: " << points.pointsLenght[pointIndex] << LOG_DATA;

    return points;
}

/*
 * Get new relative limit of segment max value
 */
double getRelativeLimitForSement(uint32_t startPosition, uint32_t stopPosition, vector data, int relativeLimit)
{
    // Look for max value
    double maxValue = 0.0;
    double minValue = 1.0;
    for (int i = startPosition; i<=stopPosition; i++)
    {
        double value = getv(data, i);
        if (maxValue < value)
            maxValue = value;
        if (minValue > value)
            minValue = value;
    }
    qDebug() << "maxValue: " << maxValue << LOG_DATA;
    qDebug() << "minValue: " << minValue << LOG_DATA;

    // Calculate new absolute limit
    double limit = minValue + ((maxValue - minValue) * relativeLimit / 100.0);

    return limit;
}

Points getPoints(int dataSize, vector data, double scaleFactor, double limit, int relativeLimit = DEFAULT_RELATIVE)
{
    Points points;

    uint8_t gotIt = 0;
    uint32_t lenght = 0;

    double initialLimit = limit;

    int useRelative = 0;
    if (relativeLimit != DEFAULT_RELATIVE)
        useRelative = 1;
    int isRelative = 0;
    int offset = 0;
    uint32_t startRelativePosition = 0;
    uint32_t stopRelativePosition = 0;
    for (uint32_t i=0; i<dataSize; i++)
    {
        double value = getv(data, i);
        // Found segment end

        if (((value < limit ||  i==(dataSize-1)) && gotIt == 1)
                || (useRelative && isRelative && i > stopRelativePosition))
        {
            qDebug() << "found end: " << i << " relative " << isRelative << limit << LOG_DATA;

            // Add point if don't use relative or it's 2nd-gen segmant
            if ((!useRelative || isRelative) && gotIt == 1)
            {
                points.pointsCount++;
                points = addOffset(points, scaleFactor * offset);
                points = addLenght(points, scaleFactor * lenght);
                points = addLabel(points, points.pointsCount);
            }

            gotIt = 0;

            // Calculate relative limit for 1st-gen segment
            if (useRelative && !isRelative)
            {
                stopRelativePosition = i;
                isRelative = 1;
                limit = getRelativeLimitForSement(startRelativePosition, stopRelativePosition, data, relativeLimit);
                qDebug() << "limit relative: " << limit << LOG_DATA;
                i = startRelativePosition;
            }

            // Finish scan for 1st-gen segment, go find next one
            if (useRelative && isRelative && i > stopRelativePosition)
            {
                isRelative = 0;
                startRelativePosition = i;
                stopRelativePosition = i;
                limit = initialLimit;
                qDebug() << "limit original: " << limit << LOG_DATA;
            }
        }
        //  It's segment, go on
        if (value > limit && gotIt == 1)
        {
            lenght++;
        }
        // Found segment start
        if (value > limit && gotIt == 0)
        {
            qDebug() << "found start: " << i << " relative " << isRelative << limit << LOG_DATA;
            if (useRelative && !isRelative)
                startRelativePosition = i;
            gotIt = 1;
            lenght = 0;
            offset = i;
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

    Points points = getPoints(
                intensiveSize,
                data->d_intensive_norm,
                scaleFactor,
                intensiveLimit,
                sptk_settings->dp->relative_limit
    );

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

    Points intensivePoints = getPoints(
                intensiveSize,
                data->d_intensive_norm,
                intensiveScaleFactor,
                intensiveLimit,
                sptk_settings->dp->relative_limit
    );
    Points pitchLogPoints = getPoints(
                pitchLogSize,
                data->d_pitch_log,
                pitchLogScaleFactor,
                MASK_LIMIT
    );

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

WaveFile * markOutFileByA0Integral(SimpleGraphData *data)
{
    SPTK_SETTINGS * sptk_settings = SettingsDialog::getSPTKsettings();

    WaveFile * waveFile = data->file_data;
    qDebug() << "waveOpenHFile" << LOG_DATA;

    int size = littleEndianBytesToUInt32(waveFile->dataChunk->chunkDataSize);
    qDebug() << "file size " << size << LOG_DATA;

    vector intensive = data_get_intensive_norm(data);
    qDebug() << "intensive " << intensive.x << LOG_DATA;

    vector intensive_smooth = data_get_intensive_smooth(data);
    qDebug() << "intensive_smooth " << intensive_smooth.x << LOG_DATA;

    double scaleFactor = 1.0 * size / CHAR_BIT_RECORD / intensive.x;
    qDebug() << "scaleFactor " << scaleFactor << LOG_DATA;

    double intensiveAbsLimit = midv(intensive_smooth) * sptk_settings->dp->markoutA0IntA0abs / 100.0;
    qDebug() << "intensiveAbsLimit " << intensiveAbsLimit << LOG_DATA;

    Points points;

    uint8_t gotIt = 0;
    double intensive_value = 0.0;
    double smooth_value = 0.0;
    uint32_t startPosition = 0;
    uint32_t stopPosition = 0;
    for (uint32_t i=0; i<intensive.x; i++)
    {
        intensive_value = getv(intensive, i);
        smooth_value = getv(intensive_smooth, i);
        if (gotIt == 0 && intensive_value > smooth_value && intensive_value > intensiveAbsLimit)
        {
            gotIt = 1;
            startPosition = i;
        } else if (gotIt == 1 && (intensive_value <= smooth_value || intensive_value <= intensiveAbsLimit))
        {
            gotIt = 0;
            stopPosition = i;

            points.pointsCount++;
            points = addOffset(points, scaleFactor * startPosition);
            points = addLenght(points, scaleFactor * (stopPosition - startPosition));
            points = addLabel(points, points.pointsCount);
        }
    }

    if (gotIt == 1)
    {
        points.pointsCount++;
        points = addOffset(points, scaleFactor * startPosition);
        points = addLenght(points, scaleFactor * (intensive.x - 1 - startPosition));
        points = addLabel(points, points.pointsCount);
    }

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
