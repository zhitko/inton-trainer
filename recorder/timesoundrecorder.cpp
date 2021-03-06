#include "timesoundrecorder.h"

#include "buffer.h"

extern "C" {
    #include "SPTK/SPTK.h"
}

#include <QDebug>

TimeSoundRecorder::TimeSoundRecorder(oal_device *device, int sampleByteSize, int time, QObject *parent) :
    SoundRecorder(device, sampleByteSize, parent),
    buffersCounter(0)
{
    maxBuffersCount = (time * RECORD_FREQ * CHAR_BIT_RECORD) / this->initBufferSize;
    qDebug() << "TimeSoundRecorder " << maxBuffersCount << LOG_DATA;
}

TimeSoundRecorder::~TimeSoundRecorder()
{
    qDebug() << "~TimeSoundRecorder" << LOG_DATA;
}


void TimeSoundRecorder::allocateNewBuffer()
{
    if(!recording) return;
    if(this->buffersCounter > this->maxBuffersCount) {
        stopRecording();
        return;
    }
    this->buffersCounter ++;
    SoundRecorder::allocateNewBuffer();
}
