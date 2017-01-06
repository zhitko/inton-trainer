#include "autosoundrecorder.h"

#include "settingsdialog.h"

#include "buffer.h"

extern "C" {
    #include "./SPTK/SPTK.h"
    #include "./SPTK/pitch/pitch.h"
    #include "./SPTK/frame/frame.h"
    #include "./SPTK/x2x/x2x.h"
    #include "./SPTK/others/func.h"
    #include "./SPTK/others/interpolation.h"
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


int AutoSoundRecorder::trimDataByMidEnergy(void* buffer, int bufferSize, void** outputBuffer)
{
    SPTK_SETTINGS * sptk_settings = SettingsDialog::getSPTKsettings();

    vector wave = sptk_v2v(buffer, bufferSize, this->sampleByteSize*CHAR_BIT);
    vector frame = sptk_frame(wave, sptk_settings->frame);
    vector intensive = vector_intensive(frame, sptk_settings->frame);
    vector midIntensive = vector_avg_intensive(intensive, sptk_settings->energyFrame);
    vector normMidIntensive = normalizev(midIntensive, 0.0, 1.0);

    int sourceStartIndex = first_greaterv(normMidIntensive, sptk_settings->energyFrame->threshold_start);
    int sourceEndIndex = last_greaterv(normMidIntensive, sptk_settings->energyFrame->threshold_end);

    int waveLen = bufferSize / this->sampleByteSize;
    double indexScale = (1.0 * waveLen) / normMidIntensive.x;

    int destinationStartIndex = sourceStartIndex * indexScale;
    int destinationEndIndex = sourceEndIndex * indexScale;

    int newSize = destinationEndIndex - destinationStartIndex;
    int newSizeByte = newSize * this->sampleByteSize;

    qDebug() << "sourceStartIndex " << sourceStartIndex;
    qDebug() << "destinationStartIndex " << destinationStartIndex;
    qDebug() << "sourceEndIndex" << sourceEndIndex;
    qDebug() << "destinationEndIndex" << destinationEndIndex;
    qDebug() << "normMidIntensive" << normMidIntensive.x;
    qDebug() << "bufferSize " << bufferSize;
    qDebug() << "waveLen " << waveLen;
    qDebug() << "indexScale " << indexScale;
    qDebug() << "newSize " << newSize;
    qDebug() << "newSizeByte " << newSizeByte;

    *outputBuffer = malloc(newSizeByte);
    void * start_pointer = ((char*)buffer) + (destinationStartIndex * this->sampleByteSize);
    memcpy(*outputBuffer, start_pointer, newSizeByte);

    freev(wave);
    freev(frame);
    freev(intensive);
    freev(midIntensive);    
    freev(normMidIntensive);
    return newSizeByte;
}

int AutoSoundRecorder::getData(void ** resBuffer)
{
    char *data;
    int bufferSize = getBufferData(this->initBuffer, (void**) &data, true);
    int newBufferSize = trimDataByMidEnergy(data, bufferSize, resBuffer);
    return newBufferSize;
}

void AutoSoundRecorder::allocateNewBuffer()
{
    if(!recording) return;
    if(this->currentBuffer)
    {
        buffersCounter++;
        if(!isOver && buffersCounter >= MAX_EMPTY_BUFFERS_BEFORE && this->initBuffer->next)
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
        else if(this->isOver && this->emptyBuffersCounter >= MAX_EMPTY_BUFFERS_AFTER)
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
