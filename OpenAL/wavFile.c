#include "wavFile.h"

WaveFile * initWaveFile()
{
    WaveFile * waveFile = (WaveFile *) malloc(sizeof(WaveFile));

    waveFile->file = NULL;
    waveFile->formatChunk = NULL;
    waveFile->waveHeader = NULL;
    waveFile->dataChunk = NULL;
    waveFile->cueChunk = NULL;
    waveFile->filePath = NULL;

    return waveFile;
}

WaveFile * waveOpenFile(const char* path)
{
    ChunkLocation formatChunkExtraBytes = {0,0};
    ChunkLocation dataChunkLocation = {0,0};
    ChunkLocation otherChunkLocation = {0,0};

    WaveFile * waveFile = initWaveFile();
    if(waveFile == NULL)
    {
        fprintf(stderr, "Memory Allocation Error: Could not allocate memory for Wave File\n");
        return 0;
    }

    waveFile->filePath = (char *)calloc(strlen(path)+1, sizeof(char));
    strncpy(waveFile->filePath, path, strlen(path));

    waveFile->file = fopen(waveFile->filePath, "rb");
    if(waveFile->file == NULL)
    {
        fprintf(stderr, "Could not open input file %s\n", waveFile->filePath);
        waveCloseFile(waveFile);
        return 0;
    }

    waveFile->waveHeader = (WaveHeader *) malloc(sizeof(WaveHeader));
    if(waveFile->waveHeader == NULL)
    {
        fprintf(stderr, "Memory Allocation Error: Could not allocate memory for Wave File Header\n");
        waveCloseFile(waveFile);
        return 0;
    }
    fread(waveFile->waveHeader, sizeof(WaveHeader), 1, waveFile->file);
    if (ferror(waveFile->file) != 0)
    {
        fprintf(stderr, "Error reading input file %s\n", waveFile->filePath);
        waveCloseFile(waveFile);
        return 0;
    }

    if (strncmp(&(waveFile->waveHeader->chunkID[0]), "RIFF", 4) != 0)
    {
        fprintf(stderr, "Input file is not a RIFF file\n");
        waveCloseFile(waveFile);
        return 0;
    }

    if (strncmp(&(waveFile->waveHeader->riffType[0]), "WAVE", 4) != 0)
    {
        fprintf(stderr, "Input file is not a WAVE file\n");
        waveCloseFile(waveFile);
        return 0;
    }

    uint32_t remainingFileSize = littleEndianBytesToUInt32(waveFile->waveHeader->dataSize) - sizeof(waveFile->waveHeader->riffType); // dataSize does not counf the chunkID or the dataSize, so remove the riffType size to get the length of the rest of the file.

    if (remainingFileSize <= 0)
    {
        fprintf(stderr, "Input file is an empty WAVE file\n");
        waveCloseFile(waveFile);
        return 0;
    }

    while (1)
    {
        char nextChunkID[4];

        fread(&nextChunkID[0], sizeof(nextChunkID), 1, waveFile->file);
        if (feof(waveFile->file))
        {
            break;
        }

        if (ferror(waveFile->file) != 0)
        {
            fprintf(stderr, "Error reading input file %s\n", waveFile->filePath);
            waveCloseFile(waveFile);
            return 0;
        }

        if (strncmp(&nextChunkID[0], "fmt ", 4) == 0)
        {
            waveFile->formatChunk = (FormatChunk *)malloc(sizeof(FormatChunk));
            if (waveFile->formatChunk == NULL)
            {
                fprintf(stderr, "Memory Allocation Error: Could not allocate memory for Wave File Format Chunk\n");
                waveCloseFile(waveFile);
                return 0;
            }

            fseek(waveFile->file, -4, SEEK_CUR);
            fread(waveFile->formatChunk, sizeof(FormatChunk), 1, waveFile->file);
            if (ferror(waveFile->file) != 0)
            {
                fprintf(stderr, "Error reading input file %s\n", waveFile->filePath);
                waveCloseFile(waveFile);
                return 0;
            }

            if (littleEndianBytesToUInt16(waveFile->formatChunk->compressionCode) != (uint16_t)1)
            {
                fprintf(stderr, "Compressed audio formats are not supported\n");
                waveCloseFile(waveFile);
                return 0;
            }

            // Note: For compressed audio data there may be extra bytes appended to the format chunk,
            // but as we are only handling uncompressed data we shouldn't encounter them

            // There may or may not be extra data at the end of the fomat chunk.  For uncompressed audio there should be no need, but some files may still have it.
            // if formatChunk.chunkDataSize > 16 (16 = the number of bytes for the format chunk, not counting the 4 byte ID and the chunkDataSize itself) there is extra data
            uint32_t extraFormatBytesCount = littleEndianBytesToUInt32(waveFile->formatChunk->chunkDataSize) - 16;
            if (extraFormatBytesCount > 0)
            {
                formatChunkExtraBytes.startOffset = ftell(waveFile->file);
                formatChunkExtraBytes.size = extraFormatBytesCount;
                fseek(waveFile->file, extraFormatBytesCount, SEEK_CUR);
                if (extraFormatBytesCount % 2 != 0)
                {
                    fseek(waveFile->file, 1, SEEK_CUR);
                }
            }
            printf("Got Format Chunk\n");
        }

        else if (strncmp(&nextChunkID[0], "data", 4) == 0)
        {
            // We found the data chunk

            waveFile->dataChunk = (DataChunk *)malloc(sizeof(DataChunk));
            if (waveFile->dataChunk == NULL)
            {
                fprintf(stderr, "Memory Allocation Error: Could not allocate memory for Wave File Data Chunk\n");
                waveCloseFile(waveFile);
                return 0;
            }

            waveFile->dataChunk->chunkID[0] = 'd';
            waveFile->dataChunk->chunkID[1] = 'a';
            waveFile->dataChunk->chunkID[2] = 't';
            waveFile->dataChunk->chunkID[3] = 'a';

            dataChunkLocation.startOffset = ftell(waveFile->file) - sizeof(nextChunkID);

            // The next 4 bytes are the chunk data size - the size of the sample data
            char sampleDataSizeBytes[4];
            fread(sampleDataSizeBytes, sizeof(char), 4, waveFile->file);
            if (ferror(waveFile->file) != 0)
            {
                fprintf(stderr, "Error reading input file %s\n", waveFile->filePath);
                waveCloseFile(waveFile);
                return 0;
            }
            uint32_t sampleDataSize = littleEndianBytesToUInt32(sampleDataSizeBytes);

            dataChunkLocation.size = sizeof(nextChunkID) + sizeof(sampleDataSizeBytes) + sampleDataSize;

            uint32ToLittleEndianBytes(sampleDataSize, waveFile->dataChunk->chunkDataSize);

            // TODO: check if no errors in allocate and read data
            waveFile->dataChunk->waveformData = (char *) malloc(sampleDataSize);
            fread(waveFile->dataChunk->waveformData, sampleDataSize, 1, waveFile->file);
            if (ferror(waveFile->file) != 0)
            {
                fprintf(stderr, "Error reading input file %s\n", waveFile->filePath);
                waveCloseFile(waveFile);
                return 0;
            }

            // Skip to the end of the chunk.  Chunks must be aligned to 2 byte boundaries, but any padding at the end of a chunk is not included in the chunkDataSize
            if (sampleDataSize % 2 != 0)
            {
                fseek(waveFile->file, 1, SEEK_CUR);
            }

            printf("Got Data Chunk\n");
        }

        else if (strncmp(&nextChunkID[0], "cue ", 4) == 0)
        {
            // We found an existing Cue Chunk

            char cueChunkDataSizeBytes[4];
            fread(cueChunkDataSizeBytes, sizeof(char), 4, waveFile->file);
            if (ferror(waveFile->file) != 0)
            {
                fprintf(stderr, "Error reading input file %s\n", waveFile->filePath);
                waveCloseFile(waveFile);
                return 0;
            }
            uint32_t cueChunkDataSize = littleEndianBytesToUInt32(cueChunkDataSizeBytes);

            char cuePointsCountBytes[4];
            fread(cuePointsCountBytes, sizeof(char), 4, waveFile->file);
            if (ferror(waveFile->file) != 0)
            {
                fprintf(stderr, "Error reading input file %s\n", waveFile->filePath);
                waveCloseFile(waveFile);
                return 0;
            }
            uint32_t cuePointsCount = littleEndianBytesToUInt16(cuePointsCountBytes);

            // Read in the existing cue points into CuePoint Structs
            CuePoint *existingCuePoints = (CuePoint *)malloc(sizeof(CuePoint) * cuePointsCount);
            for (uint32_t cuePointIndex = 0; cuePointIndex < cuePointsCount; cuePointIndex++)
            {
                fread(&existingCuePoints[cuePointIndex], sizeof(CuePoint), 1, waveFile->file);
                if (ferror(waveFile->file) != 0)
                {
                    fprintf(stderr, "Error reading input file %s\n", waveFile->filePath);
                    waveCloseFile(waveFile);
                    return 0;
                }
            }

            // Populate the existingCueChunk struct
            waveFile->cueChunk = (CueChunk *) malloc(sizeof(CueChunk));
            if (waveFile->dataChunk == NULL)
            {
                fprintf(stderr, "Memory Allocation Error: Could not allocate memory for Wave File Cue Chunk\n");
                waveCloseFile(waveFile);
                return 0;
            }
            waveFile->cueChunk->chunkID[0] = 'c';
            waveFile->cueChunk->chunkID[1] = 'u';
            waveFile->cueChunk->chunkID[2] = 'e';
            waveFile->cueChunk->chunkID[3] = ' ';
            uint32ToLittleEndianBytes(cueChunkDataSize, waveFile->cueChunk->chunkDataSize);
            uint32ToLittleEndianBytes(cuePointsCount, waveFile->cueChunk->cuePointsCount);
            waveFile->cueChunk->cuePoints = existingCuePoints;

            printf("Found Existing Cue Chunk\n");
        }

        else
        {
            // We have found a chunk type that we are not going to work with.
            otherChunkLocation.startOffset = ftell(waveFile->file) - sizeof(nextChunkID);

            char chunkDataSizeBytes[4] = {0};
            fread(chunkDataSizeBytes, sizeof(char), 4, waveFile->file);
            if (ferror(waveFile->file) != 0)
            {
                fprintf(stderr, "Error reading input file %s\n", waveFile->filePath);
                waveCloseFile(waveFile);
                return 0;
            }
            uint32_t chunkDataSize = littleEndianBytesToUInt32(chunkDataSizeBytes);

            otherChunkLocation.size = sizeof(nextChunkID) + sizeof(chunkDataSizeBytes) + chunkDataSize;

            // Skip over the chunk's data, and any padding byte
            fseek(waveFile->file, chunkDataSize, SEEK_CUR);
            if (chunkDataSize % 2 != 0)
            {
                fseek(waveFile->file, 1, SEEK_CUR);
            }

            fprintf(stdout, "Found chunk type \'%c%c%c%c\', size: %d bytes\n", nextChunkID[0], nextChunkID[1], nextChunkID[2], nextChunkID[3], chunkDataSize);
        }
    }

    return waveFile;
}

