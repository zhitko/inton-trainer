#ifndef SOUNDRECORDER_H
#define SOUNDRECORDER_H

#include "defines.h"

#include <QThread>

extern "C" {
    #include "./OpenAL/openal_wrapper.h"
}

struct buffer;

class SoundRecorder : public QThread
{
    Q_OBJECT
protected:
    void run();
    virtual void allocateNewBuffer();

    int initBufferSize;
    int length;
    int sampleByteSize;
    buffer * initBuffer, * currentBuffer;
    oal_device * device;
    int allocatedSize;
    long int avgVolumeLevel;
    bool recording;
    int currentPos;
public:
    explicit SoundRecorder(oal_device *device, int sampleByteSize, QObject *parent = 0);
    ~SoundRecorder();
    long int getVolumeLevel();
    long int getMaxVolumeLevel();
    bool isRecording();
    int getData(void **);
signals:
    void resultReady(SoundRecorder *);
    void beep();
public slots:
    void stopBeep();
    void startRecording();
    void stopRecording();
    void _startCapture();
};

#endif // SOUNDRECORDER_H
