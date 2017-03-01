#include "drawerdp.h"

#include <QFile>
#include <QDebug>

#include "settingsdialog.h"
#include <QApplication>
#include "DP/continuousdp.h"
#define DATA_PATH_TRAINING "/data/training/"

extern "C" {
    #include "./OpenAL/wavFile.h"

    #include "float.h"

    #include "./SPTK/SPTK.h"
    #include "./SPTK/others/func.h"
    #include "./SPTK/others/interpolation.h"
    #include "./SPTK/pitch/pitch.h"
    #include "./SPTK/x2x/x2x.h"
    #include "./SPTK/frame/frame.h"
    #include "./SPTK/window/window.h"
    #include "./SPTK/lpc/lpc.h"
    #include "./SPTK/spec/spec.h"
}

typedef struct { double min; double max; } MinMax;

DrawerDP::DrawerDP() :
    Drawer(),
    secFileName(""),
    first(true)
{
    this->dpData = NULL;
    this->secWaveData = NULL;
    this->errorData = NULL;
    this->timeData = NULL;
    this->pSecData = NULL;
    this->nSecData = NULL;
    this->tSecData = NULL;
    this->secPitchData = NULL;
    this->secIntensiveData = NULL;
    this->errorMax = 0;
    this->errorMin = 0;
    this->result = 0.0;
    this->f0max = 0;
    this->f0mix = 0;
    this->of0max = 0;
    this->of0mix = 0;
}

DrawerDP::~DrawerDP()
{
    qDebug() << "DrawerDP removed";
    if (this->dpData) delete this->dpData;
    if (this->secWaveData) delete this->secWaveData;
    if (this->errorData) delete this->errorData;
    if (this->timeData) delete this->timeData;
    if (this->pSecData) delete this->pSecData;
    if (this->nSecData) delete this->nSecData;
    if (this->tSecData) delete this->tSecData;
    if (this->secPitchData) delete this->secPitchData;
    if (this->secIntensiveData) delete this->secIntensiveData;
    if (this->simple_data) freeSimpleGraphData(this->simple_data);
    if (this->simple_data) delete this->simple_data;
}

int DrawerDP::Draw(mglGraph *gr)
{
    SPTK_SETTINGS * sptk_settings = SettingsDialog::getSPTKsettings();
    qDebug() << "start drawing";

    bool isCompare = !this->secFileName.isEmpty();

    gr->DefaultPlotParam();
    gr->Clf();

    qDebug() << "waveData";
    gr->MultiPlot(1, 15, 1, 1, 1, "#");
    gr->SetRange('y', 0, 1);
    gr->Plot(*waveData, "B");
    gr->Plot(*pWaveData, "y9");
    gr->Plot(*nWaveData, "q9");
    gr->Plot(*tWaveData, "c9");

    gr->MultiPlot(1, 15, 6, 1, 6, "#");
    gr->SetRange('y', 0, 1);
    gr->Axis("Y", "");
    gr->Grid("y", "W", "");
    if(sptk_settings->dp->showF0)
    {
        gr->Plot(*this->pitchData, "-r3");
        if(!isCompare){
            if(this->pitchDataOriginal) gr->Plot(*this->pitchDataOriginal, "-r2");
        }
    }
    if(sptk_settings->dp->showA0) gr->Plot(*this->intensiveData, "-g3");
    gr->Plot(*pWaveData, "y2");
    gr->Plot(*nWaveData, "q2");
    gr->Plot(*tWaveData, "c2");

    gr->MultiPlot(1, 15, 12, 1, 1, "#");
    QString path = QApplication::applicationDirPath() + DATA_PATH_TRAINING;
    gr->Puts(mglPoint(0,0),fileName.replace(path,"").replace(".wav","").replace("/"," - ").toLocal8Bit().data(), ":C", 24);

    gr->MultiPlot(1, 15, 13, 1, 1, "#");
    gr->Puts(
        mglPoint(0,0),
        QString("F0 min = %1 max = %2").arg(this->f0mix).arg(this->f0max).toLocal8Bit().data(),
        ":C",
        24
    );

    if(isCompare){
        qDebug() << "secWaveData";
        gr->MultiPlot(1, 15, 3, 1, 1, "#");
        gr->SetRange('y', 0, 1);
        gr->Plot(*pSecData, "y9");
        gr->Plot(*nSecData, "q9");
        gr->Plot(*tSecData, "c9");
        gr->Plot(*secWaveData, "G");
        gr->Plot(*dpData, "R9");

        gr->MultiPlot(1, 15, 5, 1, 1, "#");
        gr->Puts(mglPoint(0,0),QString("%1%").arg(this->result).toLocal8Bit().data(), ":C", 24);

        qDebug() << "errorData";
        gr->MultiPlot(1, 15, 6, 1, 6, "#");
        gr->SetRange('y', 0, 1);
        if(this->errorData) gr->Plot(*this->errorData, "-B3");
        if(this->timeData) gr->Plot(*this->timeData, "-R3");
        if(sptk_settings->dp->showF0) gr->Plot(*this->secPitchData, "-R3");
        if(sptk_settings->dp->showA0) gr->Plot(*this->secIntensiveData, "-G3");

        gr->MultiPlot(3, 15, 39, 1, 1, "#");
        gr->Puts(
            mglPoint(0,0),
            QString("Error min = %1 max = %2").arg(this->errorMin).arg(this->errorMax).toLocal8Bit().data(),
            ":C",
            24
        );
    }

    qDebug() << "finish drawing";
    return 0;
}

