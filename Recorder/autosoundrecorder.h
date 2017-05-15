#ifndef AUTOSOUNDRECORDER_H
#define AUTOSOUNDRECORDER_H

#include "soundrecorder.h"

const int MAX_EMPTY_BUFFERS_BEFORE = 3;
const int MAX_EMPTY_BUFFERS_AFTER = 3;

struct buffer;

class AutoSoundRecorder : public SoundRecorder
{
    Q_OBJECT
public:
    explicit AutoSoundRecorder(oal_device *device, int sampleByteSize, int maxTime = -1, int maxIdleTime = -1, QObject *parent = 0);
    ~AutoSoundRecorder();

protected:
    void allocateNewBuffer();
    int buffersCounter, emptyBuffersCounter;
    bool isSpeechDetected;
    buffer * lastActiveBuffer;
    int maxRecordSize;
    int maxIdleSize;

signals:

public slots:

};

#endif // AUTOSOUNDRECORDER_H
