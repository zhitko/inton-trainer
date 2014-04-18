#ifndef SOUNDRECORDER_H
#define SOUNDRECORDER_H

#include <QThread>

extern "C" {
    #include "./OpenAL/openal_wrapper.h"
}

const int INIT_BUFFER_SIZE = 4096;// 4096//8192//16384

struct buffer;

class SoundRecorder : public QThread
{
    Q_OBJECT
protected:
    void run();
    virtual void allocateNewBuffer();

    int length;
    int sampleByteSize;
    buffer * initBuffer, * currentBuffer;
    oal_device * device;
    int allocatedSize;
    long int avgVolumeLevel;
    bool recording;
public:
    explicit SoundRecorder(oal_device *device, int sampleByteSize, QObject *parent = 0);
    ~SoundRecorder();
    long int getVolumeLevel();
    long int getMaxVolumeLevel();
    bool isRecording();
    int getData(void **);
signals:
    void resultReady(SoundRecorder *);
public slots:
    void startRecording();
    void stopRecording();
};

#endif // SOUNDRECORDER_H
