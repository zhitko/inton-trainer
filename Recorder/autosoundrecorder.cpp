#include "autosoundrecorder.h"

#include "buffer.h"
#include "settingsdialog.h"

extern "C" {
    #include "SPTK/SPTK.h"
    #include "SPTK/pitch/pitch.h"
    #include "SPTK/x2x/x2x.h"
}

#include <QDebug>

AutoSoundRecorder::AutoSoundRecorder(oal_device *device, int sampleByteSize, int maxTime, int maxIdleTime, QObject *parent) :
    SoundRecorder(device, sampleByteSize, parent),
    lastActiveBuffer(NULL),
    buffersCounter(0),
    emptyBuffersCounter(0),
    isSpeechDetected(false)
{
    SPTK_SETTINGS * sptk_settings = SettingsDialog::getSPTKsettings();

    maxRecordSize = maxTime * RECORD_FREQ * this->sampleByteSize;
    maxIdleSize = maxIdleTime * RECORD_FREQ * this->sampleByteSize;
    if (maxIdleSize > (sptk_settings->dp->recordingFrameBefore*this->initBufferSize))
        maxIdleSize = sptk_settings->dp->recordingFrameBefore*this->initBufferSize-1;
    qDebug() << "AutoSoundRecorder " << maxRecordSize << LOG_DATA;
}

AutoSoundRecorder::~AutoSoundRecorder()
{
    qDebug() << "~AutoSoundRecorder" << LOG_DATA;
    if(lastActiveBuffer)
        freeBuffer(this->lastActiveBuffer, true);
    else
        freeBuffer(this->currentBuffer);
}

void AutoSoundRecorder::allocateNewBuffer()
{
    SPTK_SETTINGS * sptk_settings = SettingsDialog::getSPTKsettings();
    if(!recording) return;
    if(this->maxRecordSize > 0 && this->allocatedSize > this->maxRecordSize) {
        stopRecording();
        return;
    }
    if(!this->isSpeechDetected && this->maxIdleSize > 0 && (this->buffersCounter*this->initBufferSize+this->currentPos) > this->maxIdleSize) {
        stopRecording();
        return;
    }
    if(this->currentBuffer)
    {
        buffersCounter++;
        if(!this->isSpeechDetected && buffersCounter >= sptk_settings->dp->recordingFrameBefore && this->initBuffer->next)
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
        for(int i=0; i<pitch.x; i++) val += getv(pitch, i);
        if( val > 0)
        {
            this->isSpeechDetected = true;
            this->emptyBuffersCounter = 0;
            this->lastActiveBuffer = NULL;
            SoundRecorder::allocateNewBuffer();
        } else if(this->isSpeechDetected && this->emptyBuffersCounter >= sptk_settings->dp->recordingFrameAfter)
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
