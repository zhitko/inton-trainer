#include "drawerdp.h"

#include <QFile>
#include <QDebug>
#include <QFileInfo>

#include <math.h>
#include <cmath>

#include "settingsdialog.h"
#include <QApplication>
#include "DP/continuousdp.h"

#include "defines.h"

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

typedef struct { int from; int len; int type; } MaskDetails;

typedef struct { double min; double max; } MinMax;

QString getMarkLabel(int value, char * labels);

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
    this->umpData = NULL;
    this->secUmpData = NULL;
    this->octavData = NULL;
    this->secOctavData = NULL;
    this->umpMask = NULL;
    this->errorMax = 0;
    this->errorMin = 0;
    this->proximity_shape = 0.0;
    this->proximity_curve_shape = 0.0;
    this->proximity_range = 0.0;
    this->proximity_shape_mark = 0.0;
    this->proximity_range_mark = 0.0;
    this->proximity_mark = 0.0;
    this->f0max = 0;
    this->f0min = 0;
    this->userf0max = 0;
    this->userf0min = 0;
    this->simple_data = NULL;
}

DrawerDP::~DrawerDP()
{
    qDebug() << "DrawerDP removed" << LOG_DATA;
    if (this->dpData) delete this->dpData;
    qDebug() << "DrawerDP removed dpData" << LOG_DATA;
    if (this->secWaveData) delete this->secWaveData;
    qDebug() << "DrawerDP removed secWaveData" << LOG_DATA;
    if (this->errorData) delete this->errorData;
    qDebug() << "DrawerDP removed errorData" << LOG_DATA;
    if (this->timeData) delete this->timeData;
    qDebug() << "DrawerDP removed timeData" << LOG_DATA;
    if (this->pSecData) delete this->pSecData;
    qDebug() << "DrawerDP removed pSecData" << LOG_DATA;
    if (this->nSecData) delete this->nSecData;
    qDebug() << "DrawerDP removed nSecData" << LOG_DATA;
    if (this->tSecData) delete this->tSecData;
    qDebug() << "DrawerDP removed tSecData" << LOG_DATA;
    if (this->secPitchData) delete this->secPitchData;
    qDebug() << "DrawerDP removed secPitchData" << LOG_DATA;
    if (this->secIntensiveData) delete this->secIntensiveData;
    qDebug() << "DrawerDP removed secIntensiveData" << LOG_DATA;
    if (this->simple_data) freeSimpleGraphData(this->simple_data);
    qDebug() << "DrawerDP removed simple_data" << LOG_DATA;
    if (this->umpData) delete this->umpData;
    qDebug() << "DrawerDP removed umpData" << LOG_DATA;
    if (this->secUmpData) delete this->secUmpData;
    qDebug() << "DrawerDP removed secUmpData" << LOG_DATA;
    if (this->octavData) delete this->octavData;
    qDebug() << "DrawerDP removed octavData" << LOG_DATA;
    if (this->secOctavData) delete this->secOctavData;
    qDebug() << "DrawerDP removed secOctavData" << LOG_DATA;
    if (this->umpMask) delete this->umpMask;
    qDebug() << "DrawerDP removed umpMask" << LOG_DATA;
}

