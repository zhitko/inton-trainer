/*
based on https://gist.github.com/jimmcgowan/4268832
*/

#include <QDebug>
#include <QSound>

#include "buffer.h"
#include "soundrecorder.h"
#include "settingsdialog.h"

SoundRecorder::SoundRecorder(oal_device *device, int sampleByteSize, QObject *parent) :
    QThread(parent), device(device), sampleByteSize(sampleByteSize),
    length(0), allocatedSize(0), avgVolumeLevel(0), recording(false),
    initBuffer(NULL), currentBuffer(NULL), currentPos(0)
{
    initAudioInputDevice(this->device, RECORD_FREQ);
    connect(this, SIGNAL(beep()), this, SLOT(stopBeep()));
    SPTK_SETTINGS * sptk_settings = SettingsDialog::getSPTKsettings();
    this->initBufferSize = RECORD_FREQ * this->sampleByteSize * sptk_settings->dp->recordingFrameSeconds;
}

SoundRecorder::~SoundRecorder()
{
    qDebug() << "~SoundRecorder" << LOG_DATA;
    freeAudioInputDevice(this->device);
    freeBuffer(this->initBuffer, true);
}

long int SoundRecorder::getVolumeLevel()
{
    return this->avgVolumeLevel;
}

long int SoundRecorder::getMaxVolumeLevel()
{
    switch(this->sampleByteSize)
    {
        case 1: return CHAR_MAX;
        case 2: return SHRT_MAX;
        case 4: return INT_MAX;
    }
    return 0;
}

bool SoundRecorder::isRecording()
{
    return this->recording;
}

int SoundRecorder::getData(void ** resBuffer)
{
    return getBufferData(this->initBuffer, resBuffer, true);
}

int getValue(void* data, int size)
{
    int res = 0;
    switch(size)
    {
        case 1 :
            res = *((unsigned char*)data);
            break;
        case 2:
            res = *((signed short int *)data);
            break;
        case 4:
            res = *((signed long int *)data);
            break;
    }
    if(res < 0) res *= -1;
    return res;
}

void SoundRecorder::allocateNewBuffer()
{
    if(currentBuffer){
        buffer * current = this->currentBuffer;
        this->currentBuffer = makeBuffer(this->initBufferSize);
        this->currentBuffer->prev = current;
        current->next = this->currentBuffer;
        this->allocatedSize += this->initBufferSize;
    }else{
        this->initBuffer = makeBuffer(this->initBufferSize);
        this->currentBuffer = this->initBuffer;
        this->allocatedSize = this->initBufferSize;
    }
}

void SoundRecorder::run()
{
    this->allocatedSize = 0;
    currentPos = 0;
    while(recording)
    {
        msleep(200);
        qDebug() << "recording step " << this->allocatedSize << " ( " << currentPos << " : " << this->initBufferSize << " )" << LOG_DATA;
        if( currentPos >= this->initBufferSize || this->allocatedSize < this->initBufferSize )
        {
            allocateNewBuffer();
            if (!recording)
                break;
            currentPos = 0;
        }
        void * pointToWrite = (char*)this->currentBuffer->buffer_data + currentPos;
        int maxToWrite = this->initBufferSize - currentPos;
        int size = getSample(this->device, pointToWrite, this->sampleByteSize, maxToWrite);
        currentPos += size;
    }
    qDebug() << "stopCapture" << LOG_DATA;
    stopCapture(this->device);
    emit beep();
    msleep(200);
    emit resultReady(this);
}

#include <QTimer>

void SoundRecorder::startRecording()
{
    QSound::play("://signals/sounds/start.wav");

    QTimer::singleShot(400, this, SLOT(_startCapture()));
}

void SoundRecorder::_startCapture()
{
    startCapture(this->device);
    this->recording = true;
    this->start(LowPriority);

}

void SoundRecorder::stopBeep()
{
    QSound::play("://signals/sounds/end.wav");
}

void SoundRecorder::stopRecording()
{
    this->recording = false;
}
