#include "autosoundrecorder.h"

#include "settingsdialog.h"

#include "buffer.h"

extern "C" {
    #include "./SPTK/SPTK.h"
    #include "./SPTK/pitch/pitch.h"
    #include "./SPTK/x2x/x2x.h"
}

#include <QDebug>

AutoSoundRecorder::AutoSoundRecorder(oal_device *device, int sampleByteSize, QObject *parent) :
    SoundRecorder(device, sampleByteSize, parent),
    lastActiveBuffer(NULL),
    buffersCounter(0),
    emptyBuffersCounter(0),
    isOver(false)
{
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
    if(this->currentBuffer)
    {
        buffersCounter++;
        if(!isOver && buffersCounter >= MAX_EMPTY_BUFFERS && this->initBuffer->next)
        {
            buffersCounter--;
            buffer * tmp = this->initBuffer;
            this->initBuffer = this->initBuffer->next;
            this->initBuffer->prev = NULL;
            freeBuffer(tmp);

        }
        SPTK_SETTINGS * settings = SettingsDialog::getSPTKsettings();

        qDebug() << "sound data";
        vector wave = sptk_v2v(this->currentBuffer->buffer, this->currentBuffer->size, this->sampleByteSize*CHAR_BIT);
        qDebug() << "sound pitch";
        vector pitch = sptk_pitch(wave, settings->pitch);

        double val = 0.0;
        for(int i=0; i<pitch.x; i++) val += pitch.v[i];
        qDebug() << "Sound pitch sum " << val;
        if( val > 0)
        {
            this->isOver = true;
            this->emptyBuffersCounter = 0;
            this->lastActiveBuffer = NULL;
        }
        else if(this->isOver && this->emptyBuffersCounter >= MAX_EMPTY_BUFFERS)
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
            return;
        }else{
            this->emptyBuffersCounter++;
            if(!this->lastActiveBuffer) this->lastActiveBuffer = this->currentBuffer;
        }
    }
    SoundRecorder::allocateNewBuffer();
    qDebug() << "AutoSoundRecorder::allocateNewBuffer";
}
