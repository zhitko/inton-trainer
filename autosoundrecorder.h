#ifndef AUTOSOUNDRECORDER_H
#define AUTOSOUNDRECORDER_H

#include "soundrecorder.h"

const int MAX_EMPTY_BUFFERS_BEFORE = 4;
const int MAX_EMPTY_BUFFERS_AFTER = 2;

struct buffer;

class AutoSoundRecorder : public SoundRecorder
{
    Q_OBJECT
public:
    explicit AutoSoundRecorder(oal_device *device, int sampleByteSize, QObject *parent = 0);
    ~AutoSoundRecorder();

    int getData(void **);
    int trimDataByMidEnergy(void* buffer, int bufferSize, void** outputBuffer);
protected:
    void allocateNewBuffer();
    int buffersCounter, emptyBuffersCounter;
    bool isOver;
    buffer * lastActiveBuffer;

signals:

public slots:

};

#endif // AUTOSOUNDRECORDER_H
