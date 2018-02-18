#include "dpsoundrecorder.h"

#include <QFile>

#include <math.h>

#include "settingsdialog.h"

#include "buffer.h"
#include "DP/continuousdp.h"

extern "C" {
    #include "./OpenAL/wavFile.h"

    #include "./SPTK/SPTK.h"
    #include "./SPTK/pitch/pitch.h"
    #include "./SPTK/frame/frame.h"
    #include "./SPTK/x2x/x2x.h"
    #include "./SPTK/lpc/lpc.h"
    #include "./SPTK/spec/spec.h"
    #include "./SPTK/others/func.h"
    #include "./SPTK/others/interpolation.h"
}

#include <QDebug>

DPSoundRecorder::DPSoundRecorder(QString fname, oal_device *device, int sampleByteSize, QObject *parent) :
    SoundRecorder(device, sampleByteSize, parent),
    buffersCounter(0),
    dp(NULL)
{
    qDebug() << "DPSoundRecorder" << LOG_DATA;

    QFile file(fname);
    file.open(QIODevice::ReadOnly);
    WaveFile * waveFile = waveOpenHFile(file.handle());
    qDebug() << "waveOpenFile" << LOG_DATA;

    int size = littleEndianBytesToUInt32(waveFile->dataChunk->chunkDataSize);
    qDebug() << "chunkDataSize" << LOG_DATA;
    short int bits = littleEndianBytesToUInt16(waveFile->formatChunk->significantBitsPerSample);
    qDebug() << "significantBitsPerSample" << LOG_DATA;

    vector wave = sptk_v2v(waveFile->dataChunk->waveformData, size, bits);
    qDebug() << "wave" << LOG_DATA;

    this->buffersBeforeMaxCount = ceil(DP_SIGNALE_SCALE * wave.x / this->initBufferSize);

    this->pattern = this->proc(wave);
    freev(wave);
}

DPSoundRecorder::~DPSoundRecorder()
{
    if(this->dp)
    {
        delete dp;
    }
    qDebug() << "~DPSoundRecorder" << LOG_DATA;
}

vector DPSoundRecorder::proc(vector wave)
{
    SPTK_SETTINGS * sptk_settings = SettingsDialog::getSPTKsettings();

    vector frame = sptk_frame(wave, sptk_settings->frame);
    vector lpc = sptk_lpc(frame, sptk_settings->lpc);
    vector spec = sptk_spec(lpc, sptk_settings->spec);

    freev(frame);
    freev(lpc);

    return spec;
}

bool DPSoundRecorder::procDP(vector signal)
{
    SPTK_SETTINGS * sptk_settings = SettingsDialog::getSPTKsettings();
    int speksize = sptk_settings->spec->leng / 2 + 1;

    if (this->dp == NULL)
    {
        this->dp = new ContinuousDP(
            new SpectrSignal(copyv(this->pattern), speksize),
            new SpectrSignal(copyv(signal), speksize),
            DP_SIGNALE_SCALE,
            sptk_settings->dp->continiusLimit
        );
    } else {
        this->dp->addNewSignal(new SpectrSignal(copyv(signal), speksize));
    }
    return this->dp->calculate();
}

int DPSoundRecorder::getData(void ** resBuffer)
{
    int bufferSize = getBufferData(this->initBuffer, resBuffer, true);
    return bufferSize;
}

void DPSoundRecorder::allocateNewBuffer()
{
    /*
    if(!recording) return;
    if(this->currentBuffer)
    {
        buffersCounter++;
        if(buffersCounter >= this->buffersBeforeMaxCount)
        {
            buffersCounter--;
            buffer * tmp = this->initBuffer;
            this->initBuffer = this->initBuffer->next;
            this->initBuffer->prev = NULL;
            freeBuffer(tmp);

        }

        vector wave = sptk_v2v(this->currentBuffer->buffer, this->currentBuffer->size, this->sampleByteSize*CHAR_BIT);
        vector spec = this->proc(wave);

        bool isDPGotMin = this->procDP(spec);

        freev(wave);
        freev(spec);

        if( isDPGotMin )
        {
            stopRecording();
        }
    }
    */
    SoundRecorder::allocateNewBuffer();
}
