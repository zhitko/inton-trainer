#ifndef DPSOUNDRECORDER
#define DPSOUNDRECORDER

#include "defines.h"

#include "soundrecorder.h"

const int DP_SIGNALE_SCALE = 3;

struct buffer;

extern "C" {
    #include "./sptk/vector.h"
}

class ContinuousDP;

class DPSoundRecorder : public SoundRecorder
{
    Q_OBJECT
public:
    explicit DPSoundRecorder(QString, oal_device*, int, QObject *parent = 0);
    ~DPSoundRecorder();

    int getData(void **);
protected:
    virtual void allocateNewBuffer();
    vector proc(vector);
    bool procDP(vector);

    int buffersCounter;
    vector pattern;
    int buffersBeforeMaxCount;
    ContinuousDP * dp;

signals:

public slots:

};


#endif // DPSOUNDRECORDER