int DrawerDP::Draw(mglGraph *gr)
{
    SPTK_SETTINGS * sptk_settings = SettingsDialog::getSPTKsettings();
    qDebug() << "start drawing" << LOG_DATA;

    bool isCompare = !this->secFileName.isEmpty();

    gr->DefaultPlotParam();
    gr->Clf();

    QFileInfo fileInfo(this->fileName);
    QString phrase = QString(fileInfo.baseName());
    phrase = "\\b\\big{" + phrase + "}";
    qDebug() << "Phrase" << phrase;

    if(!this->showUMP)
    {
        qDebug() << "waveData" << LOG_DATA;
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
        gr->Puts(mglPoint(0,0),phrase.toLocal8Bit().data(), ":C", 24);

        gr->MultiPlot(2, 15, 26, 1, 1, "#");
        gr->Puts(
            mglPoint(0,0),
            QString("Template F0 min = %1, F0 max = %2").arg(this->f0min).arg(this->f0max).toLocal8Bit().data(),
            ":C",
            24
        );
    } else {
        gr->DefaultPlotParam();

        gr->MultiPlot(1, 36, 1, 1, 3, "#");
        gr->Puts(mglPoint(0,0), QString("INTONATION").toLocal8Bit().data(), ":C", 24);

        gr->MultiPlot(10, 12, 11, 1, 1, "#");
        gr->Puts(mglPoint(0,0), QString("Octaves Range").toLocal8Bit().data(), ":C", 20);

        gr->MultiPlot(10, 12, 15, 1, 1, "#");
        gr->Puts(mglPoint(0,0), QString("Tonal Portrait").toLocal8Bit().data(), ":C", 20);

        gr->MultiPlot(1, 12, 11, 1, 1, "#");
        gr->Puts(mglPoint(0,0),phrase.toLocal8Bit().data(), ":C", 24);

        gr->MultiPlot(2, 12, 20, 1, 1, "#");
        gr->Puts(
            mglPoint(0,0),
            QString("Template F0 min = %1, F0 max = %2").arg(this->f0min).arg(this->f0max).toLocal8Bit().data(),
            ":C",
            20
        );

        if(!isCompare)
        {
            gr->MultiPlot(20, 12, 41, 4, 8, "#");
            if (this->octavData->a[0] > OCTAVE_MAX_1)
            {
                gr->SetRange('y', 0, OCTAVE_MAX_2);
            } else {
                gr->SetRange('y', 0, OCTAVE_MAX_1);
            }
            gr->Axis("Y", "");
            gr->Grid("y", "W", "");
            gr->Bars(*this->octavData, "r");
        }

        gr->MultiPlot(40, 12, 129, 12, 6, "#");
        gr->Label('y', "_{low}", -1);
        gr->Label('y', "_{medium}", 0);
        gr->Label('y', "_{high}", 1);

        gr->MultiPlot(20, 12, 46, 13, 8, "#");

        gr->SetRange('y', 0, 1);
        gr->SetTicks('y', 1./3.);
        gr->SetRange('x', 0, this->umpData->nx);
        gr->Grid("y", "W", "");
        gr->SetTicks('x', sptk_settings->dp->portLen);
        gr->Grid("x", "W", "");

        gr->Area(*this->umpMask, "gwwww!");
        gr->Plot(*this->umpData, "-r4");
    }

    if(isCompare){
        if(!this->showUMP)
        {
            qDebug() << "secWaveData" << LOG_DATA;
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
                QString("Range: \\big{%1} \t Shape: \\big{%2} \t Average: \\big{%3}")
                        .arg(getMarkLabel(this->proximity_range_mark, sptk_settings->dp->mark_labels))
                        .arg(getMarkLabel(this->proximity_shape_mark, sptk_settings->dp->mark_labels))
                        .arg(getMarkLabel(this->proximity_mark, sptk_settings->dp->mark_labels))
                        .toLocal8Bit().data(),
                ":C",
                24
            );

            qDebug() << "errorData" << LOG_DATA;
            gr->MultiPlot(1, 15, 6, 1, 6, "#");
            gr->SetRange('y', 0, 1);
            if(this->errorData) gr->Plot(*this->errorData, "-B3");
            if(this->timeData) gr->Plot(*this->timeData, "-R3");
            if(sptk_settings->dp->showF0) gr->Plot(*this->secPitchData, "-R4");
            if(sptk_settings->dp->showA0) gr->Plot(*this->secIntensiveData, "-G4");

            gr->MultiPlot(2, 15, 27, 1, 1, "#");
            gr->Puts(
                mglPoint(0,0),
                QString("User F0 min = %1, F0 max = %2").arg(this->userf0min).arg(this->userf0max).toLocal8Bit().data(),
                ":C",
                24
            );
        } else {
            gr->DefaultPlotParam();

            gr->MultiPlot(1, 36, 1, 1, 3, "#");
            gr->Puts(
                mglPoint(0,0),
                QString("Range: \\big{%1} \t Shape: \\big{%2} \t Average: \\big{%3}")
                        .arg(getMarkLabel(this->proximity_range_mark, sptk_settings->dp->mark_labels))
                        .arg(getMarkLabel(this->proximity_shape_mark, sptk_settings->dp->mark_labels))
                        .arg(getMarkLabel(this->proximity_mark, sptk_settings->dp->mark_labels))
                        .toLocal8Bit().data(),
                ":C",
                24
            );

            gr->MultiPlot(10, 12, 11, 1, 1, "#");
            gr->Puts(mglPoint(0,0), QString("Octaves Range").toLocal8Bit().data(), ":C", 20);

            gr->MultiPlot(10, 12, 15, 1, 1, "#");
            gr->Puts(mglPoint(0,0), QString("Tonal Portrait").toLocal8Bit().data(), ":C", 20);

            gr->MultiPlot(1, 12, 11, 1, 1, "#");

            gr->Puts(mglPoint(0,0),phrase.toLocal8Bit().data(), ":C", 24);

            gr->MultiPlot(2, 12, 20, 1, 1, "#");
            gr->Puts(
                mglPoint(0,0),
                QString("Template F0 min = %1, F0 max = %2").arg(this->f0min).arg(this->f0max).toLocal8Bit().data(),
                ":C",
                20
            );

            gr->MultiPlot(2, 12, 21, 1, 1, "#");
            gr->Puts(
                mglPoint(0,0),
                QString("User F0 min = %1, F0 max = %2").arg(this->userf0min).arg(this->userf0max).toLocal8Bit().data(),
                ":C",
                20
            );

            gr->MultiPlot(20, 12, 41, 4, 8, "#");
            if (this->octavData->a[0] > OCTAVE_MAX_1 || this->secOctavData->a[1] > OCTAVE_MAX_1)
            {
                gr->SetRange('y', 0, OCTAVE_MAX_2);
            } else {
                gr->SetRange('y', 0, OCTAVE_MAX_1);
            }
            gr->Axis("Y", "");
            gr->Grid("y", "W", "");
            gr->Bars(*this->secOctavData, "R");
            gr->Bars(*this->octavData, "r");

            gr->MultiPlot(40, 12, 129, 12, 6, "#");
            gr->Label('y', "_{low}", -1);
            gr->Label('y', "_{medium}", 0);
            gr->Label('y', "_{high}", 1);

            gr->MultiPlot(20, 12, 46, 13, 8, "#");
            gr->SetRange('y', 0, 1);
            gr->SetTicks('y', 1./3.);
            gr->SetRange('x', 0, this->umpData->nx);
            gr->Grid("y", "W", "");
            gr->SetTicks('x', sptk_settings->dp->portLen);
            gr->Grid("x", "W", "");

            gr->Area(*this->umpMask, "gwwww!");
            gr->Plot(*this->secUmpData, "-R5");
            gr->Plot(*this->umpData, "-r4");
        }
    }

    qDebug() << "finish drawing" << LOG_DATA;
    return 0;
}

