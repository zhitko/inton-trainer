#include <QDebug>
#include <QFile>
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

    vector sptk_intensive(vector data, FRAME_SETTINGS * settings)
    {
        int frameLength = settings->leng,
            resultLength = data.x / frameLength;
        vector result = makev(resultLength);

        double middle = 0.0;
        for(int i=0;i<resultLength;i++)
        {
            middle = 0.0;
            for(int j=(i*frameLength);j<((i+1)*frameLength);j++)
                middle += fabs(data.v[j]);
            middle /= frameLength;
            result.v[i] = middle;
        }

        return result;
    }
}

Drawer::Drawer(QString fname) : mglDraw(),
    specMin(0.0), specMax(1.0),
    waveMin(DBL_MAX), pitchMin(DBL_MAX),
    waveMax(DBL_MIN), pitchMax(DBL_MIN),
    _waveMin(DBL_MAX), _pitchMin(DBL_MAX),
    _waveMax(DBL_MIN), _pitchMax(DBL_MIN),
    stereo(false), fileName(fname)
{
    SPTK_SETTINGS * sptk_settings = SettingsDialog::getSPTKsettings();
    MathGLSettings * mathgl_settings = SettingsDialog::getMathGLSettings();

    QFile file(this->fileName);
    file.open(QIODevice::ReadOnly);
    WaveFile * waveFile = waveOpenHFile(file.handle());

    int size = littleEndianBytesToUInt32(waveFile->dataChunk->chunkDataSize);
    short int bits = littleEndianBytesToUInt16(waveFile->formatChunk->significantBitsPerSample);
    qDebug() << "waveOpenFile";
    d_wave = sptk_v2v(waveFile->dataChunk->waveformData, size, bits);
    qDebug() << "sptk_v2v";
    d_pitch = sptk_pitch(d_wave, sptk_settings->pitch);
    qDebug() << "sptk_pitch";
    d_frame = sptk_frame(d_wave, sptk_settings->frame);
    qDebug() << "sptk_frame";
    d_intensive = sptk_intensive(d_frame, sptk_settings->frame);
    qDebug() << "sptk_intensive";
    d_window = sptk_window(d_frame, sptk_settings->window);
    qDebug() << "sptk_window";
    d_lpc = sptk_lpc(d_frame, sptk_settings->lpc);
    qDebug() << "sptk_lpc";
    d_spec = sptk_spec(d_lpc, sptk_settings->spec);
    qDebug() << "sptk_spec";

    waveData.Create(d_wave.x);
    waveDataLen = d_wave.x;
    for(long i=0;i<d_wave.x;i++)
    {
        double value = d_wave.v[i];
        waveData.a[i] = value;
        if(value > _waveMax) _waveMax = value;
        if(value < _waveMin) _waveMin = value;
    }
    waveMin = _waveMin;
    waveMax = _waveMax;
    qDebug() << "waveData Filled " << _waveMin << " " << _waveMax;

    intensiveData.Create(d_intensive.x);
    for(long i=0;i<d_intensive.x;i++)
    {
        if(d_intensive.v[i] == 0) intensiveData.a[i] = NAN;
        else
        {
            double value = d_intensive.v[i];
            intensiveData.a[i] = value;
        }
    }
    intensiveData.Norm();
    qDebug() << "intensiveData Filled";

    pitchData.Create(d_pitch.x);
    for(long i=0;i<d_pitch.x;i++)
    {
        if(d_pitch.v[i] == 0) pitchData.a[i] = NAN;
        else
        {
            double value = d_pitch.v[i];
            pitchData.a[i] = value;
            if(value > _pitchMax) _pitchMax = value;
            if(value < _pitchMin) _pitchMin = value;
        }
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

    file.close();
    waveCloseFile(waveFile);
    qDebug() << "Drawer created";

    this->specAuto();
    this->pitchAuto();
}

Drawer::~Drawer()
{
    qDebug() << "Drawer removed";
    freev(d_wave);
    freev(d_pitch);
    freev(d_intensive);
    freev(d_frame);
    freev(d_window);
    freev(d_lpc);
    freev(d_spec);
}

int Drawer::getDataLenght()
{
    return waveDataLen;
}

void Drawer::specAuto()
{
    specMin = 0.8;
    specMax = 1.0;
}

void Drawer::pitchAuto()
{
    pitchMin = _pitchMin;
    pitchMax = _pitchMax;
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
    gr->Plot(pitchData, "-G6");
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
