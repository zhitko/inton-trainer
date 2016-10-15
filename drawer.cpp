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

vector calculateMask(vector wave, vector pitch)
{
    vector mask;
    qDebug() << "Use pitch log for mask";

    SPTK_SETTINGS * sptk_settings = SettingsDialog::getSPTKsettings();
    PITCH_SETTINGS log_settings;
    memcpy(&log_settings, sptk_settings->pitch, sizeof(PITCH_SETTINGS));
    log_settings.OTYPE = 2;

    vector logf0 = sptk_pitch_spec(wave, &log_settings, pitch.x);
    qDebug() << "logf0";

    mask = normalizev(logf0, 0.0, 1.0);
    qDebug() << "mask";

    return mask;
}

vector readMaskFromFile(WaveFile* waveFile, int length, char marker)
{
    qDebug() << "Use file data for mask";

    vector mask_from_file;

    if (!((waveFile->cueChunk != NULL)
            && (littleEndianBytesToUInt16(waveFile->cueChunk->cuePointsCount) > 0)
            && (waveFile->listChunk != NULL)
            && (waveFile->listChunk->ltxtChunks != NULL)
            && (waveFile->listChunk->ltxtCount > 0)
            && (waveFile->listChunk->lablChunks != NULL)
            && (waveFile->listChunk->lablCount > 0)))
            return zerov(0);

    int cuePointsCount = littleEndianBytesToUInt16(waveFile->cueChunk->cuePointsCount);

    int *pointsFrom = (int*) malloc(sizeof(int)*cuePointsCount);
    int *pointsLength = (int*) malloc(sizeof(int)*cuePointsCount);


    qDebug() << "cueChunks";
    for(int i=0; i<cuePointsCount; i++)
    {
        CuePoint point = waveFile->cueChunk->cuePoints[i];
        int id = littleEndianBytesToUInt16(point.cuePointID);
        int pos = littleEndianBytesToUInt16(point.frameOffset);
        pointsFrom[id-1]=pos;
        pointsLength[id-1]=0;
        qDebug() << "cueChunk cuePointID " << id << " frameOffset " << pos;
    }

    qDebug() << "ltxtCount";
    for(int i=0; i<waveFile->listChunk->ltxtCount; i++)
    {
        LtxtChunk ltxt = waveFile->listChunk->ltxtChunks[i];
        int id = littleEndianBytesToUInt16(ltxt.cuePointID);
        int length = littleEndianBytesToUInt32(ltxt.sampleLength);
        pointsLength[id-1] = length;
        qDebug() << "ltxtChunk cuePointID " << id << " sampleLength " << length;
    }

    qDebug() << "lablChunks";
    if (marker != NULL)
    {
        int *markedPointsFrom = NULL;
        int *markedPointsLength = NULL;
        int count = 0;
        for(int i=0; i<waveFile->listChunk->lablCount; i++)
        {
            LablChunk labl = waveFile->listChunk->lablChunks[i];
            int id = littleEndianBytesToUInt16(labl.cuePointID);
            char * text = waveFile->listChunk->lablChunks[i].text;
            if (text[0] == marker)
            {
                count++;
                markedPointsFrom = (int*) realloc(markedPointsFrom, sizeof(int)*count);
                markedPointsLength = (int*) realloc(markedPointsLength, sizeof(int)*count);
                markedPointsFrom[count-1] = pointsFrom[id-1];
                markedPointsLength[count-1] = pointsLength[id-1];
            }
            qDebug() << "lablChunk cuePointID " << id << " text " << text[0];
        }
        free(pointsFrom);
        free(pointsLength);
        pointsFrom = markedPointsFrom;
        pointsLength = markedPointsLength;
        cuePointsCount = count;
    }

    mask_from_file = make_mask(length, cuePointsCount, pointsFrom, pointsLength);
    qDebug() << "make_mask";

    free(pointsFrom);
    free(pointsLength);

    return mask_from_file;
}

bool validateMask(vector mask)
{
    bool valid = false;
    for (int i=0; i<mask.x; i++)
    {
        if (mask.v[i] == 1)
        {
            valid = true;
        }
    }
    return valid;
}

