#include "drawereval.h"

#include <QFile>
#include <QDebug>

#include "settingsdialog.h"

#include "DP/vectordp.h"

extern "C" {
    #include "./OpenAL/wavFile.h"

    #include "float.h"

    #include "./SPTK/SPTK.h"
    #include "./SPTK/pitch/pitch.h"
    #include "./SPTK/x2x/x2x.h"
    #include "./SPTK/frame/frame.h"
    #include "./SPTK/window/window.h"
    #include "./SPTK/lpc/lpc.h"
    #include "./SPTK/spec/spec.h"
}

DrawerEval::DrawerEval(QString fname) :
    Drawer(fname),
    secFileName(""),
    _secWaveMin(DBL_MAX), _secWaveMax(DBL_MIN),
    _secPitchMin(DBL_MAX), _secPitchMax(DBL_MIN),
    secWaveDataLen(0)
{
}

int DrawerEval::Draw(mglGraph *gr)
{
    qDebug() << "start drawing";

    gr->DefaultPlotParam();
    gr->Clf();

    qDebug() << "waveData";
    gr->MultiPlot(1, 16, 0, 1, 1, "#");
    gr->SetRange('y', waveMin, waveMax);
    gr->Plot(waveData, "-G");

    qDebug() << "pitchData";
    gr->MultiPlot(1, 16, 3, 1, 6, "#");
    gr->SetRange('y', pitchMin, pitchMax);
    gr->Plot(pitchData, "-G6");
    gr->Axis("Y", "");
    gr->Grid("y", "W", "");

    qDebug() << "intensiveData";
    gr->MultiPlot(1, 16, 10, 1, 6, "#");
    gr->SetRange('y', 0.0, 1.0);
    gr->Plot(intensiveData, "-G6");
    gr->Axis("Y", "");
    gr->Grid("y", "W", "");

    if(!this->fileName.isEmpty()){
        qDebug() << "secWaveData";
        gr->MultiPlot(1, 16, 1, 1, 1, "#");
        gr->SetRange('y', _secWaveMin, _secWaveMax);
        gr->Plot(secWaveData, "R");

        qDebug() << "secPitchData";
        gr->MultiPlot(1, 16, 3, 1, 6, "#");
        gr->SetRange('y', _secPitchMin, _secPitchMax);
        gr->Plot(secPitchData, "-R6");

        qDebug() << "secPitchData";
        gr->MultiPlot(1, 16, 3, 1, 6, "#");
        gr->SetRange('y', _secPitchMin, _secPitchMax);
        gr->Plot(secPitchDataOrig, ".R1");

        qDebug() << "secIntensiveData";
        gr->MultiPlot(1, 16, 10, 1, 6, "#");
        gr->SetRange('y', 0.0, 1.0);
        gr->Plot(secIntensiveData, "-R6");
    }

    qDebug() << "finish drawing";
    return 0;
}

DrawerEval::~DrawerEval()
{
    qDebug() << "DrawerEval removed";
    freev(dsec_wave);
    freev(dsec_pitch);
    freev(dsec_intensive);
    freev(dsec_frame);
    freev(dsec_window);
}

void DrawerEval::Proc(QString fname)
{
    SPTK_SETTINGS * sptk_settings = SettingsDialog::getSPTKsettings();

//    freev(dsec_wave);
//    freev(dsec_pitch);
//    freev(dsec_intensive);
//    freev(dsec_frame);
//    freev(dsec_window);

    this->secFileName = fname;

    QFile file(this->secFileName);
    file.open(QIODevice::ReadOnly);
    WaveFile * waveFile = waveOpenHFile(file.handle());

    int size = littleEndianBytesToUInt32(waveFile->dataChunk->chunkDataSize);
    short int bits = littleEndianBytesToUInt16(waveFile->formatChunk->significantBitsPerSample);
    qDebug() << "secWaveOpenFile";
    dsec_wave = sptk_v2v(waveFile->dataChunk->waveformData, size, bits);
    qDebug() << "secSptk_v2v";
    dsec_pitch = sptk_pitch(dsec_wave, sptk_settings->pitch);
    qDebug() << "secSptk_pitch";
    dsec_frame = sptk_frame(dsec_wave, sptk_settings->frame);
    qDebug() << "secSptk_frame";
    dsec_intensive = sptk_intensive(dsec_frame, sptk_settings->frame);
    qDebug() << "secSptk_intensive";
    dsec_window = sptk_window(dsec_frame, sptk_settings->window);
    qDebug() << "secSptk_window";

    qDebug() << " wave:" << dsec_wave.x
             << " pitch:" << dsec_pitch.x
             << " intensive:" << dsec_intensive.x;
    qDebug() << "Start DP";
    VectorDP dp(d_pitch, dsec_pitch);
    qDebug() << "Apply DP";
    vector newPitch = dp.getScaledSignal();
    qDebug() << "Stop DP";

    secWaveData.Create(dsec_wave.x);
    secWaveDataLen = dsec_wave.x;
    for(long i=0;i<dsec_wave.x;i++)
    {
        double value = dsec_wave.v[i];
        secWaveData.a[i] = value;
        if(value > _secWaveMax) _secWaveMax = value;
        if(value < _secWaveMin) _secWaveMin = value;
    }
    qDebug() << "secWaveData Filled " << _secWaveMin << " " << _secWaveMax;

    secIntensiveData.Create(dsec_intensive.x);
    for(long i=0;i<dsec_intensive.x;i++)
    {
        if(dsec_intensive.v[i] == 0) secIntensiveData.a[i] = NAN;
        else
        {
            double value = dsec_intensive.v[i];
            secIntensiveData.a[i] = value;
        }
    }
    secIntensiveData.Norm();
    qDebug() << "secIntensiveData Filled";

    secPitchDataOrig.Create(dsec_pitch.x);
    for(long i=0;i<dsec_pitch.x;i++)
    {
        if(dsec_pitch.v[i] == 0) secPitchDataOrig.a[i] = NAN;
        else
        {
            double value = dsec_pitch.v[i];
            secPitchDataOrig.a[i] = value;
            if(value > _secPitchMax) _secPitchMax = value;
            if(value < _secPitchMin) _secPitchMin = value;
        }
    }
    qDebug() << "secPitchData Filled " << _secPitchMin << " " << _secPitchMax;

    secPitchData.Create(newPitch.x);
    for(long i=0;i<newPitch.x;i++)
    {
        if(newPitch.v[i] == 0) secPitchData.a[i] = NAN;
        else
        {
            double value = newPitch.v[i];
            secPitchData.a[i] = value;
            if(value > _secPitchMax) _secPitchMax = value;
            if(value < _secPitchMin) _secPitchMin = value;
        }
    }
    qDebug() << "secPitchData Filled " << _secPitchMin << " " << _secPitchMax;

    file.close();
    waveCloseFile(waveFile);
    qDebug() << "Drawer created";
}
