#include <QDebug>
#include "drawer.h"

#include "settingsdialog.h"

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

Drawer::Drawer(QString fname):mglDraw(){
    SPTK_SETTINGS * sptk_settings = SettingsDialog::getSPTKsettings();
    MathGLSettings * mathgl_settings = SettingsDialog::getMathGLSettings();

    specMin = 0.0;
    specMax = 1.0;
    waveMin = pitchMin = DBL_MAX;
    waveMax = pitchMax = DBL_MIN;
    stereo = false;

    this->fileName = fname;

    WaveFile * waveFile = waveOpenFile(fileName.toLocal8Bit().data());

    int size = littleEndianBytesToUInt32(waveFile->dataChunk->chunkDataSize);
    short int bits = littleEndianBytesToUInt16(waveFile->formatChunk->significantBitsPerSample);
    qDebug() << "waveOpenFile";
    vector d_wave = sptk_v2v(waveFile->dataChunk->waveformData, size, bits);
    waveCloseFile(waveFile);
    qDebug() << "sptk_v2v";
    vector d_pitch = sptk_pitch(d_wave, sptk_settings->pitch);
    qDebug() << "sptk_pitch";
    vector d_frame = sptk_frame(d_wave, sptk_settings->frame);
    qDebug() << "sptk_frame";
    vector d_window = sptk_window(d_frame, sptk_settings->window);
    qDebug() << "sptk_window";
    vector d_lpc = sptk_lpc(d_frame, sptk_settings->lpc);
    qDebug() << "sptk_lpc";
    vector d_spec = sptk_spec(d_lpc, sptk_settings->spec);
    qDebug() << "sptk_spec";

    waveData.Create(d_wave.x);
    for(long i=0;i<d_wave.x;i++)
    {
        waveData.a[i] = d_wave.v[i];
    }
    waveMin = waveData.Min("x").a[0];
    waveMax = waveData.Max("x").a[0];
    qDebug() << "waveData Filled " << waveMin << " " << waveMax;

    pitchData.Create(d_pitch.x);
    for(long i=0;i<d_pitch.x;i++)
    {
        if(d_pitch.v[i] == 0)
            pitchData.a[i] = NAN;
        else
            pitchData.a[i] = d_pitch.v[i];
    }
    pitchMin = sptk_settings->pitch->min_freq;
    pitchMax = sptk_settings->pitch->max_freq;
    qDebug() << "pitchData Filled " << pitchMin << " " << pitchMax;

    int speksize = sptk_settings->spec->leng / 2 + 1;
    int specX = d_spec.x/speksize;
    int specY = speksize;
    int specZ = 256;
    specData.Create(specX, specY);
    for(long j=0;j<specY;j++)
        for(long i=0;i<specX;i++)
        {
            long i0 = i+specX*j;
            long i1 = j+specY*i;
            specData.a[i0] = d_spec.v[i1];
        }
    specData.Squeeze(mathgl_settings->quality, 1);
    qDebug() << "specData Filled " << specX << " " << specY << " " << specZ;

    freev(d_wave);
    freev(d_pitch);
    freev(d_frame);
    freev(d_window);
    freev(d_lpc);
    freev(d_spec);

    qDebug() << "Drawer created";
}

Drawer::~Drawer()
{
    qDebug() << "Drawer removed";
}

void Drawer::specAuto()
{
    specMin = 0.9;
    specMax = 1.0;
}

void Drawer::pitchAuto()
{
    pitchMin = pitchData.Min("xy").a[0];
    pitchMax = pitchData.Max("xy").a[0];
}

int Drawer::Draw(mglGraph *gr)
{
    qDebug() << "start drawing";

    gr->DefaultPlotParam();
    gr->Clf();

    qDebug() << "waveData";
    gr->MultiPlot(1, 16, 0, 1, 2, "#");
    gr->SetRange('y', waveMin, waveMax);
    gr->Plot(waveData);

    qDebug() << "pitchData";
    gr->MultiPlot(1, 16, 3, 1, 6, "#");
    gr->SetRange('y', pitchMin, pitchMax);
    gr->Plot(pitchData);
    gr->Axis("Y", "");
    gr->Grid("y", "W", "");

    qDebug() << "specData";
    specData.Norm(0, specMax);
    gr->MultiPlot(1, 16, 10, 1, 6, "#");
    if(stereo) gr->Rotate(50,60);
    QString colors = QString("w{w,%1}k").arg(QString::number(specMin));
    qDebug() << colors;
    gr->SetDefScheme(colors.toStdString().c_str());
    gr->Surf(specData);

    qDebug() << "finish drawing";
    return 0;
}

void Drawer::Reload()	{}		///< Function for reloading data
void Drawer::Click()	{}		///< Callback function on mouse click
