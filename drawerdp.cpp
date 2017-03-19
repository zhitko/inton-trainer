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
    this->ps = 0.0;
    this->pr = 0.0;
    this->f0max = 0;
    this->f0min = 0;
    this->userf0max = 0;
    this->userf0min = 0;
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
    gr->Plot(*pWaveData, "q9");
    gr->Plot(*nWaveData, "k9");
    gr->Plot(*tWaveData, "c9");

    gr->MultiPlot(1, 15, 6, 1, 6, "#");
    gr->SetRange('y', 0, 1);
    gr->Axis("Y", "");
    gr->Grid("y", "W", "");
    if(sptk_settings->dp->showF0) gr->Plot(*this->pitchData, "-r3");
    if(sptk_settings->dp->showOriginalF0) gr->Plot(*this->pitchDataOriginal, "-r2");
    if(sptk_settings->dp->showA0) gr->Plot(*this->intensiveData, "-g3");
    gr->Plot(*pWaveData, "q2");
    gr->Plot(*nWaveData, "k2");
    gr->Plot(*tWaveData, "c2");

    gr->MultiPlot(1, 15, 12, 1, 1, "#");
    QString path = QApplication::applicationDirPath() + DATA_PATH_TRAINING;
    gr->Puts(mglPoint(0,0),fileName.replace(path,"").replace(".wav","").replace("/"," - ").toLocal8Bit().data(), ":C", 24);

    gr->MultiPlot(1, 15, 13, 1, 1, "#");
    gr->Puts(
        mglPoint(0,0),
        QString("Template F0 min = %1 max = %2").arg(this->f0min).arg(this->f0max).toLocal8Bit().data(),
        ":C",
        24
    );

    if(isCompare){
        qDebug() << "secWaveData";
        gr->MultiPlot(1, 15, 3, 1, 1, "#");
        gr->SetRange('y', 0, 1);
        gr->Plot(*pSecData, "q9");
        gr->Plot(*nSecData, "k9");
        gr->Plot(*tSecData, "c9");
        gr->Plot(*secWaveData, "G");
        gr->Plot(*dpData, "R9");

        gr->MultiPlot(1, 15, 4, 1, 1, "#");
        gr->Puts(
            mglPoint(0,0),
            QString("Proximity to curve shape: \\big{Ps = %1%} \t Proximity to the range: \\big{Pr = %2%").arg(this->ps).arg(this->pr).toLocal8Bit().data(),
            ":C",
            24
        );

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

        gr->MultiPlot(3, 15, 41, 1, 1, "#");
        gr->Puts(
            mglPoint(0,0),
            QString("User F0 min = %1 max = %2").arg(this->userf0min).arg(this->userf0max).toLocal8Bit().data(),
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
    vector dataMid = mid(cuttedData, sptk_settings->plotF0->midFrame);
    min = dataMid.v[min_greaterv(dataMid, 0.0)];
    max = dataMid.v[maxv(dataMid)];
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

double calculateResultR(vector x, vector y)
{
    double result = 0;
    double my = midv(y);
    double mx = midv(x);
    double xx = 0.0;
    double yy = 0.0;
    for(int i=0; i<x.x && i<y.x; i++)
    {
        result += (x.v[i]-mx)*(y.v[i]-my);
        xx += (x.v[i]-mx)*(x.v[i]-mx);
        yy += (y.v[i]-my)*(y.v[i]-my);
    }
    result = result / sqrt(xx*yy);
    return round(fabs(result)*100);
}

double calculateResultD(vector x, vector y)
{
    double result = 0;
    for(int i=0; i<x.x && i<y.x; i++)
    {
        result += (x.v[i]-y.v[i])*(x.v[i]-y.v[i]);
    }
    result = sqrt(result) / sqrt(x.x);
    return round((1-result)*100);
}

void DrawerDP::Proc(QString fname)
{
    SPTK_SETTINGS * sptk_settings = SettingsDialog::getSPTKsettings();

    if(first)
    {
        qDebug() << "Drawer::Proc";
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

        if(sptk_settings->dp->showF0)
        {
            vector pitch_cutted = copyv(this->simple_data->d_pitch_originl);
            MinMax mm = applyMask(&pitch_cutted, &this->simple_data->d_mask);
            this->pitchData = createMglData(pitch_cutted, this->pitchData, true);
            this->pitchData->Norm();

            this->f0max = mm.max;
            this->f0min = mm.min;

            freev(pitch_cutted);
            qDebug() << "pitchData Filled";
        }

        if(sptk_settings->dp->showOriginalF0)
        {
            this->pitchDataOriginal = createMglData(this->simple_data->d_pitch_originl, this->pitchDataOriginal, true);
            this->pitchDataOriginal->Norm();
        }

        if(sptk_settings->dp->showA0)
        {
            this->intensiveData = createMglData(this->simple_data->d_intensive, this->intensiveData);
            this->intensiveData->Norm();
        }

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
            new SpectrSignal(copyv(this->simple_data->d_spec_proc), speksize),
            new SpectrSignal(copyv(dataSec->d_spec_proc), speksize),
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
        MinMax mm = applyMask(&pitch_cutted, &this->simple_data->d_mask);
        this->userf0max = mm.max;
        this->userf0min = mm.min;
        if(sptk_settings->dp->showF0)
        {
            this->secPitchData = createMglData(pitch_cutted, this->secPitchData, true);
        }
//        this->pr = round((1.0 - fabs(1.0*this->f0max/this->f0min - 1.0*this->userf0max/this->userf0min))*100);
        double user_max = 1.0*this->userf0max;
        double user_min = 1.0*this->userf0min;
        double template_max = 1.0*this->f0max;
        double template_min = 1.0*this->f0min;
        if (user_min == 0) user_min = 1;
        if (template_min == 0) template_min = 1;
        this->pr = round( (user_max/user_min)/(template_max/template_min) )*100;

        qDebug() << "sptk_settings->dp->errorType " << sptk_settings->dp->errorType;
        vector o_pitch_cutted = copyv(this->simple_data->d_pitch_originl);
        applyMask(&o_pitch_cutted, &this->simple_data->d_mask);
        switch (sptk_settings->dp->errorType) {
        case 0:
            this->ps = calculateResultR(o_pitch_cutted, pitch_cutted);
            break;
        case 1:
            this->ps = calculateResultD(o_pitch_cutted, pitch_cutted);
            break;
        }

        freev(o_pitch_cutted);
        freev(pitch_cutted);

        if(sptk_settings->dp->showA0)
        {
            vector intensive_cutted = cutv(dataSec->d_intensive, startPos, endPos);
            applyMapping(&intensive_cutted, &mapping);
            this->secIntensiveData = createMglData(intensive_cutted, this->secIntensiveData);
            this->secIntensiveData->Norm();
            freev(intensive_cutted);

            qDebug() << "intensiveData Filled " << this->secIntensiveData->nx;
        }

        freeiv(mapping);

        freeSimpleGraphData(dataSec);
        qDebug() << "New Data Processed";
    }
}