double range_log(double x)
{
    return log10(x/10)*500;
}

void setMark(vector * vec, int pos)
{
    if (vec->x > pos)
    {
        setv(*vec, 0, MASK_MAX);
        if (pos != 0)
        {
            setv(*vec, pos-1, MASK_MIN);
        }
        if (vec->x != pos + 1)
        {
            setv(*vec, pos+1, MASK_MIN);
        }
    } else {
        qDebug() << "WARNING setMark ? " << (vec->x > pos) << LOG_DATA;
        qDebug() << "WARNING setMark x " << vec->x << LOG_DATA;
        qDebug() << "WARNING setMark pos " << pos << LOG_DATA;
    }
}

void setMark(vector * vec, int from, int to)
{
    int i=from;
    for (i=from; i<to; i++)
    {
        setv(*vec, i, MASK_MAX);
    }
    setv(*vec, from, MASK_MIN);
    setv(*vec, i, MASK_MIN);
}

void applyMapping(vector * data, intvector * mapping)
{
    qDebug() << "data " << data->x << LOG_DATA;
    qDebug() << "mapping " << mapping->x << LOG_DATA;
    double scale = 1.0 * data->x / mapping->x;
    qDebug() << "scale " << scale << LOG_DATA;
    vector newData = zerov(data->x);
    for (int i=0; i<newData.x; i++)
    {
        int j = 1.0 * i / scale;
        int index = getiv(*mapping, j);
        setv(newData, i, getv(*data, index));
    }
    freev(*data);
    data->v = newData.v;
}

vector norm(vector data, double targetMin, double targetMax, bool procZeros = true)
{
    vector result = makev(data.x);
    double sourceMin = 0.0;
    if (procZeros){
        sourceMin = getv(data, minv(data));
    } else {
        sourceMin = getv(data, min_greaterv(data, 0.0));
    }
    double sourceMax = getv(data, maxv(data));
    qDebug() << "sourceMin " << sourceMin << LOG_DATA;
    qDebug() << "sourceMax " << sourceMax << LOG_DATA;

    double sourceScale = sourceMax - sourceMin;
    double targetScale = targetMax - targetMin;

    double zsrc, scaled;

    for(int i=0;i<data.x;i++)
        if (procZeros || getv(data, i) != 0)
        {
            zsrc =  getv(data, i) - sourceMin;
            scaled = zsrc * targetScale / sourceScale;
            setv(result, i, scaled + targetMin);
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
            if (procZeros || getv(data, i) != 0)
                {
                if( position < 0 || position > resultLength )
                {
                    setv(middle, frameIndex, 0.0);
                } else {
                    setv(middle, frameIndex, fabs(getv(data, position)));
                    frameIndex++;
                }
            }
        }
        if (frameIndex > frame/3)
        {
            qsort (middle.v, frameIndex-1, sizeof(double), compare);
            setv(result, i, getv(middle, frameIndex/2));
        }
    }
    freev(middle);

    return result;
}

int compare_mask_details (const void * a, const void * b)
{
  return ( ((MaskDetails*)a)->from - ((MaskDetails*)b)->from );
}

