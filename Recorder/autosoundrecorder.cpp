#include "autosoundrecorder.h"

#include "buffer.h"

extern "C" {
    #include "SPTK/SPTK.h"
    #include "SPTK/pitch/pitch.h"
    #include "SPTK/x2x/x2x.h"
}

#include <QDebug>

AutoSoundRecorder::AutoSoundRecorder(oal_device *device, int sampleByteSize, int maxTime, QObject *parent) :
    SoundRecorder(device, sampleByteSize, parent),
    lastActiveBuffer(NULL),
    buffersCounter(0),
    emptyBuffersCounter(0),
    isSpeechDetected(false)
{
    maxRecordSize = maxTime * 8000 * this->sampleByteSize;
    qDebug() << "AutoSoundRecorder " << maxRecordSize;
}

AutoSoundRecorder::~AutoSoundRecorder()
{
    qDebug() << "~AutoSoundRecorder";
    if(lastActiveBuffer)
        freeBuffer(this->lastActiveBuffer, true);
    else
        freeBuffer(this->currentBuffer);
}

void AutoSoundRecorder::allocateNewBuffer()
{
    if(!recording) return;
    if(this->maxRecordSize > 0 && this->allocatedSize > this->maxRecordSize) {
        stopRecording();
        return;
    }
    if(this->currentBuffer)
    {
        buffersCounter++;
        if(!this->isSpeechDetected && buffersCounter >= MAX_EMPTY_BUFFERS && this->initBuffer->next)
        {
            buffersCounter--;
            buffer * tmp = this->initBuffer;
            this->initBuffer = this->initBuffer->next;
            this->initBuffer->prev = NULL;
            freeBuffer(tmp);
            // TODO: reuse first empty buffer

        }
        vector wave = sptk_v2v(this->currentBuffer->buffer_data, this->currentBuffer->size, this->sampleByteSize*CHAR_BIT);
        vector pitch = sptk_pitch(wave, initPitchSettings());

        double val = 0.0;
        for(int i=0; i<pitch.x; i++) val += pitch.v[i];
        if( val > 0)
        {
            this->isSpeechDetected = true;
            this->emptyBuffersCounter = 0;
            this->lastActiveBuffer = NULL;
            SoundRecorder::allocateNewBuffer();
        } else if(this->isSpeechDetected && this->emptyBuffersCounter >= MAX_EMPTY_BUFFERS)
        {
            if(this->lastActiveBuffer && this->lastActiveBuffer->prev)
            {
                this->lastActiveBuffer->prev->next = NULL;
            }
            else if(this->currentBuffer && this->currentBuffer->prev)
            {
                this->currentBuffer->prev->next = NULL;
            }
            stopRecording();
        } else if(this->isSpeechDetected) {
            this->emptyBuffersCounter++;
            if(!this->lastActiveBuffer) this->lastActiveBuffer = this->currentBuffer;
            SoundRecorder::allocateNewBuffer();
        } else {

        }
    } else {
        SoundRecorder::allocateNewBuffer();
    }
}
