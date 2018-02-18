#ifndef AUTOSOUNDRECORDER_H
#define AUTOSOUNDRECORDER_H

#include "defines.h"

#include "soundrecorder.h"

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