vector makeUmp(vector * data, vector * mask, MaskData mask_p, MaskData mask_n, MaskData mask_t, double mask_scale, int part_len, int useStripUmp)
{
    const int TYPE_P = 1;
    const int TYPE_N = 2;
    const int TYPE_T = 3;
    int len_p = mask_p.pointsFrom.x;
    int len_n = mask_n.pointsFrom.x;
    int len_t = mask_t.pointsFrom.x;
    int len = len_p + len_n + len_t;
    MaskDetails * details = new MaskDetails[len];

    for(int i=0; i<len_p; i++)
        details[i] = MaskDetails{
                getiv(mask_p.pointsFrom, i)/mask_scale,
                getiv(mask_p.pointsLength, i)/mask_scale,
                TYPE_P
        };

    for(int i=0; i<len_n; i++)
        details[len_p + i] = MaskDetails{
                getiv(mask_n.pointsFrom, i)/mask_scale,
                getiv(mask_n.pointsLength, i)/mask_scale,
                TYPE_N
        };

    for(int i=0; i<len_t; i++)
        details[len_p + len_n + i] = MaskDetails{
                getiv(mask_t.pointsFrom, i)/mask_scale,
                getiv(mask_t.pointsLength, i)/mask_scale,
                TYPE_T
        };

    qsort(details, len, sizeof(MaskDetails), compare_mask_details);

    int merge = 0;
    for(int i=0; i<(len-1); i++)
        if(details[i].type == details[i+1].type && (details[i].type == TYPE_P || details[i].type == TYPE_T))
            merge ++;

    int merge_len = len - merge;
    MaskDetails * merged_details = new MaskDetails[merge_len];

    merged_details[0] = details[0];
    int ii = 0;

    for(int i=1; i<len; i++)
    {
        if(details[i-1].type == details[i].type && (details[i].type == TYPE_P || details[i].type == TYPE_T))
        {
            if (useStripUmp)
                merged_details[ii].len += details[i].len;
            else
                merged_details[ii].len = details[i].from + details[i].len - merged_details[ii].from;
        } else {
            ii++;
            merged_details[ii] = details[i];
        }
    }

    delete details;

    if(!useStripUmp)
    {
        if(merge_len>0)
        {
            merged_details[0].len += merged_details[0].from;
            merged_details[0].from = 0;
        }

        for(int i=1; i<merge_len; i++)
        {
            int t1 = merged_details[i-1].type;
            int t2 = merged_details[i].type;

            if((t1 == TYPE_P || t1 == TYPE_T) && t2 == TYPE_N)
            {
                merged_details[i-1].len = merged_details[i].from - merged_details[i-1].from;
            } else if(t1 == TYPE_N && (t2 == TYPE_P || t2 == TYPE_T))
            {
                int d = merged_details[i].from - (merged_details[i-1].from + merged_details[i-1].len);
                merged_details[i].from -= d;
                merged_details[i].len += d;
            } else if(t1 == TYPE_N && t2 == TYPE_N)
            {
                int d = (merged_details[i].from - (merged_details[i-1].from + merged_details[i-1].len))/2;
                merged_details[i-1].len += d;
                merged_details[i].from -= d;
                merged_details[i].len += d;
            }
        }
    }

    int clone = 0;
    for(int i=1; i<(merge_len-1); i++)
    {
        int t1 = merged_details[i-1].type;
        int t2 = merged_details[i].type;
        int t3 = merged_details[i+1].type;
        if(t1 == TYPE_N && t3 == TYPE_N && (t2 == TYPE_P || t2 == TYPE_T))
            clone++;
    }

    int clone_len = merge_len + clone;
    MaskDetails * clone_details = new MaskDetails[clone_len];

    ii = 0;
    clone_details[0] = merged_details[0];

    vector ump_mask = zerov(clone_len*MASK_LEN);

    if(merged_details[0].type == TYPE_N)
    {
        setv(ump_mask, 0, MASK_MIN);
        for(int i=1; i<MASK_LEN; i++)
        {
            setv(ump_mask, i,MASK_MAX);
        }
        setv(ump_mask, MASK_LEN, MASK_MIN);
    }

    for(int i=1; i<(merge_len-1); i++)
    {
        int t1 = merged_details[i-1].type;
        int t2 = merged_details[i].type;
        int t3 = merged_details[i+1].type;
        if(t1 == TYPE_N && t3 == TYPE_N && (t2 == TYPE_P || t2 == TYPE_T))
        {
            ii++;
            clone_details[ii] = merged_details[i];
            clone_details[ii].len = merged_details[i].len / 2;
            ii++;
            clone_details[ii] = merged_details[i];
            clone_details[ii].from = clone_details[ii-1].from + clone_details[ii-1].len;
            clone_details[ii].len = clone_details[ii-1].len;
        } else {
            ii++;
            clone_details[ii] = merged_details[i];

            if(clone_details[ii].type == TYPE_N)
            {
                setv(ump_mask, ii*MASK_LEN, MASK_MIN);
                for(int i=1; i<MASK_LEN; i++)
                {
                    setv(ump_mask, ii*MASK_LEN+i, MASK_MAX);
                }
                setv(ump_mask, (ii+1)*MASK_LEN, MASK_MIN);
            }
        }
    }

    clone_details[clone_len-1] = merged_details[merge_len-1];
    if(clone_details[clone_len-1].type == TYPE_N)
    {
        ii++;
        setv(ump_mask, ii*MASK_LEN, MASK_MIN);
        for(int i=1; i<MASK_LEN; i++)
        {
            setv(ump_mask, ii*MASK_LEN+i, MASK_MAX);
        }
        setv(ump_mask, (ii+1)*MASK_LEN, MASK_MIN);
    }

    delete merged_details;

    qDebug() << "data " << data->x << LOG_DATA;
    qDebug() << "mask " << mask->x << LOG_DATA;
    vector strip_data;
    strip_data = copyv(*data);
    qDebug() << "strip_data " << strip_data.x << LOG_DATA;

    qDebug() << "clone_len " << clone_len << LOG_DATA;

    vector resized_data = zerov(part_len*clone_len);

    double scale = 1.0 * mask->x / data->x;

    ii = 0;
    int p = 0;
    for(int i=0; i<clone_len; i++)
    {
        int len = clone_details[i].len/scale;
        vector in = zerov(len);
        for(int j=0; j<len; j++)
        {
            p = clone_details[i].from/scale + j;
            setv(in, j, getv(strip_data, p));
        }
        qDebug() << "len " << in.x << " - " << clone_details[i].from << LOG_DATA;

        vector out = vector_resize(in, part_len);
        for (int j=0; j<out.x; j++)
        {
            p = j + i*part_len;
            setv(resized_data, p, getv(out, j));
        }
        freev(in);
        freev(out);

        ii += len;
    }

    freev(strip_data);

    freev(*data);
    data->v = resized_data.v;
    data->x = resized_data.x;

    delete clone_details;

    return ump_mask;
}