void setMark(vector * vec, int pos)
{
    if (vec->x > pos)
    {
        vec->v[pos] = 1.0;
        if (pos != 0)
        {
            vec->v[pos-1] = 0.0001;
        }
        if (vec->x != pos + 1)
        {
            vec->v[pos+1] = 0.0001;
        }
    } else {
        qDebug() << "WARNING setMark ? " << (vec->x > pos) ;
        qDebug() << "WARNING setMark x " << vec->x ;
        qDebug() << "WARNING setMark pos " << pos ;
    }
}

void setMark(vector * vec, int from, int to)
{
    int i=from;
    for (i=from; i<to; i++)
    {
        vec->v[i] = 1.0;
    }
    vec->v[from] = 0.0001;
    vec->v[i] = 0.0001;
}

int getMappingValue(intvector* mapping, int index)
{
    if (index > mapping->x)
    {
        return mapping->v[mapping->x - 1];
    } else {
        return mapping->v[index];
    }
}

void applyMapping(vector * data, intvector * mapping)
{
    qDebug() << "data " << data->x;
    qDebug() << "mapping " << mapping->x;
    double scale = 1.0 * data->x / mapping->x;
    qDebug() << "scale " << scale;
    vector newData = zerov(data->x);
    for (int i=0; i<newData.x; i++)
    {
        int j = 1.0 * i / scale;
        int index = getMappingValue(mapping, j);
//        qDebug() << "index " << i << " " << j << " " << index;
        newData.v[i] = getv(*data, index);
    }
    freev(*data);
    data->v = newData.v;
}

vector norm(vector data, double targetMin, double targetMax, bool procZeros = true)
{
    vector result = makev(data.x);
    double sourceMin = 0.0;
    if (procZeros){
        sourceMin = data.v[minv(data)];
    } else {
        sourceMin = data.v[min_greaterv(data, 0.0)];
    }
    double sourceMax = data.v[maxv(data)];
    qDebug() << "sourceMin " << sourceMin;
    qDebug() << "sourceMax " << sourceMax;

    double sourceScale = sourceMax - sourceMin;
    double targetScale = targetMax - targetMin;

    double zsrc, scaled;

    for(int i=0;i<data.x;i++)
        if (procZeros || data.v[i] != 0)
        {
            zsrc = data.v[i] - sourceMin;
            scaled = zsrc * targetScale / sourceScale;
            result.v[i] = scaled + targetMin;
        }
    return result;
}

int compare (const void * a, const void * b)
{
  return ( *(double*)a - *(double*)b );
}

vector mid(vector data, int frame, bool procZeros = true)
{
    int resultLength = data.x;
    vector result = zerov(resultLength);
    int frameIndex = 0;
    vector middle = zerov(frame);
    for(int i=0;i<resultLength;i++)
    {
        frameIndex = 0;
        for(int j=0; j < frame; j++)
        {
            int position = i+j-frame/2;
            if (procZeros || data.v[i] != 0)
                {
                if( position < 0 || position > resultLength )
                    middle.v[frameIndex] = 0.0;
                else {
                    middle.v[frameIndex] = fabs(data.v[position]);
                    frameIndex++;
                }
            }
        }
        if (frameIndex > frame/3)
        {
            qsort (middle.v, frameIndex-1, sizeof(double), compare);
            result.v[i] = middle.v[frameIndex/2];
        }
    }
    freev(middle);

    return result;
}

