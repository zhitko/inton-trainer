#include <QDebug>
#include <QFile>
#include "drawer.h"
#include <stdlib.h>

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
    #include "./intensive/intensive.h"
}

GraphData ProcWave2Data(QString fname)
{
    SPTK_SETTINGS * sptk_settings = SettingsDialog::getSPTKsettings();

    GraphData data;

    QFile file(fname);
    file.open(QIODevice::ReadOnly);
    WaveFile * waveFile = waveOpenHFile(file.handle());

    int size = littleEndianBytesToUInt32(waveFile->dataChunk->chunkDataSize);
    short int bits = littleEndianBytesToUInt16(waveFile->formatChunk->significantBitsPerSample);
    qDebug() << "waveOpenFile";
    data.d_wave = sptk_v2v(waveFile->dataChunk->waveformData, size, bits);
    qDebug() << "sptk_v2v";
    data.d_frame = sptk_frame(data.d_wave, sptk_settings->frame);
    qDebug() << "sptk_frame";
    data.d_intensive = sptk_intensive(data.d_frame, sptk_settings->frame);
    qDebug() << "sptk_intensive";
    data.d_mid_intensive = sptk_mid_intensive(data.d_intensive, sptk_settings->energyFrame);
    qDebug() << "sptk_mid_intensive";
    data.d_window = sptk_window(data.d_frame, sptk_settings->window);
    qDebug() << "sptk_window";
    data.d_lpc = sptk_lpc(data.d_frame, sptk_settings->lpc);
    qDebug() << "sptk_lpc";
    data.d_spec = sptk_spec(data.d_lpc, sptk_settings->spec);
    qDebug() << "sptk_spec";
    data.d_pitch = sptk_pitch_spec(data.d_wave, sptk_settings->pitch, data.d_intensive.x);
    qDebug() << "sptk_pitch";
    PITCH_SETTINGS log_settings;
    memcpy(&log_settings, sptk_settings->pitch, sizeof(PITCH_SETTINGS));
    log_settings.OTYPE = 2;
    data.d_log = sptk_pitch_spec(data.d_wave, &log_settings, data.d_intensive.x);
    qDebug() << "d_log";

    file.close();
    waveCloseFile(waveFile);

    return data;
}

void freeGraphData(GraphData data)
{
    freev(data.d_frame);
    freev(data.d_intensive);
    freev(data.d_log);
    freev(data.d_mid_intensive);
    freev(data.d_lpc);
    freev(data.d_pitch);
    freev(data.d_spec);
    freev(data.d_wave);
    freev(data.d_window);
}

void vectorToData(vector vec, mglData * data)
{
    data->Create(vec.x);
    for(long i=0;i<vec.x;i++)
        data->a[i] = vec.v[i];
}

Drawer::Drawer() : mglDraw(),
    specMin(0.0), specMax(1.0),
    waveMin(DBL_MAX), pitchMin(DBL_MAX),
    waveMax(DBL_MIN), pitchMax(DBL_MIN),
    _waveMin(DBL_MAX), _pitchMin(DBL_MAX),
    _waveMax(DBL_MIN), _pitchMax(DBL_MIN),
    data(NULL), stereo(false)
{
}

Drawer::~Drawer()
{
//    freeGraphData(*data);
//    free(data);
    qDebug() << "Drawer removed";
}

void Drawer::Proc(QString fname)
{
    MathGLSettings * mathgl_settings = SettingsDialog::getMathGLSettings();
    SPTK_SETTINGS * sptk_settings = SettingsDialog::getSPTKsettings();

    this->fileName = fname;

    data = (GraphData*) malloc(sizeof(GraphData));
    *(data) = ProcWave2Data(this->fileName);
    data->d_pitch = sptk_fill_empty(data->d_pitch);

    vectorToData(data->d_wave, &waveData);
    waveDataLen = waveData.GetNx();
    _waveMin = waveMin = waveData.Min("x").a[0];
    _waveMax = waveMax = waveData.Max("x").a[0];
    qDebug() << "waveData Filled " << _waveMin << " " << _waveMax;

    vectorToData(data->d_intensive, &intensiveData);
    intensiveData.Norm(GRAPH_Y_VAL_MAX);
    qDebug() << "intensiveData Filled";

    vectorToData(data->d_log, &logData);
    logData.Norm(GRAPH_Y_VAL_MAX);
    qDebug() << "logData Filled";

    vectorToData(data->d_mid_intensive, &midIntensiveData);
    midIntensiveData.Norm(GRAPH_Y_VAL_MAX);
    qDebug() << "midIntensiveData Filled";

    vectorToData(data->d_pitch, &pitchData);
    _pitchMin = pitchData.Min("x").a[0];
    _pitchMax = pitchData.Max("x").a[0];
    pitchMin = sptk_settings->pitch->MIN_FREQ;
    pitchMax = sptk_settings->pitch->MAX_FREQ;
    qDebug() << "pitchData Filled " << pitchMin << " " << pitchMax;

    int speksize = sptk_settings->spec->leng / 2 + 1;
    int specX = data->d_spec.x/speksize;
    int specY = speksize;
    specData.Create(specX, specY);
    for(long j=0;j<specY;j++)
        for(long i=0;i<specX;i++)
        {
            long i0 = i+specX*j;
            long i1 = j+specY*i;
            specData.a[i0] = data->d_spec.v[i1];
        }
    specData.Squeeze(mathgl_settings->quality, 1);
    qDebug() << "specData Filled " << specX << " " << specY;

    this->specAuto();
    this->pitchAuto();
    qDebug() << "Data Processed";
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