MinMax applyMask(vector * data, vector * mask)
{
    double min, max;
    SPTK_SETTINGS * sptk_settings = SettingsDialog::getSPTKsettings();

    vector scaledMask = vector_resize(*mask, data->x);
    qDebug() << "scaledMask" << LOG_DATA;
    vector cuttedData = vector_cut_by_mask(*data, scaledMask);
    qDebug() << "newData" << LOG_DATA;
    vector dataMid = mid(cuttedData, sptk_settings->plotF0->midFrame);
    min = getv(dataMid, min_greaterv(dataMid, 0.0));
    max = getv(dataMid, maxv(dataMid));
    qDebug() << "pitch_mid" << LOG_DATA;
    vector newDataNorm = norm(dataMid, 0.0, 1.0, !sptk_settings->plotF0->normF0MinMax);
    qDebug() << "newDataNorm" << LOG_DATA;
    vector dataInterpolate = copyv(newDataNorm);
    qDebug() << "dataInterpolate " << dataInterpolate.x << LOG_DATA;

    int start = first_fromv(dataInterpolate, 0, 0.0);
    int end = first_greater_fromv(dataInterpolate, start, 0.0);

    if (start == 0)
    {
        qDebug() << "vector_interpolate_part(" << &dataInterpolate << ", " << start << ", " << end << ", " << sptk_settings->plotF0->interpolation_type << ")" << LOG_DATA;
        vector_interpolate_part(
                    &dataInterpolate,
                    start,
                    end,
                    sptk_settings->plotF0->interpolation_type
        );

        start = first_fromv(dataInterpolate, end, 0.0);
        end = first_greater_fromv(dataInterpolate, start, 0.0);
    }

    do
    {
        qDebug() << "vector_interpolate_part(" << &dataInterpolate << ", " << start-1 << ", " << end << ", " << sptk_settings->plotF0->interpolation_type << ")" << LOG_DATA;
        vector_interpolate_part(
                    &dataInterpolate,
                    start-1,
                    end,
                    sptk_settings->plotF0->interpolation_type
        );
        start = first_fromv(dataInterpolate, end, 0.0);
        end = first_greater_fromv(dataInterpolate, start, 0.0);
    } while (end != start && end != dataInterpolate.x);

    qDebug() << "vector_interpolate_part(" << &dataInterpolate << ", " << start-1 << ", " << dataInterpolate.x - 1 << ", " << sptk_settings->plotF0->interpolation_type << ")" << LOG_DATA;
    vector_interpolate_part(
                &dataInterpolate,
                start-1,
                dataInterpolate.x - 1,
                sptk_settings->plotF0->interpolation_type
    );

    freev(scaledMask);
    freev(cuttedData);
    freev(dataMid);
    freev(newDataNorm);
    freev(*data);
    data->v = newDataNorm.v;
    data->v = dataInterpolate.v;
    qDebug() << "finish applyMask" << LOG_DATA;
    return MinMax{min, max};
}

void getMark(vector * vec, MaskData * points, int startPos, double marksScale, intvector * mapping)
{
    for (int i=0; i<points->pointsFrom.x; i++)
    {
        int from = getiv(*mapping, 1.0*getiv(points->pointsFrom, i)/marksScale) + startPos;
        int to = getiv(*mapping, 1.0*(getiv(points->pointsFrom, i) + getiv(points->pointsLength, i))/marksScale) + startPos;
        setMark(vec, from, to);
        qDebug() << "setMark pSecVector " << from << " - " << to << LOG_DATA;
    }
}

void getMark(vector * vec, MaskData * points)
{
    for (int i=0; i<points->pointsFrom.x; i++)
    {
        int from = getiv(points->pointsFrom, i);
        int to = getiv(points->pointsFrom, i) + getiv(points->pointsLength, i);
        setMark(vec, from, to);
    }
}

QString getMarkLabel(int value, char * labels)
{
    QStringList labelsList = QString(labels).split(",");

    if (value > labelsList.size())
    {
        return QString::number(value);
    } else {
        return labelsList.at(value-1);
    }
}

