#ifndef TIMESOUNDRECORDER_H
#define TIMESOUNDRECORDER_H

#include "soundrecorder.h"

struct buffer;

class TimeSoundRecorder : public SoundRecorder
{
    Q_OBJECT
public:
    explicit TimeSoundRecorder(oal_device *device, int sampleByteSize, int time = 3, QObject *parent = 0);
    ~TimeSoundRecorder();

protected:
    void allocateNewBuffer();
    int buffersCounter;
    int maxBuffersCount;

signals:

public slots:

};

#endif // TIMESOUNDRECORDER_H