vector getFileMask(WaveFile* waveFile, vector wave, vector pitch, char marker = NULL)
{
    vector mask;
    bool tryFileData = (waveFile->cueChunk != NULL)
            && (littleEndianBytesToUInt16(waveFile->cueChunk->cuePointsCount) > 0)
            && (waveFile->listChunk != NULL)
            && (waveFile->listChunk->ltxtChunks != NULL)
            && (waveFile->listChunk->ltxtCount > 0)
            && (waveFile->listChunk->lablChunks != NULL)
            && (waveFile->listChunk->lablCount > 0);

//    qDebug() << "waveFile->cueChunk " << (waveFile->cueChunk != NULL);
//    qDebug() << "waveFile->listChunk " << (waveFile->listChunk != NULL);
//    qDebug() << "waveFile->listChunk->ltxtCount " << (waveFile->listChunk->ltxtCount);
//    qDebug() << "waveFile->listChunk->lablCount " << (waveFile->listChunk->lablCount);

    if (tryFileData)
    {
        qDebug() << "tryFileData";
        vector mask_from_file = readMaskFromFile(waveFile, wave.x, marker);
        mask = vector_resize(mask_from_file, pitch.x);
        freev(mask_from_file);
        qDebug() << "vector_resize";
    }

    if (!tryFileData || !validateMask(mask)) {
        qDebug() << "!tryFileData";
//        mask = calculateMask(wave, pitch);
        mask = onesv(wave.x);
    }

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

vector processZeros(vector data)
{
    bool needProcess = false;
    int start = 0;
    int end = 0;
    for (int i=0; i<data.x; i++)
    {
        end = i - 1;
        if (data.v[i] != 0 && needProcess)
        {
            needProcess = false;
            for (int j=end; j>=start; j--)
            {
                data.v[j] = data.v[i];
            }
        }
        else if (data.v[i] == 0 && !needProcess)
        {
            start = i;
            needProcess = true;
        }
    }
    if (needProcess)
    {
        end++;
        for (int i=start; i<=end; i++)
        {
            data.v[i] = data.v[start-1];
        }
    }
    return data;
}

GraphData ProcWave2Data(QString fname)
{
    SPTK_SETTINGS * sptk_settings = SettingsDialog::getSPTKsettings();

    QFile file(fname);
    file.open(QIODevice::ReadOnly);
    WaveFile * waveFile = waveOpenHFile(file.handle());
    qDebug() << "waveOpenFile";

    int size = littleEndianBytesToUInt32(waveFile->dataChunk->chunkDataSize);
    qDebug() << "chunkDataSize";
    short int bits = littleEndianBytesToUInt16(waveFile->formatChunk->significantBitsPerSample);
    qDebug() << "significantBitsPerSample";

    vector wave = sptk_v2v(waveFile->dataChunk->waveformData, size, bits);
    qDebug() << "wave";

    vector frame = sptk_frame(wave, sptk_settings->frame);
    qDebug() << "frame";

    vector intensive = vector_intensive(frame, sptk_settings->frame);
    qDebug() << "intensive";

    vector intensive_avg = vector_avg_intensive(intensive, sptk_settings->energyFrame);
    qDebug() << "intensive_avg";

    vector window = sptk_window(frame, sptk_settings->window);
    qDebug() << "window";

    vector lpc = sptk_lpc(frame, sptk_settings->lpc);
    qDebug() << "lpc";

    vector spec = sptk_spec(lpc, sptk_settings->spec);
    qDebug() << "spec";

    vector spec_exp = vector_pow_exp(spec, sptk_settings->spec->factor, sptk_settings->spec->min);
    qDebug() << "spec_log";

    vector pitch = processZeros(sptk_pitch_spec(wave, sptk_settings->pitch, intensive.x));
    qDebug() << "pitch";

    vector mask = getFileMask(waveFile, wave, pitch);
    qDebug() << "mask";

    vector pitch_cutted = processZeros(vector_cut_by_mask(pitch, mask));
    qDebug() << "pitch_cutted";
    double pitch_min = pitch_cutted.v[minv(pitch_cutted)];
    double pitch_max = pitch_cutted.v[maxv(pitch_cutted)];

    vector intensive_cutted = vector_cut_by_mask(intensive, mask);
    qDebug() << "intensive_cutted";

    vector inverted_mask = vector_invert_mask(mask);
    qDebug() << "inverted_mask";

    vector pitch_interpolate = vector_interpolate_by_mask(
                pitch_cutted,
                inverted_mask,
                sptk_settings->plotF0->interpolation_edges,
                sptk_settings->plotF0->interpolation_type
                );
    qDebug() << "pitch_interpolate";

    vector intensive_interpolate = vector_interpolate_by_mask(
                intensive_cutted,
                inverted_mask,
                sptk_settings->plotEnergy->interpolation_edges,
                sptk_settings->plotEnergy->interpolation_type
                );
    qDebug() << "intensive_interpolate";

    vector pitch_mid = vector_mid(pitch_interpolate, sptk_settings->plotF0->midFrame);
    qDebug() << "pitch_mid";

    vector intensive_mid = vector_mid(intensive_interpolate, sptk_settings->plotEnergy->midFrame);
    qDebug() << "intensive_mid";

    vector norm_wave = normalizev(wave, 0.0, 1.0);

    vector p_mask = readMaskFromFile(waveFile, wave.x, 'P');
    qDebug() << "p_mask";

    vector n_mask = readMaskFromFile(waveFile, wave.x, 'N');
    qDebug() << "n_mask";

    vector t_mask = readMaskFromFile(waveFile, wave.x, 'T');
    qDebug() << "t_mask";

    vector p_wave = zero_to_nan(vector_cut_by_mask(norm_wave, p_mask));
    qDebug() << "p_mask";

    vector n_wave = zero_to_nan(vector_cut_by_mask(norm_wave, n_mask));
    qDebug() << "n_mask";

    vector t_wave = zero_to_nan(vector_cut_by_mask(norm_wave, t_mask));
    qDebug() << "t_mask";

    vector pnt_mask = onesv(norm_wave.x);
    for (int i=0; i<p_mask.x && i<n_mask.x && i<t_mask.x && i<norm_wave.x; i++) {
        if (p_mask.v[i] == 1 || n_mask.v[i] == 1 || t_mask.v[i] == 1)
        {
            pnt_mask.v[i] = 0;
        } else {
            pnt_mask.v[i] = 1;
        }
    }

    vector display_wave = zero_to_nan(vector_cut_by_mask(norm_wave, pnt_mask));

    freev(frame);
    freev(window);
    freev(lpc);
    freev(pitch_cutted);
    freev(intensive_cutted);
    freev(inverted_mask);
    freev(pitch_interpolate);
    freev(intensive_interpolate);
    freev(wave);
    qDebug() << "freev";

    file.close();
    waveCloseFile(waveFile);
    qDebug() << "waveCloseFile";

    GraphData data;

    data.d_full_wave = norm_wave;
    data.d_wave = display_wave;
    data.d_p_wave = p_wave;
    data.d_n_wave = n_wave;
    data.d_t_wave = t_wave;
    data.d_pitch_originl = pitch;
    data.d_pitch = pitch_mid;
    data.pitch_max = pitch_max;
    data.pitch_min = pitch_min;
    data.d_intensive_original = intensive;
    data.d_intensive = intensive_mid;
    data.d_avg_intensive = intensive_avg;
    data.d_spec_exp = spec_exp;
    data.d_spec = spec;
    data.d_mask = mask;
    data.p_mask = p_mask;
    data.n_mask = n_mask;
    data.t_mask = t_mask;
    data.pnt_mask = pnt_mask;

    return data;
}

void freeGraphData(GraphData data)
{
    freev(data.d_intensive_original);
    freev(data.d_intensive);
    freev(data.d_avg_intensive);
    freev(data.d_pitch_originl);
    freev(data.d_pitch);
    freev(data.d_spec);
    freev(data.d_spec_exp);
    freev(data.d_wave);
    freev(data.d_full_wave);
    freev(data.d_mask);
    freev(data.d_p_wave);
    freev(data.d_t_wave);
    freev(data.d_n_wave);
    freev(data.p_mask);
    freev(data.n_mask);
    freev(data.t_mask);
    freev(data.pnt_mask);
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
    qDebug() << "waveData Filled";

    vectorToData(data->d_n_wave, &nWaveData);
    vectorToData(data->d_t_wave, &tWaveData);
    vectorToData(data->d_p_wave, &pWaveData);

    vectorToData(data->d_mask, &maskData);
    qDebug() << "maskData Filled";

    vectorToDataWithNan(data->d_intensive_original, &intensiveDataOriginal);
    intensiveDataOriginal.Norm(GRAPH_Y_VAL_MAX);
    qDebug() << "intensiveData Filled";

    vectorToData(data->d_intensive, &intensiveData);
    intensiveData.Norm(GRAPH_Y_VAL_MAX);
    qDebug() << "intensiveData Filled";

    vectorToData(data->d_avg_intensive, &midIntensiveData);
    midIntensiveData.Norm(GRAPH_Y_VAL_MAX);
    qDebug() << "midIntensiveData Filled";

    vectorToDataWithNan(data->d_pitch_originl, &pitchDataOriginal);
    pitchDataOriginal.Norm(GRAPH_Y_VAL_MAX);
    vectorToData(data->d_pitch, &pitchData);
    pitchData.Norm(GRAPH_Y_VAL_MAX);
    qDebug() << "pitchData Filled";

    int speksize = sptk_settings->spec->leng / 2 + 1;
    int specX = data->d_spec_exp.x/speksize;
    int specY = speksize;
    specData.Create(specX, specY);
    for(long j=0;j<specY;j++)
        for(long i=0;i<specX;i++)
        {
            long i0 = i+specX*j;
            long i1 = j+specY*i;
            specData.a[i0] = data->d_spec_exp.v[i1];
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
    gr->SetRange('y', 0, 1);
    gr->Plot(waveData);
    gr->Plot(pWaveData, "y1");
    gr->Plot(nWaveData, "q1");
    gr->Plot(tWaveData, "c1");

    qDebug() << "pitchData " << data->pitch_min << " " << data->pitch_max;
    gr->MultiPlot(1, 16, 3, 1, 6, "#");
    gr->Puts(mglPoint(-0.9,1),QString("%1").arg(data->pitch_max).toLocal8Bit().data());
    gr->SetRange('y', 0, GRAPH_Y_VAL_MAX);
    gr->Plot(pitchData, "-G6");
    gr->Axis("Y", "");
    gr->Grid("y", "W", "");
    gr->Puts(mglPoint(-0.9,-1),QString("%1").arg(data->pitch_min).toLocal8Bit().data());

    qDebug() << "scaledMaskData";
    gr->MultiPlot(1, 16, 3, 1, 6, "#");
    gr->SetRange('y', 0, 1);
    gr->Plot(maskData, "-G1");
    gr->Axis("Y", "");
    gr->Grid("y", "W", "");

    qDebug() << "specData";
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