void waveCloseFile(WaveFile *waveFile)
{
    // TODO: free allocated memory in stucts
    if(waveFile == NULL) return;
    if(waveFile->file != NULL) fclose(waveFile->file);
    if(waveFile->formatChunk != NULL) free(waveFile->formatChunk);
    if(waveFile->waveHeader != NULL) free(waveFile->waveHeader);
    if(waveFile->dataChunk != NULL)
    {
        free(waveFile->dataChunk->waveformData);
        free(waveFile->dataChunk);
    }
    if(waveFile->cueChunk != NULL) free(waveFile->cueChunk);
    if(waveFile->filePath != NULL) free(waveFile->filePath);
    free(waveFile);
}

WaveHeader * makeWaveHeader()
{
    WaveHeader * header = (WaveHeader *) malloc(sizeof(WaveHeader));
    uint32ToLittleEndianBytes(0, header->dataSize);
    header->chunkID[0] = 'R';
    header->chunkID[1] = 'I';
    header->chunkID[2] = 'F';
    header->chunkID[3] = 'F';
    header->riffType[0] = 'W';
    header->riffType[1] = 'A';
    header->riffType[2] = 'V';
    header->riffType[3] = 'E';
    return header;
}

FormatChunk * makeFormatChunk(uint16_t numberOfChannels, uint32_t sampleRate, uint16_t significantBitsPerSample)
{
    FormatChunk * format = (FormatChunk *) malloc(sizeof(FormatChunk));
    format->chunkID[0] = 'f';
    format->chunkID[1] = 'm';
    format->chunkID[2] = 't';
    format->chunkID[3] = ' ';
    // Unsigned 4-byte little endian int: Byte count for the remainder of the chunk: 16 + extraFormatbytes.
    uint32ToLittleEndianBytes(16, format->chunkDataSize);
    // Unsigned 2-byte little endian int
    uint16ToLittleEndianBytes(1, format->compressionCode);
    // Unsigned 2-byte little endian int
    uint16ToLittleEndianBytes(numberOfChannels, format->numberOfChannels);
    // Unsigned 4-byte little endian int
    uint32ToLittleEndianBytes(sampleRate, format->sampleRate);
    // Unsigned 2-byte little endian int: The number of bytes per sample slice. This value is not affected by the
    // number of channels and can be calculated with the formula: blockAlign = significantBitsPerSample / 8 * numberOfChannels
    uint16_t blockAlign = significantBitsPerSample / 8 * numberOfChannels;
    uint16ToLittleEndianBytes(blockAlign, format->blockAlign);
    // Unsigned 4-byte little endian int: This value indicates how many bytes of wave data must be streamed to a
    // D/A converter per second in order to play the wave file. This information is useful when determining if data
    // can be streamed from the source fast enough to keep up with playback. = SampleRate * BlockAlign.
    uint32ToLittleEndianBytes(sampleRate * blockAlign, format->averageBytesPerSecond);
    // Unsigned 2-byte little endian int
    uint16ToLittleEndianBytes(significantBitsPerSample, format->significantBitsPerSample);
    return format;
}