double calculateMark(double value, double level, double delimeter)
{
    double mark = 1.0;
    if (value >= level)
    {
        mark = round( fabs(level - value) / delimeter );
    }
    return mark;
}

/*
 * Correlation of F0-curves
R_{ AB }=\left| \left[ \frac { \sum _{ len }^{ i=0 }{ { ({ A }_{ i }-{ M }_{ A }) } } { ({ B }_{ i }-{ M }_{ A }) } }{ \sqrt { \sum _{ len }^{ i=0 }{ { ({ A }_{ i }-{ M }_{ A }) }^{ 2 } } *\sum _{ len }^{ i=0 }{ { ({ B }_{ i }-{ M }_{ A }) }^{ 2 } }  }  } *100% \right]  \right| 
*/
double calculateResultR(vector x, vector y)
{
    double result = 0;
    double my = midv(y);
    double mx = midv(x);
    double xx = 0.0;
    double yy = 0.0;
    for(int i=0; i<x.x && i<y.x; i++)
    {
        result += (getv(x, i)-mx)*(getv(y, i)-my);
        xx += (getv(x, i)-mx)*(getv(x, i)-mx);
        yy += (getv(y, i)-my)*(getv(y, i)-my);
    }
    result = result / sqrt(xx*yy);
    return round(fabs(result)*100);
}

/*
 * Integral proximity of F0-curves
D_{ AB }=\left[ \left( 1-\frac { \sqrt { \sum _{ len }^{ i=0 }{ { ({ A }_{ i }-{ B }_{ i }) }^{ 2 } }  }  }{ \sqrt { len }  }  \right) *100% \right] 
*/
double calculateResultD(vector x, vector y)
{
    double result = 0;
    for(int i=0; i<x.x && i<y.x; i++)
    {
        result += (getv(x, i)-getv(y, i))*(getv(x, i)-getv(y, i));
    }
    result = sqrt(result) / sqrt(x.x);
    return round((1-result)*100);
}

/*
 * Similarity of F0-curves
*/
double calculateResultS(vector x, vector y)
{
    double result = 0;

    double ab = 0;
    double a2 = 0;
    double b2 = 0;

    for(int i=0; i<x.x && i<y.x; i++)
    {
        ab += getv(x, i) * getv(y, i);
        a2 += getv(x, i) * getv(x, i);
        b2 += getv(y, i) * getv(y, i);
    }

    result = ab / (sqrt(a2) * sqrt(b2));

    return round(result*100);
}

/*
 * Local proximity of F0-curves
*/
double calculateResultL(vector x, vector y)
{
    double result = 0;
    for(int i=0; i<x.x && i<y.x; i++)
    {
        double r = fabs(getv(x, i)-getv(y, i));
        if (r > result)
        {
            result = r;
        }
    }
    return round((1-result)*100);
}

int DrawerDP::getDataSeconds()
{
    if (this->simple_data)
    {
        return this->simple_data->seconds;
    } else {
        return 0;
    }

}

