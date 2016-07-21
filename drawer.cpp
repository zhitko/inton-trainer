#include <QDebug>
#include <QFile>
#include "drawer.h"
#include <stdlib.h>
#include <cmath>
#include <limits>

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
    vector intensive_cutted = vector_cut_by_mask(intensive, mask);
    qDebug() << "vector_cut_by_mask intensive";
    vector inverted_mask = vector_invert_mask(mask);
    freev(mask);

    vector pitch_interpolate = vector_interpolate_by_mask(
                pitch_cutted,
                inverted_mask,
                sptk_settings->plotF0->interpolation_edges,
                sptk_settings->plotF0->interpolation_type
                );
    freev(pitch_cutted);

    vector intensive_interpolate = vector_interpolate_by_mask(
                intensive_cutted,
                inverted_mask,
                sptk_settings->plotEnergy->interpolation_edges,
                sptk_settings->plotEnergy->interpolation_type
                );
    freev(intensive_cutted);

    vector pitch_mid = vector_mid(pitch_interpolate, sptk_settings->plotF0->midFrame);
    freev(pitch_interpolate);
    qDebug() << "vector_mid pitch";
    vector intensive_mid = vector_mid(intensive_interpolate, sptk_settings->plotEnergy->midFrame);
    freev(intensive_interpolate);
    qDebug() << "vector_mid intensive";

    freev(inverted_mask);

    file.close();
    waveCloseFile(waveFile);

    GraphData data;

    data.d_wave = wave;
    data.d_pitch_originl = pitch;
    data.d_pitch = pitch_mid;
    data.d_log = logf0;
    data.d_intensive_original = intensive;
    data.d_intensive = intensive_mid;
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

void vectorToDataWithNan(vector vec, mglData * data)
{
    data->Create(vec.x);
    for(long i=0;i<vec.x;i++)
        if (vec.v[i] != 0)
            data->a[i] = vec.v[i];
        else data->a[i] = std::numeric_limits<double>::quiet_NaN();
}

Drawer::Drawer() : mglDraw(),
    data(NULL), stereo(false)
{
}

Drawer::~Drawer()
{
    freeGraphData(*data);
    free(data);
    qDebug() << "Drawer removed";
}

double Drawer::calcResultMark(vector v1, vector v2, int errors)
{
    vector v1norm = normalizev(v1, 0.0, 1.0);
    vector v2norm = normalizev(v2, 0.0, 1.0);
    qDebug() << "v1norm" << v1norm.x;
    qDebug() << "v2norm" << v2norm.x;
    double diff = 0;
    for(int i=0; i<v1norm.x && i<v1norm.x; i++)
    {
        diff += std::abs(v1norm.v[i] - v2norm.v[i]);
//        qDebug() << "diff" << diff;
//        qDebug() << "v1norm" << v1norm.v[i];
//        qDebug() << "v2norm" << v2norm.v[i];
    }
    diff /= v1norm.x;
    freev(v1norm);
    freev(v2norm);
    return 100 - diff * 100;
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
    waveData.Norm(GRAPH_Y_VAL_MAX);
    qDebug() << "waveData Filled";

    vectorToDataWithNan(data->d_intensive_original, &intensiveDataOriginal);
    intensiveDataOriginal.Norm(GRAPH_Y_VAL_MAX);
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

    vectorToDataWithNan(data->d_pitch_originl, &pitchDataOriginal);
    pitchDataOriginal.Norm(GRAPH_Y_VAL_MAX);
    vectorToData(data->d_pitch, &pitchData);
    pitchData.Norm(GRAPH_Y_VAL_MAX);
    qDebug() << "pitchData Filled";

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

    qDebug() << "Data Processed";
}

int Drawer::getDataLenght()
{
    return waveDataLen;
}

int Drawer::Draw(mglGraph *gr)
{
    qDebug() << "start drawing";

    gr->DefaultPlotParam();
    gr->Clf();

    qDebug() << "waveData";
    gr->MultiPlot(1, 16, 0, 1, 2, "#");
    gr->SetRange('y', 0, GRAPH_Y_VAL_MAX);
    gr->Plot(waveData);

    qDebug() << "pitchData";
    gr->MultiPlot(1, 16, 3, 1, 6, "#");
    gr->SetRange('y', 0, GRAPH_Y_VAL_MAX);
    gr->Plot(pitchData, "-G6");
    gr->Axis("Y", "");
    gr->Grid("y", "W", "");

    qDebug() << "specData";
    specData.Norm(0, GRAPH_Y_VAL_MAX);
    gr->MultiPlot(1, 16, 10, 1, 6, "#");
    if(stereo) gr->Rotate(50,60);
    QString colors = QString("w{w,%1}k").arg(QString::number(0));
    qDebug() << colors;
    gr->SetDefScheme(colors.toStdString().c_str());
    gr->Surf(specData);

    qDebug() << "finish drawing";
    return 0;
}

void Drawer::Reload()	{}		///< Function for reloading data
void Drawer::Click()	{}		///< Callback function on mouse click