DataChunk * makeDataChunk(uint32_t chunkDataSize, char *waveformData)
{
    DataChunk * data = (DataChunk *) malloc(sizeof(DataChunk));
    data->chunkID[0] = 'd';
    data->chunkID[1] = 'a';
    data->chunkID[2] = 't';
    data->chunkID[3] = 'a';
    uint32ToLittleEndianBytes(chunkDataSize, data->chunkDataSize);
    data->waveformData = waveformData;
    return data;
}

WaveFile * makeWaveFile(WaveHeader *waveHeader, FormatChunk *formatChunk, DataChunk *dataChunk)
{
    WaveFile *wave = (WaveFile *) malloc(sizeof(WaveFile));
    wave->filePath = NULL;
    wave->file = NULL;
    uint32_t fileSize = 4; // the 4 bytes for the Riff Type "WAVE"
    if(formatChunk != NULL) fileSize += sizeof(FormatChunk);
    if(dataChunk != NULL)
    {
        uint32_t dataChunkSize = sizeof(DataChunk) + littleEndianBytesToUInt32(dataChunk->chunkDataSize);
        fileSize += dataChunkSize;
        if (dataChunkSize % 2 != 0) fileSize++;
    }
    uint32ToLittleEndianBytes(fileSize, waveHeader->dataSize);
    wave->waveHeader = waveHeader;
    wave->formatChunk = formatChunk;
    wave->dataChunk = dataChunk;
    wave->cueChunk = NULL;
    return wave;
}