MinMax applyMask(vector * data, vector * mask)
{
    double min, max;
    SPTK_SETTINGS * sptk_settings = SettingsDialog::getSPTKsettings();

    vector scaledMask = vector_resize(*mask, data->x);
    qDebug() << "scaledMask";
    vector cuttedData = vector_cut_by_mask(*data, scaledMask);
    qDebug() << "newData";
    min = cuttedData.v[min_greaterv(cuttedData, 0.0)];
    max = cuttedData.v[maxv(cuttedData)];
    vector dataMid = mid(cuttedData, sptk_settings->plotF0->midFrame);
    qDebug() << "pitch_mid";
    vector newDataNorm = norm(dataMid, 0.0, 1.0, !sptk_settings->plotF0->normF0MinMax);
    qDebug() << "newDataNorm";    
    vector dataInterpolate = copyv(newDataNorm);
    qDebug() << "dataInterpolate " << dataInterpolate.x;

    int start = first_fromv(dataInterpolate, 0, 0.0);
    int end = first_greater_fromv(dataInterpolate, start, 0.0);

    if (start == 0)
    {
        try {
            vector_interpolate_part(
                        &dataInterpolate,
                        start,
                        end,
                        sptk_settings->plotF0->interpolation_type
            );
        } catch(int err){ qDebug() << "Error " << err; }

        start = first_fromv(dataInterpolate, end, 0.0);
        end = first_greater_fromv(dataInterpolate, start, 0.0);
    }

    do
    {
        qDebug() << start << " " << end;
        try {
            vector_interpolate_part(
                        &dataInterpolate,
                        start-1,
                        end,
                        sptk_settings->plotF0->interpolation_type
            );
        } catch(int err){ qDebug() << "Error " << err; }
        start = first_fromv(dataInterpolate, end, 0.0);
        end = first_greater_fromv(dataInterpolate, start, 0.0);
    } while (end != start && end != dataInterpolate.x);

    try {
        vector_interpolate_part(
                    &dataInterpolate,
                    start-1,
                    dataInterpolate.x - 1,
                    sptk_settings->plotF0->interpolation_type
        );
    } catch(int err){ qDebug() << "Error " << err; }

    freev(scaledMask);
    freev(cuttedData);
    freev(dataMid);
    freev(newDataNorm);
    freev(*data);
    data->v = newDataNorm.v;
    data->v = dataInterpolate.v;

    return MinMax{min, max};
}

void getMark(vector * vec, MaskData * points, int startPos, double marksScale, intvector * mapping)
{
    for (int i=0; i<points->pointsFrom.x; i++)
    {
        int from = getMappingValue(mapping, 1.0*(points->pointsFrom.v[i])/marksScale) + startPos;
        int to = getMappingValue(mapping, 1.0*(points->pointsFrom.v[i] + points->pointsLength.v[i])/marksScale) + startPos;
        setMark(vec, from, to);
        qDebug() << "setMark pSecVector " << from << " - " << to;
    }
}

void getMark(vector * vec, MaskData * points)
{
    for (int i=0; i<points->pointsFrom.x; i++)
    {
        int from = points->pointsFrom.v[i];
        int to = points->pointsFrom.v[i] + points->pointsLength.v[i];
        setMark(vec, from, to);
    }
}

