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
    #include "./others/func.h"
    #include "./others/interpolation.h"
}

GraphData ProcWave2Data(QString fname)
{
    SPTK_SETTINGS * sptk_settings = SettingsDialog::getSPTKsettings();

    QFile file(fname);
    file.open(QIODevice::ReadOnly);
    WaveFile * waveFile = waveOpenHFile(file.handle());

    int size = littleEndianBytesToUInt32(waveFile->dataChunk->chunkDataSize);
    short int bits = littleEndianBytesToUInt16(waveFile->formatChunk->significantBitsPerSample);
    qDebug() << "waveOpenFile";
    vector wave = sptk_v2v(waveFile->dataChunk->waveformData, size, bits);
    qDebug() << "sptk_v2v";
    vector frame = sptk_frame(wave, sptk_settings->frame);
    qDebug() << "sptk_frame";
    vector intensive = vector_intensive(frame, sptk_settings->frame);
    qDebug() << "vector_intensive";
    vector intensive_avg = vector_avg_intensive(intensive, sptk_settings->energyFrame);
    qDebug() << "vector_avg_intensive";
    vector window = sptk_window(frame, sptk_settings->window);
    qDebug() << "sptk_window";
    vector lpc = sptk_lpc(frame, sptk_settings->lpc);
    qDebug() << "sptk_lpc";
    vector spec = sptk_spec(lpc, sptk_settings->spec);
    qDebug() << "sptk_spec";
    vector pitch = sptk_pitch_spec(wave, sptk_settings->pitch, intensive.x);
    qDebug() << "sptk_pitch_spec";
    PITCH_SETTINGS log_settings;
    memcpy(&log_settings, sptk_settings->pitch, sizeof(PITCH_SETTINGS));
    log_settings.OTYPE = 2;
    vector logf0 = sptk_pitch_spec(wave, &log_settings, intensive.x);
    qDebug() << "sptk_pitch_spec log(f0)";

    vector mask = normalizev(logf0, 0.0, 1.0);
    qDebug() << "normalizev";
    vector pitch_cutted = vector_cut_by_mask(pitch, mask);
    qDebug() << "vector_cut_by_mask pitch";
    freev(pitch);
    vector intensive_cutted = vector_cut_by_mask(intensive, mask);
    qDebug() << "vector_cut_by_mask intensive";
    freev(intensive);
    vector inverted_mask = vector_invert_mask(mask);
    freev(mask);

    vector pitch_mid = vector_mid(pitch_cutted, sptk_settings->plot->midFrame);
    qDebug() << "vector_mid pitch";
    vector intensive_mid = vector_mid(intensive_cutted, sptk_settings->plot->midFrame);
    qDebug() << "vector_mid intensive";

    vector pitch_interpolate = vector_interpolate_by_mask(
                pitch_mid,
                inverted_mask,
                sptk_settings->plot->interpolation_edges,
                sptk_settings->plot->interpolation_type
                );
    freev(pitch_mid);

    vector intensive_interpolate = vector_interpolate_by_mask(
                intensive_mid,
                inverted_mask,
                sptk_settings->plot->interpolation_edges,
                sptk_settings->plot->interpolation_type
                );
    freev(intensive_mid);

    freev(inverted_mask);

    file.close();
    waveCloseFile(waveFile);

    GraphData data;

    data.d_wave = wave;
    data.d_pitch_originl = pitch_cutted;
    data.d_pitch = pitch_interpolate;
    data.d_log = logf0;
    data.d_intensive_original = intensive_cutted;
    data.d_intensive = intensive_interpolate;
    data.d_avg_intensive = intensive_avg;
    data.d_frame = frame;
    data.d_window = window;
    data.d_lpc = lpc;
    data.d_spec = spec;

    return data;
}

void freeGraphData(GraphData data)
{
    freev(data.d_frame);
    freev(data.d_intensive_original);
    freev(data.d_intensive);
    freev(data.d_log);
    freev(data.d_avg_intensive);
    freev(data.d_lpc);
    freev(data.d_pitch_originl);
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
    data->d_pitch = vector_fill_empty(data->d_pitch);

    vectorToData(data->d_wave, &waveData);
    waveDataLen = waveData.GetNx();
    _waveMin = waveMin = waveData.Min("x").a[0];
    _waveMax = waveMax = waveData.Max("x").a[0];
    qDebug() << "waveData Filled " << _waveMin << " " << _waveMax;

    vectorToData(data->d_intensive_original, &intensiveDataOriginal);
    intensiveData.Norm(GRAPH_Y_VAL_MAX);
    qDebug() << "intensiveData Filled";

    vectorToData(data->d_intensive, &intensiveData);
    intensiveData.Norm(GRAPH_Y_VAL_MAX);
    qDebug() << "intensiveData Filled";

    vectorToData(data->d_log, &logData);
    logData.Norm(GRAPH_Y_VAL_MAX);
    qDebug() << "logData Filled";

    vectorToData(data->d_avg_intensive, &midIntensiveData);
    midIntensiveData.Norm(GRAPH_Y_VAL_MAX);
    qDebug() << "midIntensiveData Filled";

    vectorToData(data->d_pitch_originl, &pitchDataOriginal);
    pitchDataOriginal.Norm(GRAPH_Y_VAL_MAX);
    vectorToData(data->d_pitch, &pitchData);
    pitchData.Norm(GRAPH_Y_VAL_MAX);
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