void saveWaveFile(WaveFile *waveFile, const char *filePath)
{
    waveFile->filePath = filePath;
    waveFile->file = fopen(waveFile->filePath, "wb");
    if (waveFile->file == NULL)
    {
        fprintf(stderr, "Could not open output file %s\n", waveFile->filePath);
        return;
    }
    if (waveFile->waveHeader != NULL)
        if (fwrite(waveFile->waveHeader, sizeof(WaveHeader), 1, waveFile->file) < 1)
        {
            fprintf(stderr, "Error writing header to output file.\n");
            return;
        }
    if (waveFile->formatChunk != NULL)
        if (fwrite(waveFile->formatChunk, sizeof(FormatChunk), 1, waveFile->file) < 1)
        {
            fprintf(stderr, "Error writing format chunk to output file.\n");
            return;
        }
    if (waveFile->dataChunk != NULL)
    {
        uint32_t dataChunkSize = littleEndianBytesToUInt32(waveFile->dataChunk->chunkDataSize);
        if (fwrite(waveFile->dataChunk->chunkID, sizeof(char), 4, waveFile->file) < 1)
        {
            fprintf(stderr, "Error writing data chunk (chunkID) to output file.\n");
            return;
        }
        if (fwrite(waveFile->dataChunk->chunkDataSize, sizeof(char), 4, waveFile->file) < 1)
        {
            fprintf(stderr, "Error writing data chunk (chunkDataSize) to output file.\n");
            return;
        }
        if (fwrite(waveFile->dataChunk->waveformData, dataChunkSize, 1, waveFile->file) < 1)
        {
            fprintf(stderr, "Size data chunk (waveformData) %d.\n", dataChunkSize);
            fprintf(stderr, "Error writing data chunk (waveformData) to output file.\n");
            return;
        }
        if (dataChunkSize % 2 != 0)
        {
            if (fwrite("\0", sizeof(char), 1, waveFile->file) < 1)
            {
                fprintf(stderr, "Error writing padding character to output file.\n");
                return;

            }
        }
    }
    fclose(waveFile->file);
}

