#include <QDebug>
#include <QFile>
#include "drawer.h"
#include <stdlib.h>
#include <cmath>
#include <limits>

#include "settingsdialog.h"

extern "C" {
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

vector calculateMask(vector wave, vector pitch)
{
    vector mask;
    qDebug() << "Use pitch log for mask" << LOG_DATA;

    SPTK_SETTINGS * sptk_settings = SettingsDialog::getSPTKsettings();
    PITCH_SETTINGS log_settings;
    memcpy(&log_settings, sptk_settings->pitch, sizeof(PITCH_SETTINGS));
    log_settings.OTYPE = 2;

    vector logf0 = sptk_pitch_spec(wave, &log_settings, pitch.x);
    qDebug() << "logf0" << LOG_DATA;

    mask = normalizev(logf0, 0.0, 1.0);
    qDebug() << "mask" << LOG_DATA;

    return mask;
}

vector scaleVectorByDPResults(vector data, SpectrDP* dp)
{
    int res_len = dp->getSignalSize();
    vector scaledData = vector_resize(data, res_len);
    VectorSignal signal(scaledData);
    vector new_data = ((VectorSignal*)dp->applyMask<double>(&signal))->getArray();
    freev(scaledData);
    return new_data;
}

vector getSignalWithMask(vector mask, SpectrDP* dp, vector signal)
{
    vector mask_new = scaleVectorByDPResults(mask, dp);
    vector mask_scalled = vector_resize(mask_new, signal.x);
    vector new_signal = zero_to_nan(vector_cut_by_mask(signal, mask_scalled));
    freev(mask_new);
    freev(mask_scalled);
    return new_signal;
}

mglData * createMglData(vector vec, mglData * data, bool nan)
{
    qDebug() << "createMglData " << vec.x << " " << LOG_DATA;
    if (data != NULL) {
        delete data;
        qDebug() << "delete data" << LOG_DATA;
    }
    if (vec.x > 0)
    {
        data = new mglData(vec.x, vec.v);
        qDebug() << "new mglData" << LOG_DATA;
    } else {
        double * tmp = new double[1];
        tmp[0] = 0.0;
        data = new mglData(1, tmp);
        qDebug() << "new mglData empty" << LOG_DATA;
        return data;
    }
    if (nan)
    {
        vectorToDataWithNan(vec, data);
        qDebug() << "vectorToDataWithNan" << LOG_DATA;
    } else {
        vectorToData(vec, data);
        qDebug() << "vectorToData" << LOG_DATA;
    }
    return data;
}

void vectorToData(vector vec, mglData * data)
{
    qDebug() << "vectorToData" << LOG_DATA;
    data->Create(vec.x);
    for(long i=0;i<vec.x;i++)
        data->a[i] = getv(vec, i);
}

void vectorToDataWithNan(vector vec, mglData * data)
{
    qDebug() << "vectorToDataWithNan " << vec.x << LOG_DATA;
    data->Create(vec.x);
    for(long i=0;i<vec.x;i++)
        if (getv(vec, i) != 0)
            data->a[i] = getv(vec, i);
        else data->a[i] = std::numeric_limits<double>::quiet_NaN();
}

Drawer::Drawer() : mglDraw(),
    data(NULL), stereo(false), showUMP(false)
{
    waveData = NULL;
    pitchData = NULL;
    pitchDataOriginal = NULL;
    specData = NULL;
    maskData = NULL;
    scaledMaskData = NULL;
    intensiveData = NULL;
    intensiveDataOriginal = NULL;
    pWaveData = NULL;
    nWaveData = NULL;
    tWaveData = NULL;
    proximity_curve_correlation = 0;
    proximity_curve_integral = 0;
    proximity_curve_local = 0;
    proximity_average = 0;
}

Drawer::~Drawer()
{
    if (this->waveData) delete this->waveData;
    if (this->pitchData) delete this->pitchData;
    if (this->pitchDataOriginal) delete this->pitchDataOriginal;
    if (this->specData) delete this->specData;
    if (this->maskData) delete this->maskData;
    if (this->scaledMaskData) delete this->scaledMaskData;
    if (this->intensiveData) delete this->intensiveData;
    if (this->intensiveDataOriginal) delete this->intensiveDataOriginal;
    if (this->pWaveData) delete this->pWaveData;
    if (this->nWaveData) delete this->nWaveData;
    if (this->tWaveData) delete this->tWaveData;
    if (data) freeGraphData(data);
    if (data) free(data);
    qDebug() << "Drawer removed" << LOG_DATA;
}

double Drawer::calcResultMark(vector v1, vector v2)
{
    vector v1norm = normalizev(v1, 0.0, 1.0);
    vector v2norm = normalizev(v2, 0.0, 1.0);
    qDebug() << "v1norm" << v1norm.x << LOG_DATA;
    qDebug() << "v2norm" << v2norm.x << LOG_DATA;
    double diff = 0;
    for(int i=0; i<v1norm.x && i<v1norm.x; i++)
    {
        double a = getv(v1norm, i);
        double b = getv(v2norm, i);
        diff += std::abs(a - b);
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
    data = ProcWave2Data(this->fileName);
    data->d_pitch = vector_fill_empty(data->d_pitch);

    waveData = createMglData(data->d_wave, waveData);
    waveDataLen = waveData->GetNx();
    qDebug() << "waveData Filled" << LOG_DATA;

    nWaveData = createMglData(data->d_n_wave, nWaveData);
    tWaveData = createMglData(data->d_t_wave, tWaveData);
    pWaveData = createMglData(data->d_p_wave, pWaveData);

    maskData = createMglData(data->d_mask, maskData);
    qDebug() << "maskData Filled" << LOG_DATA;

    intensiveDataOriginal = createMglData(data->d_intensive_original, intensiveDataOriginal, true);
    intensiveDataOriginal->Norm();
    qDebug() << "intensiveData Filled" << LOG_DATA;

    intensiveData = createMglData(data->d_intensive, intensiveData);
    intensiveData->Norm();
    qDebug() << "intensiveData Filled" << LOG_DATA;

    pitchDataOriginal = createMglData(data->d_pitch_original, pitchDataOriginal, true);
    pitchDataOriginal->Norm();
    pitchData = createMglData(data->d_pitch, pitchData);
    pitchData->Norm();
    qDebug() << "pitchData Filled" << LOG_DATA;

    int speksize = sptk_settings->spec->leng / 2 + 1;
    int specX = data->d_spec_proc.x/speksize;
    int specY = speksize;
    if (specData) delete specData;
    specData = new mglData();
    specData->Create(specX, specY);
    for(long j=0;j<specY;j++)
        for(long i=0;i<specX;i++)
        {
            long i0 = i+specX*j;
            long i1 = j+specY*i;
            specData->a[i0] = getv(data->d_spec_proc, i1);
        }
    specData->Squeeze(mathgl_settings->quality, 1);
    qDebug() << "specData Filled " << specX << " " << specY << LOG_DATA;

    qDebug() << "Data Processed" << LOG_DATA;
}

int Drawer::getDataLenght()
{
    return waveDataLen;
}

int Drawer::getDataSeconds()
{
    return 1.0 * waveDataLen / RECORD_FREQ;
}

int Drawer::Draw(mglGraph *gr)
{
    qDebug() << "start drawing" << LOG_DATA;

    gr->DefaultPlotParam();
    gr->Clf();

    qDebug() << "waveData" << LOG_DATA;
    gr->MultiPlot(1, 16, 0, 1, 2, "#");
    gr->SetRange('y', 0, 1);
    gr->Plot(*waveData);
    gr->Plot(*pWaveData, "y1");
    gr->Plot(*nWaveData, "q1");
    gr->Plot(*tWaveData, "c1");

    qDebug() << "pitchData " << data->pitch_min << " " << data->pitch_max << LOG_DATA;
    gr->MultiPlot(1, 16, 3, 1, 6, "#");
    gr->Puts(mglPoint(-0.9,1),QString("%1").arg(data->pitch_max).toLocal8Bit().data());
    gr->SetRange('y', 0, 1);
    gr->Plot(*pitchData, "-G6");
    gr->Axis("Y", "");
    gr->Grid("y", "W", "");
    gr->Puts(mglPoint(-0.9,-1),QString("%1").arg(data->pitch_min).toLocal8Bit().data());

    qDebug() << "scaledMaskData" << LOG_DATA;
    gr->SetRange('y', 0, 1);
    gr->Plot(*maskData, "-G1");

    qDebug() << "specData" << LOG_DATA;
    gr->MultiPlot(1, 16, 10, 1, 6, "#");
    if(stereo) gr->Rotate(50,60);
    QString colors = QString("w{w,%1}k").arg(QString::number(0));
    qDebug() << colors << LOG_DATA;
    gr->SetDefScheme(colors.toStdString().c_str());
    gr->Surf(*specData);

    qDebug() << "finish drawing" << LOG_DATA;
    return 0;
}

void Drawer::Reload()	{}		///< Function for reloading data
void Drawer::Click()	{}		///< Callback function on mouse click