void DrawerDP::Proc(QString fname)
{
    SPTK_SETTINGS * sptk_settings = SettingsDialog::getSPTKsettings();

    if(first)
    {
        qDebug() << "Drawer::Proc" << LOG_DATA;
        first = false;

        this->fileName = fname;

        this->simple_data = SimpleProcWave2Data(this->fileName);

        this->waveData = createMglData(this->simple_data->d_full_wave, this->waveData);
        qDebug() << "waveData Filled" << LOG_DATA;

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

        vector pitch_cutted = copyv(this->simple_data->d_pitch);
        MinMax mm = applyMask(&pitch_cutted, &this->simple_data->d_mask);
        qDebug() << "MinMax " << mm.min << ":" << mm.max << LOG_DATA;

        vector pitch_smooth;
        if (sptk_settings->dp->umpSmoothType == 0)
        {
            pitch_smooth = vector_smooth_lin(pitch_cutted, sptk_settings->dp->umpSmoothValue);
        } else if (sptk_settings->dp->umpSmoothType == 1) {
            pitch_smooth = vector_smooth_mid(pitch_cutted, sptk_settings->dp->umpSmoothValue);
        } else {
            pitch_smooth = copyv(pitch_cutted);
        }

        this->pitchData = createMglData(pitch_smooth, this->pitchData, true);
        this->pitchData->Norm();
        qDebug() << "pitchData createMglData" << LOG_DATA;

        this->f0max = mm.max;
        this->f0min = mm.min;

        this->octavData = new mglData(2);
        qDebug() << "this->f0max " << this->f0max << LOG_DATA;
        qDebug() << "this->f0min " << this->f0min << LOG_DATA;
        this->octavData->a[0] = (1.0 * this->f0max / this->f0min) - 1;
        qDebug() << "this->octavData->a[0] " << this->octavData->a[0] << LOG_DATA;
        if(this->octavData->a[0] > OCTAVE_MAX_2) this->octavData->a[0] = OCTAVE_MAX_2;
        qDebug() << "octavData createMglData" << LOG_DATA;

        vector origin_ump = copyv(pitch_smooth);

        double mask_scale = 1.0 * this->simple_data->d_full_wave.x / this->simple_data->d_mask.x;
        qDebug() << "mask_scale " << mask_scale << LOG_DATA;
        vector ump_mask = makeUmp(
            &origin_ump,
            &this->simple_data->d_mask,
            this->simple_data->md_p,
            this->simple_data->md_n,
            this->simple_data->md_t,
            mask_scale,
            sptk_settings->dp->portLen,
            sptk_settings->dp->useStripUmp
        );

        this->umpData = createMglData(origin_ump, this->umpData);
        this->umpData->Norm();

        this->umpMask = createMglData(ump_mask, this->umpMask, true);

        freev(origin_ump);

        freev(pitch_cutted);
        freev(pitch_smooth);
        qDebug() << "pitchData Filled" << LOG_DATA;

        if(sptk_settings->dp->showOriginalF0)
        {
            this->pitchDataOriginal = createMglData(this->simple_data->d_pitch_original, this->pitchDataOriginal, true);
            this->pitchDataOriginal->Norm();
        }

        if(sptk_settings->dp->showA0)
        {
            this->intensiveData = createMglData(this->simple_data->d_intensive, this->intensiveData);
            this->intensiveData->Norm();
        }

        qDebug() << "intensiveData Filled" << LOG_DATA;

        this->proximity_curve_correlation = 0;
        this->proximity_curve_integral = 0;
        this->proximity_curve_similarity = 0;
        this->proximity_curve_local = 0;
    }
    else
    {
        qDebug() << "DrawerDP::Proc" << LOG_DATA;
        this->secFileName = fname;

        SimpleGraphData * dataSec = SimpleProcWave2Data(this->secFileName);

        this->secWaveData = createMglData(dataSec->d_full_wave, this->secWaveData);
        qDebug() << "waveData New Filled" << LOG_DATA;

        qDebug() << "Start DP" << LOG_DATA;
        SpectrSignal * firstSignal;
        SpectrSignal * secondSignal;
        if (sptk_settings->dp->useForDP == 0)
        {
            int speksize = sptk_settings->spec->leng / 2 + 1;
            qDebug() << "data->d_spec " << this->simple_data->d_spec.x << LOG_DATA;
            qDebug() << "dataSec->d_spec " << dataSec->d_spec.x << LOG_DATA;
            firstSignal = new SpectrSignal(copyv(this->simple_data->d_spec_proc), speksize);
            secondSignal = new SpectrSignal(copyv(dataSec->d_spec_proc), speksize);
        }
        if (sptk_settings->dp->useForDP == 1)
        {
            int speksize = sptk_settings->lpc->cepstrum_order + 1;
            qDebug() << "data->d_cepstrum " << this->simple_data->d_cepstrum.x << LOG_DATA;
            qDebug() << "dataSec->d_cepstrum " << dataSec->d_cepstrum.x << LOG_DATA;
            firstSignal = new SpectrSignal(copyv(this->simple_data->d_cepstrum), speksize);
            secondSignal = new SpectrSignal(copyv(dataSec->d_cepstrum), speksize);
        }

        ContinuousDP dp(
            firstSignal,
            secondSignal,
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
        qDebug() << "Stop DP" << LOG_DATA;
        vector errorVector = dp.getErrorVector();
        int endPos = minv(errorVector);
        this->errorMax = getv(errorVector, maxv(errorVector));
        this->errorMin = getv(errorVector, endPos);
        qDebug() << "errorVector " << errorVector.x << LOG_DATA;

        vector timeVector = dp.getTimeVector();
        qDebug() << "timeVector " << timeVector.x << LOG_DATA;

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

        int startPos = endPos - getv(timeVector, endPos);
        vector dpVector = zerov(errorVector.x);
        setMark(&dpVector, startPos);
        setMark(&dpVector, endPos);
        this->dpData = createMglData(dpVector, this->dpData, true);
        freev(dpVector);

        freev(errorVector);

        intvector mapping = dp.getMapping(endPos);

        double marksScale = 1.0 * this->simple_data->d_full_wave.x / mapping.x;
        qDebug() << "data->d_wave.x " << this->simple_data->d_full_wave.x << LOG_DATA;
        qDebug() << "mapping " << mapping.x << LOG_DATA;
        qDebug() << "marksScale " << marksScale << LOG_DATA;

        vector pSecVector = zerov(timeVector.x);
        vector nSecVector = zerov(timeVector.x);
        vector tSecVector = zerov(timeVector.x);

        freev(timeVector);

        qDebug() << "len " << endPos - startPos << LOG_DATA;
        qDebug() << "startPos " << startPos << LOG_DATA;
        qDebug() << "endPos " << endPos << LOG_DATA;

        getMark(&pSecVector, &this->simple_data->md_p, startPos, marksScale, &mapping);
        getMark(&nSecVector, &this->simple_data->md_n, startPos, marksScale, &mapping);
        getMark(&tSecVector, &this->simple_data->md_t, startPos, marksScale, &mapping);

        this->pSecData = createMglData(pSecVector, this->pSecData, true);
        this->nSecData = createMglData(nSecVector, this->nSecData, true);
        this->tSecData = createMglData(tSecVector, this->tSecData, true);

        freev(pSecVector);
        freev(nSecVector);
        freev(tSecVector);

        vector pitch_cutted = cutv(dataSec->d_pitch, startPos, endPos);
        applyMapping(&pitch_cutted, &mapping);

        MinMax mm = applyMask(&pitch_cutted, &this->simple_data->d_mask);
        this->userf0max = mm.max;
        this->userf0min = mm.min;

        vector pitch_smooth;
        if (sptk_settings->dp->umpSmoothType == 0)
        {
            pitch_smooth = vector_smooth_lin(pitch_cutted, sptk_settings->dp->umpSmoothValue);
        } else if (sptk_settings->dp->umpSmoothType == 1) {
            pitch_smooth = vector_smooth_mid(pitch_cutted, sptk_settings->dp->umpSmoothValue);
        } else {
            pitch_smooth = copyv(pitch_cutted);
        }

        this->secOctavData = new mglData(2);
        this->secOctavData->a[1] = (1.0 * this->userf0max / this->userf0min) - 1;
        if(this->secOctavData->a[1] > OCTAVE_MAX_2) this->secOctavData->a[1] = OCTAVE_MAX_2;

        if(sptk_settings->dp->showF0)
        {
            this->secPitchData = createMglData(pitch_smooth, this->secPitchData, true);
        }

        double user_max = 1.0*this->userf0max;
        double user_min = 1.0*this->userf0min;
        double template_max = 1.0*this->f0max;
        double template_min = 1.0*this->f0min;
        if (user_min == 0) user_min = 1;
        if (template_min == 0) template_min = 1;

        double tmm = template_max/template_min;
        double umm = user_max/user_min;

        this->proximity_range = round(fabs(1.0 - (fabs((tmm - 1.0) - (umm - 1.0)) / (tmm - 1.0))) * 100.0);
        this->proximity_range_mark = calculateMark(proximity_range, sptk_settings->dp->mark_level, sptk_settings->dp->mark_delimeter);

        qDebug() << "sptk_settings->dp->errorType " << sptk_settings->dp->errorType << LOG_DATA;
        vector o_pitch_cutted = copyv(this->simple_data->d_pitch_original);
        applyMask(&o_pitch_cutted, &this->simple_data->d_mask);
        switch (sptk_settings->dp->errorType) {
        case 0:
            this->proximity_shape = calculateResultR(o_pitch_cutted, pitch_smooth);
            break;
        case 1:
            this->proximity_shape = calculateResultD(o_pitch_cutted, pitch_smooth);
            break;
        }

        vector sec_ump = copyv(pitch_smooth);

        double mask_scale = 1.0 * this->simple_data->d_full_wave.x / this->simple_data->d_mask.x;
        makeUmp(
            &sec_ump,
            &this->simple_data->d_mask,
            this->simple_data->md_p,
            this->simple_data->md_n,
            this->simple_data->md_t,
            mask_scale,
            sptk_settings->dp->portLen,
            sptk_settings->dp->useStripUmp
        );

        this->secUmpData = createMglData(sec_ump, this->secUmpData);
        this->secUmpData->Norm();

        vector ump;
        ump.x = this->umpData->nx;
        ump.v = this->umpData->a;

        this->proximity_curve_correlation = calculateResultR(ump, sec_ump);
        this->proximity_curve_integral = calculateResultD(ump, sec_ump);
        this->proximity_curve_similarity = calculateResultS(ump, sec_ump);
        this->proximity_curve_local = calculateResultL(ump, sec_ump);

        switch (sptk_settings->dp->errorType) {
        case 0:
            this->proximity_curve_shape = proximity_curve_correlation;
            break;
        case 1:
            this->proximity_curve_shape = proximity_curve_integral;
            break;
        }

        this->proximity_shape_mark = calculateMark(proximity_curve_shape, sptk_settings->dp->mark_level, sptk_settings->dp->mark_delimeter);

        this->proximity_mark = round( (this->proximity_range_mark + this->proximity_shape_mark) / 2);

        freev(sec_ump);

        freev(o_pitch_cutted);
        freev(pitch_cutted);
        freev(pitch_smooth);

        if(sptk_settings->dp->showA0)
        {
            vector intensive_cutted = cutv(dataSec->d_intensive, startPos, endPos);
            applyMapping(&intensive_cutted, &mapping);
            this->secIntensiveData = createMglData(intensive_cutted, this->secIntensiveData);
            this->secIntensiveData->Norm();
            freev(intensive_cutted);

            qDebug() << "intensiveData Filled " << this->secIntensiveData->nx << LOG_DATA;
        }

        freeiv(mapping);

        freeSimpleGraphData(dataSec);
        qDebug() << "New Data Processed" << LOG_DATA;
    }
}