void DrawerDP::Proc(QString fname)
{
    SPTK_SETTINGS * sptk_settings = SettingsDialog::getSPTKsettings();

    if(first)
    {
        qDebug() << "Drawer::Proc";
//        Drawer::Proc(fname);
        first = false;

        this->fileName = fname;

        this->simple_data = SimpleProcWave2Data(this->fileName);

        this->waveData = createMglData(this->simple_data->d_full_wave, this->waveData);
        qDebug() << "waveData Filled";

        vector pVector = zerov(this->simple_data->d_full_wave.x);
        vector nVector = zerov(this->simple_data->d_full_wave.x);
        vector tVector = zerov(this->simple_data->d_full_wave.x);

        getMark(&pVector, &this->simple_data->md_p);
        getMark(&nVector, &this->simple_data->md_n);
        getMark(&tVector, &this->simple_data->md_t);

        this->pWaveData = createMglData(pVector, this->pWaveData, true);
        this->nWaveData = createMglData(nVector, this->nWaveData, true);
        this->tWaveData = createMglData(tVector, this->tWaveData, true);

        freev(pVector);
        freev(nVector);
        freev(tVector);

        vector pitch_cutted = copyv(this->simple_data->d_pitch_originl);
        MinMax mm = applyMask(&pitch_cutted, &this->simple_data->d_mask);
        this->pitchData = createMglData(pitch_cutted, this->pitchData, true);
        this->pitchData->Norm();

        this->f0max = mm.max;
        this->f0mix = mm.min;
        this->of0max = this->simple_data->d_pitch_originl.v[maxv(this->simple_data->d_pitch_originl)];
        this->of0mix = this->simple_data->d_pitch_originl.v[minv(this->simple_data->d_pitch_originl)];

        freev(pitch_cutted);
        qDebug() << "pitchData Filled";

        this->pitchDataOriginal = createMglData(this->simple_data->d_pitch_originl, this->pitchDataOriginal, true);
        this->pitchDataOriginal->Norm();

        this->intensiveData = createMglData(this->simple_data->d_intensive, this->intensiveData);
        this->intensiveData->Norm();

        qDebug() << "intensiveData Filled";
    }
    else
    {
        qDebug() << "DrawerDP::Proc";
        this->secFileName = fname;

        SimpleGraphData * dataSec = SimpleProcWave2Data(this->secFileName);

        this->secWaveData = createMglData(dataSec->d_full_wave, this->secWaveData);
        qDebug() << "waveData New Filled";

        qDebug() << "Start DP";
        int speksize = sptk_settings->spec->leng / 2 + 1;
        qDebug() << "data->d_spec " << this->simple_data->d_spec.x;
        qDebug() << "dataSec->d_spec " << dataSec->d_spec.x;
        ContinuousDP dp(
            new SpectrSignal(copyv(this->simple_data->d_spec), speksize),
            new SpectrSignal(copyv(dataSec->d_spec), speksize),
            1,
            sptk_settings->dp->continiusLimit
        );
        dp.applySettings(
            sptk_settings->dp->continiusKH,
            sptk_settings->dp->continiusKV,
            sptk_settings->dp->continiusKD,
            sptk_settings->dp->continiusKT
        );
        dp.calculate();
        qDebug() << "Stop DP";
        vector errorVector = dp.getErrorVector();
        int endPos = minv(errorVector);
        this->errorMax = errorVector.v[maxv(errorVector)];
        this->errorMin = errorVector.v[endPos];
        qDebug() << "errorVector " << errorVector.x;

        vector timeVector = dp.getTimeVector();
        qDebug() << "timeVector " << timeVector.x;

        if (sptk_settings->dp->showError)
        {
            this->errorData = createMglData(errorVector, this->errorData);
            this->errorData->Norm();
        }

        if (sptk_settings->dp->showTime)
        {
            this->timeData = createMglData(timeVector, this->timeData);
            this->timeData->Norm();
        }

        int startPos = endPos - timeVector.v[endPos];
        vector dpVector = zerov(errorVector.x);
        setMark(&dpVector, startPos);
        setMark(&dpVector, endPos);
        this->dpData = createMglData(dpVector, this->dpData, true);
        freev(dpVector);

        freev(errorVector);

        intvector mapping = dp.getMapping(endPos);

        double marksScale = 1.0 * this->simple_data->d_full_wave.x / mapping.x;
        qDebug() << "data->d_wave.x " << this->simple_data->d_full_wave.x;
        qDebug() << "mapping " << mapping.x;
        qDebug() << "marksScale " << marksScale;

        vector pSecVector = zerov(timeVector.x);
        vector nSecVector = zerov(timeVector.x);
        vector tSecVector = zerov(timeVector.x);

        freev(timeVector);

        qDebug() << "len " << endPos - startPos ;
        qDebug() << "startPos " << startPos;
        qDebug() << "endPos " << endPos;

        getMark(&pSecVector, &this->simple_data->md_p, startPos, marksScale, &mapping);
        getMark(&nSecVector, &this->simple_data->md_n, startPos, marksScale, &mapping);
        getMark(&tSecVector, &this->simple_data->md_t, startPos, marksScale, &mapping);

        this->pSecData = createMglData(pSecVector, this->pSecData, true);
        this->nSecData = createMglData(nSecVector, this->nSecData, true);
        this->tSecData = createMglData(tSecVector, this->tSecData, true);

        freev(pSecVector);
        freev(nSecVector);
        freev(tSecVector);

        vector pitch_cutted = cutv(dataSec->d_pitch_originl, startPos, endPos);
        applyMapping(&pitch_cutted, &mapping);
        applyMask(&pitch_cutted, &this->simple_data->d_mask);
        this->secPitchData = createMglData(pitch_cutted, this->secPitchData, true);
        qDebug() << "pitchData Filled " << dataSec->d_pitch.x;
        qDebug() << "intensiveData Filled " << this->pitchData->nx;

        vector o_pitch_cutted = copyv(this->simple_data->d_pitch_originl);
        applyMask(&o_pitch_cutted, &this->simple_data->d_mask);

        this->result = 0;
        int i = 0;
        for (i=0; i<o_pitch_cutted.x && i<pitch_cutted.x; i++)
        {
            this->result += fabs(o_pitch_cutted.v[i] - pitch_cutted.v[i]);
        }
        this->result /= 1.0*i;
        this->result = 1.0 - this->result;
        this->result *= 100;
        this->result = round(this->result);
        freev(o_pitch_cutted);
        freev(pitch_cutted);

        vector intensive_cutted = cutv(dataSec->d_intensive, startPos, endPos);
        applyMapping(&intensive_cutted, &mapping);
        this->secIntensiveData = createMglData(intensive_cutted, this->secIntensiveData);
        this->secIntensiveData->Norm();
        freev(intensive_cutted);

        qDebug() << "intensiveData Filled " << this->secIntensiveData->nx;

        freeiv(mapping);

        freeSimpleGraphData(dataSec);
        qDebug() << "New Data Processed";
    }
}


