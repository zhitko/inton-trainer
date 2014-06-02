#ifndef WAVFILE_H
#define WAVFILE_H

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

// The header of a wave file
typedef struct {
    char chunkID[4];      // Must be "RIFF" (0x52494646)
    char dataSize[4];     // Byte count for the rest of the file (i.e. file length - 8 bytes)
    char riffType[4];     // Must be "WAVE" (0x57415645)
} WaveHeader;

// The format chunk of a wave file
typedef struct {
    char chunkID[4];                  // String: must be "fmt " (0x666D7420).
    char chunkDataSize[4];            // Unsigned 4-byte little endian int: Byte count for the remainder of the chunk: 16 + extraFormatbytes.
    char compressionCode[2];          // Unsigned 2-byte little endian int
    char numberOfChannels[2];         // Unsigned 2-byte little endian int
    char sampleRate[4];               // Unsigned 4-byte little endian int
    char averageBytesPerSecond[4];    // Unsigned 4-byte little endian int: This value indicates how many bytes of wave data must be streamed to a D/A converter per second in order to play the wave file. This information is useful when determining if data can be streamed from the source fast enough to keep up with playback. = SampleRate * BlockAlign.
    char blockAlign[2];               // Unsigned 2-byte little endian int: The number of bytes per sample slice. This value is not affected by the number of channels and can be calculated with the formula: blockAlign = significantBitsPerSample / 8 * numberOfChannels
    char significantBitsPerSample[2]; // Unsigned 2-byte little endian int
} FormatChunk;

typedef struct {
    char chunkID[4];              // String: must be "data"
    char chunkDataSize[4];        // Unsigned 4-byte little endian int
    char *waveformData;
} DataChunk;

// CuePoint: each individual 'marker' in a wave file is represented by a cue point.
typedef struct {
    char cuePointID[4];        // a unique ID for the Cue Point.
    char playOrderPosition[4]; // Unsigned 4-byte little endian int: If a Playlist chunk is present in the Wave file, this the sample number at which this cue point will occur during playback of the entire play list as defined by the play list's order.  **Otherwise set to same as sample offset??***  Set to 0 when there is no playlist.
    char dataChunkID[4];       // Unsigned 4-byte little endian int: The ID of the chunk containing the sample data that corresponds to this cue point.  If there is no playlist, this should be 'data'.
    char chunkStart[4];        // Unsigned 4-byte little endian int: The byte offset into the Wave List Chunk of the chunk containing the sample that corresponds to this cue point. This is the same chunk described by the Data Chunk ID value. If no Wave List Chunk exists in the Wave file, this value is 0.
    char blockStart[4];        // Unsigned 4-byte little endian int: The byte offset into the "data" or "slnt" Chunk to the start of the block containing the sample. The start of a block is defined as the first byte in uncompressed PCM wave data or the last byte in compressed wave data where decompression can begin to find the value of the corresponding sample value.
    char frameOffset[4];       // Unsigned 4-byte little endian int: The offset into the block (specified by Block Start) for the sample that corresponds to the cue point.
} CuePoint;

// CuePoints are stored in a CueChunk
typedef struct {
    char chunkID[4];        // String: Must be "cue " (0x63756520).
    char chunkDataSize[4];  // Unsigned 4-byte little endian int: Byte count for the remainder of the chunk: 4 (size of cuePointsCount) + (24 (size of CuePoint struct) * number of CuePoints).
    char cuePointsCount[4]; // Unsigned 4-byte little endian int: Length of cuePoints[].
    CuePoint *cuePoints;
} CueChunk;

// Some chunks we don't care about the contents and will just copy them from the input file to the output,
// so this struct just stores positions of such chunks in the input file
typedef struct {
    long startOffset; // in bytes
    long size;		  // in bytes
} ChunkLocation;

// Wave file
typedef struct {
    char *filePath;
    FILE *file;
    WaveHeader *waveHeader;
    FormatChunk *formatChunk;
    DataChunk *dataChunk;
    CueChunk *cueChunk;
} WaveFile;

WaveFile * initWaveFile();

// Open WaveFile
WaveFile * waveOpenHFile(int);
WaveFile * waveOpenFile(const char*);

// Close WaveFile
void waveCloseFile(WaveFile*);

// Create and Write WaveFile functions
WaveHeader * makeWaveHeader();
FormatChunk * makeFormatChunk(uint16_t numberOfChannels, uint32_t sampleRate, uint16_t significantBitsPerSample);
DataChunk * makeDataChunk(uint32_t chunkDataSize, char *waveformData);
WaveFile * makeWaveFile(WaveHeader *waveHeader, FormatChunk *formatChunk, DataChunk *dataChunk);
 WaveFile * makeWaveFileFromData(char *waveformData, uint32_t chunkDataSize, uint16_t numberOfChannels, uint32_t sampleRate, uint16_t significantBitsPerSample);
void saveWaveFile(WaveFile *waveFile, const char *filePath);

// All data in a Wave file must be little endian.
// These are functions to convert 2- and 4-byte unsigned ints to and from little endian, if needed

enum HostEndiannessType {
    EndiannessUndefined = 0,
    LittleEndian,
    BigEndian
};

static enum HostEndiannessType HostEndianness = EndiannessUndefined;

enum HostEndiannessType getHostEndianness();
uint32_t littleEndianBytesToUInt32(char littleEndianBytes[4]);
void uint32ToLittleEndianBytes(uint32_t uInt32Value, char out_LittleEndianBytes[4]);
uint16_t littleEndianBytesToUInt16(char littleEndianBytes[2]);
void uint16ToLittleEndianBytes(uint16_t uInt16Value, char out_LittleEndianBytes[2]);

#endif // WAVFILE_H