enum HostEndiannessType getHostEndianness()
{
    int i = 1;
    char *p = (char *)&i;

    if (p[0] == 1)
        return LittleEndian;
    else
        return BigEndian;
}

uint32_t littleEndianBytesToUInt32(char littleEndianBytes[4])
{
    if (HostEndianness == EndiannessUndefined)
    {
        HostEndianness = getHostEndianness();
    }

    uint32_t uInt32Value;
    char *uintValueBytes = (char *)&uInt32Value;

    if (HostEndianness == LittleEndian)
    {
        uintValueBytes[0] = littleEndianBytes[0];
        uintValueBytes[1] = littleEndianBytes[1];
        uintValueBytes[2] = littleEndianBytes[2];
        uintValueBytes[3] = littleEndianBytes[3];
    }
    else
    {
        uintValueBytes[0] = littleEndianBytes[3];
        uintValueBytes[1] = littleEndianBytes[2];
        uintValueBytes[2] = littleEndianBytes[1];
        uintValueBytes[3] = littleEndianBytes[0];
    }

    return uInt32Value;
}

void uint32ToLittleEndianBytes(uint32_t uInt32Value, char out_LittleEndianBytes[4])
{
    if (HostEndianness == EndiannessUndefined)
    {
        HostEndianness = getHostEndianness();
    }

    char *uintValueBytes = (char *)&uInt32Value;

    if (HostEndianness == LittleEndian)
    {
        out_LittleEndianBytes[0] = uintValueBytes[0];
        out_LittleEndianBytes[1] = uintValueBytes[1];
        out_LittleEndianBytes[2] = uintValueBytes[2];
        out_LittleEndianBytes[3] = uintValueBytes[3];
    }
    else
    {
        out_LittleEndianBytes[0] = uintValueBytes[3];
        out_LittleEndianBytes[1] = uintValueBytes[2];
        out_LittleEndianBytes[2] = uintValueBytes[1];
        out_LittleEndianBytes[3] = uintValueBytes[0];
    }
}

uint16_t littleEndianBytesToUInt16(char littleEndianBytes[2])
{
    if (HostEndianness == EndiannessUndefined)
    {
        HostEndianness = getHostEndianness();
    }

    uint32_t uInt16Value;
    char *uintValueBytes = (char *)&uInt16Value;

    if (HostEndianness == LittleEndian)
    {
        uintValueBytes[0] = littleEndianBytes[0];
        uintValueBytes[1] = littleEndianBytes[1];
    }
    else
    {
        uintValueBytes[0] = littleEndianBytes[1];
        uintValueBytes[1] = littleEndianBytes[0];
    }

    return uInt16Value;
}


void uint16ToLittleEndianBytes(uint16_t uInt16Value, char out_LittleEndianBytes[2])
{
    if (HostEndianness == EndiannessUndefined)
    {
        HostEndianness = getHostEndianness();
    }

    char *uintValueBytes = (char *)&uInt16Value;

    if (HostEndianness == LittleEndian)
    {
        out_LittleEndianBytes[0] = uintValueBytes[0];
        out_LittleEndianBytes[1] = uintValueBytes[1];
    }
    else
    {
        out_LittleEndianBytes[0] = uintValueBytes[1];
        out_LittleEndianBytes[1] = uintValueBytes[0];
    }
}
