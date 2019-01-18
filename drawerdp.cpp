#include "drawerdp.h"

#include <QFile>
#include <QDebug>
#include <QFileInfo>

#include <math.h>
#include <cmath>

#include "settingsdialog.h"
#include <QApplication>
#include "dp/continuousdp.h"

#include "utils.h"
#include "defines.h"

#include "analysis/curve_similarity.h"
#include "analysis/ump.h"
#include "analysis/mask_data.h"

extern "C" {
    #include "./openal/wavFile.h"

    #include "float.h"

    #include "./sptk/SPTK.h"
    #include "./sptk/others/func.h"
    #include "./sptk/others/interpolation.h"
    #include "./sptk/pitch/pitch.h"
    #include "./sptk/x2x/x2x.h"
    #include "./sptk/frame/frame.h"
    #include "./sptk/window/window.h"
    #include "./sptk/lpc/lpc.h"
    #include "./sptk/spec/spec.h"
    #include "./analysis/derivative.h"
}

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
    this->secPitchDataDerivative = NULL;
    this->umpData = NULL;
    this->secUmpData = NULL;
    this->umpDerivativeData = NULL;
    this->secUmpDerivativeData = NULL;
    this->octavData = NULL;
    this->secOctavData = NULL;
    this->umpMask = NULL;
    this->errorMax = 0;
    this->errorMin = 0;
    this->proximity_curve_shape = 0.0;
    this->proximity_range = 0.0;
    this->proximity_shape_mark = 0.0;
    this->proximity_range_mark = 0.0;
    this->f0max = 0;
    this->f0min = 0;
    this->userf0max = 0;
    this->userf0min = 0;
    this->simple_data = NULL;
    this->ru = 1.0;
    this->rt = 1.0;
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
    if (this->secPitchDataDerivative) delete this->secPitchDataDerivative;
    qDebug() << "DrawerDP removed secPitchDataDerivative" << LOG_DATA;
    if (this->secIntensiveData) delete this->secIntensiveData;
    qDebug() << "DrawerDP removed secIntensiveData" << LOG_DATA;
    if (this->simple_data) freeSimpleGraphData(this->simple_data);
    qDebug() << "DrawerDP removed simple_data" << LOG_DATA;
    if (this->umpData) delete this->umpData;
    qDebug() << "DrawerDP removed umpData" << LOG_DATA;
    if (this->secUmpData) delete this->secUmpData;
    qDebug() << "DrawerDP removed secUmpData" << LOG_DATA;
    if (this->umpDerivativeData) delete this->umpDerivativeData;
    qDebug() << "DrawerDP removed umpDerivativeData" << LOG_DATA;
    if (this->secUmpDerivativeData) delete this->secUmpDerivativeData;
    qDebug() << "DrawerDP removed secUmpDerivativeData" << LOG_DATA;
    if (this->octavData) delete this->octavData;
    qDebug() << "DrawerDP removed octavData" << LOG_DATA;
    if (this->secOctavData) delete this->secOctavData;
    qDebug() << "DrawerDP removed secOctavData" << LOG_DATA;
    if (this->umpMask) delete this->umpMask;
    qDebug() << "DrawerDP removed umpMask" << LOG_DATA;
}

QString DrawerDP::getMarksTitle()
{
    SPTK_SETTINGS * sptk_settings = SettingsDialog::getSPTKsettings();

    if (sptk_settings->dp->show_marks)
    {
        return QString("Range (%3%): \\big{%1} \t Shape (%4%): \\big{%2}")
            .arg(getMarkLabel(this->proximity_range_mark, sptk_settings->dp->mark_labels))
            .arg(getMarkLabel(this->proximity_shape_mark, sptk_settings->dp->mark_labels))
            .arg(this->proximity_range)
            .arg(this->proximity_curve_shape);
    } else {
        return QString("Range: \\big{%1%} \t Shape: \\big{%2%}")
            .arg(this->proximity_range)
            .arg(this->proximity_curve_shape);
    }
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
    qDebug() << "Phrase" << phrase << LOG_DATA;

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
        if(sptk_settings->dp->showDerivativeF0)
        {
            gr->Plot(*this->pitchDataDerivative, "-m2");
            gr->Line(mglPoint(-1,this->pitchDataDerivativeZero), mglPoint(1,this->pitchDataDerivativeZero), "-m2");
        }
        if(sptk_settings->dp->showA0) gr->Plot(*this->intensiveData, "-b3");
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

        gr->MultiPlot(8, 12, 80, 3, 1, "#");
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

        if (sptk_settings->dp->showPlane) {
            gr->MultiPlot(20, 12, 48, 8, 8, "#");
            gr->Puts(mglPoint(-0.8, 2.6), "F0 (%)", ":C", 3);
            gr->Puts(mglPoint(1.3, 0), "dF0 (%)", ":C", 3);
            gr->SetRange('x', 0, 1);
            gr->SetRange('y', 0, 1);
            gr->SetTicks('y', 2);
            gr->AddTick('y', 0.2, "20");
            gr->AddTick('y', 0.4, "40");
            gr->AddTick('y', 0.6, "60");
            gr->AddTick('y', 0.8, "80");
            gr->AddTick('y', 1.0, "100");
            gr->Grid("x", "W", "");
            gr->Grid("y", "W", "");
            gr->Axis("y");
            gr->Box();

            gr->Plot(*this->umpData, *this->umpDerivativeData, "-r4");
        } else {
            gr->MultiPlot(20, 12, 47, 13, 8, "#");
            gr->SetRange('y', 0, 1);
            gr->SetTicks('y', 1./3.);
            gr->Grid("y", "W", "");
            gr->SetTicks('x', sptk_settings->dp->portLen);
            gr->Grid("x", "W5-", "");

            gr->Area(*this->umpMask, "gwwww!");

            if (sptk_settings->dp->showF0 || !sptk_settings->dp->showDerivativeF0)
            {
                gr->SetRange('x', 0, this->umpData->nx);
                gr->Plot(*this->umpData, "-r4");
            }
            if (sptk_settings->dp->showDerivativeF0)
            {
                gr->SetRange('x', 0, this->umpDerivativeData->nx);
                gr->Plot(*this->umpDerivativeData, "-m4");
            }
            if (sptk_settings->dp->showMeanValueUMP)
            {
                gr->FPlot(QString::number(this->meanValueUMP).toLocal8Bit().data(), ";r4");
            }
            if (sptk_settings->dp->showCenterGravityUMP)
            {
                gr->SetRange('x', 0, this->relativeTempo);
                gr->FPlot(QString::number(this->centricGravityUMP).toLocal8Bit().data(), "t", "0", "ir4");
            }

            gr->MultiPlot(40, 12, 129, 12, 6, "#");
            gr->Label('y', "_{low}", -1);
            gr->Label('y', "_{medium}", 0);
            gr->Label('y', "_{high}", 1);
        }
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
                this->getMarksTitle().toLocal8Bit().data(),
                ":C",
                24
            );

            qDebug() << "errorData" << LOG_DATA;
            gr->MultiPlot(1, 15, 6, 1, 6, "#");
            gr->SetRange('y', 0, 1);
            if(this->errorData) gr->Plot(*this->errorData, "-B3");
            if(this->timeData) gr->Plot(*this->timeData, "-R3");
            if(sptk_settings->dp->showF0) gr->Plot(*this->secPitchData, "-R4");
            if(sptk_settings->dp->showA0) gr->Plot(*this->secIntensiveData, "-B4");
            if(sptk_settings->dp->showDerivativeF0)
            {
                gr->Plot(*this->secPitchDataDerivative, "-m4");
                gr->Line(mglPoint(-1,this->secPitchDataDerivativeZero), mglPoint(1,this->secPitchDataDerivativeZero), "-m4");
            }

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
                this->getMarksTitle().toLocal8Bit().data(),
                ":C",
                24
            );

            gr->MultiPlot(10, 12, 11, 1, 1, "#");
            gr->Puts(mglPoint(0,0), QString("Octaves Range").toLocal8Bit().data(), ":C", 20);

            gr->MultiPlot(10, 12, 15, 1, 1, "#");
            gr->Puts(mglPoint(0,0), QString("Tonal Portrait").toLocal8Bit().data(), ":C", 20);

            gr->MultiPlot(1, 12, 11, 1, 1, "#");

            gr->Puts(mglPoint(0,0),phrase.toLocal8Bit().data(), ":C", 24);

            gr->MultiPlot(8, 12, 80, 3, 1, "#");
            gr->Puts(
                mglPoint(0,0),
                QString("Template F0 min = %1, F0 max = %2").arg(this->f0min).arg(this->f0max).toLocal8Bit().data(),
                ":C",
                20
            );

            gr->MultiPlot(8, 12, 83, 3, 1, "#");
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

            if (sptk_settings->dp->showPlane) {
                gr->MultiPlot(20, 12, 48, 8, 8, "#");
                gr->Puts(mglPoint(-0.8, 2.6), "F0 (%)", ":C", 3);
                gr->Puts(mglPoint(1.3, 0), "dF0 (%)", ":C", 3);
                gr->SetRange('x', 0, 1);
                gr->SetRange('y', 0, 1);
                gr->SetTicks('y', 2);
                gr->AddTick('y', 0.2, "20");
                gr->AddTick('y', 0.4, "40");
                gr->AddTick('y', 0.6, "60");
                gr->AddTick('y', 0.8, "80");
                gr->AddTick('y', 1.0, "100");
                gr->Grid("x", "W", "");
                gr->Grid("y", "W", "");
                gr->Axis("y");
                gr->Box();

                gr->Plot(*this->umpData, *this->umpDerivativeData, "-r4");
                gr->Plot(*this->secUmpData, *this->secUmpDerivativeData, "-R5");
            } else {
                gr->MultiPlot(20, 12, 46, 13, 8, "#");
                gr->SetRange('y', 0, 1);
                gr->SetTicks('y', 1./3.);
                gr->Grid("y", "W", "");
                gr->SetTicks('x', sptk_settings->dp->portLen);
                gr->Grid("x", "W5-", "");

                gr->Area(*this->umpMask, "gwwww!");
                if (sptk_settings->dp->showF0 || !sptk_settings->dp->showDerivativeF0)
                {
                    gr->SetRange('x', 0, this->umpData->nx);
                    gr->Plot(*this->secUmpData, "-R5");
                    gr->Plot(*this->umpData, "-r4");
                }
                if (sptk_settings->dp->showDerivativeF0)
                {
                    gr->SetRange('x', 0, this->umpDerivativeData->nx);
                    gr->Plot(*this->secUmpDerivativeData, "-M5");
                    gr->Plot(*this->umpDerivativeData, "-m4");
                }
                if (sptk_settings->dp->showMeanValueUMP)
                {
                    gr->FPlot(QString::number(this->meanValueUMP).toLocal8Bit().data(), ";r4");
                    gr->FPlot(QString::number(this->userMeanValueUMP).toLocal8Bit().data(), ";R4");
                }
                if (sptk_settings->dp->showCenterGravityUMP)
                {
                    gr->SetRange('x', 0, this->relativeTempo);
                    gr->FPlot(QString::number(this->centricGravityUMP).toLocal8Bit().data(), "t", "0", "ir4");
                    gr->SetRange('x', 0, this->userRelativeTempo);
                    gr->FPlot(QString::number(this->userCentricGravityUMP).toLocal8Bit().data(), "t", "0", "iR4");
                }

                gr->MultiPlot(40, 12, 129, 12, 6, "#");
                gr->Label('y', "_{low}", -1);
                gr->Label('y', "_{medium}", 0);
                gr->Label('y', "_{high}", 1);
            }
        }
    }

    qDebug() << "finish drawing" << LOG_DATA;
    return 0;
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



int compare (const void * a, const void * b)
{
  return ( *(double*)a - *(double*)b );
}

MinMax applyMask(vector * data, vector * mask)
{
    double min, max;
    SPTK_SETTINGS * sptk_settings = SettingsDialog::getSPTKsettings();

    vector scaledMask = vector_resize(*mask, data->x);
    qDebug() << "scaledMask" << LOG_DATA;
    vector cuttedData = vector_cut_by_mask(*data, scaledMask);
    qDebug() << "newData" << LOG_DATA;
    vector dataMid = vector_mid(cuttedData, sptk_settings->plotF0->frame, 1);
    min = getv(dataMid, min_greaterv(dataMid, 0.0));
    max = getv(dataMid, maxv(dataMid));
    qDebug() << "pitch_mid" << LOG_DATA;
    vector newDataNorm = vector_normalize_optional_zeros(dataMid, NORM_FROM, NORM_TO, !sptk_settings->plotF0->normF0MinMax);
    qDebug() << "newDataNorm" << LOG_DATA;
    vector dataInterpolate = copyv(newDataNorm);
    qDebug() << "dataInterpolate " << dataInterpolate.x << LOG_DATA;

    int start = first_fromv(dataInterpolate, 0, 0.0);
    int end = first_greater_fromv(dataInterpolate, start, 0.0);

    if (start == 0)
    {
        qDebug() << "vector_interpolate_part_first(" << &dataInterpolate << ", " << start << ", " << end << ", " << sptk_settings->plotF0->interpolation_type << ")" << LOG_DATA;
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

    qDebug() << "vector_interpolate_part_last(" << &dataInterpolate << ", " << start-1 << ", " << dataInterpolate.x - 1 << ", " << sptk_settings->plotF0->interpolation_type << ")" << LOG_DATA;
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

    if (0 < value && value <= labelsList.size())
    {
        return labelsList.at(value-1);
    } else {
        return QString::number(value);
    }
}

double calculateMark(double value, double level, double count)
{
    double delimeter = (100.0 - level) / count;
    double mark = 1.0;
    if (value >= level)
    {
        mark = round( fabs(level - value) / delimeter );
    }
    return mark;
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
        this->f0max = mm.max;
        this->f0min = mm.min;
        qDebug() << "MinMax " << this->f0min << ":" << this->f0max << LOG_DATA;

        this->relativeRegF0 = 1.0 * (this->f0max + this->f0min) / 2.0;
        this->relativeDiapF0 = 1.0 * this->f0max / this->f0min;
        this->relativeTempo = pitch_cutted.x;

        vector intensive_cutted = copyv(this->simple_data->d_intensive);
        applyMask(&intensive_cutted, &this->simple_data->d_mask);
        this->relativeVolume = midv(intensive_cutted);
        freev(intensive_cutted);

        vector pitch_smooth;
        if (sptk_settings->dp->umpSmoothType == 0)
        {
            pitch_smooth = vector_smooth_lin(pitch_cutted, sptk_settings->dp->umpSmoothValue);
        } else if (sptk_settings->dp->umpSmoothType == 1) {
            pitch_smooth = vector_smooth_mid(pitch_cutted, sptk_settings->dp->umpSmoothValue);
        } else {
            pitch_smooth = copyv(pitch_cutted);
        }

        vector pitch_norm = normalizev(pitch_smooth, 0.0, 1.0);
        this->meanValueUMP = midv(pitch_norm);

        this->rootMeanSquareUMP = 0.0;
        double centricGravityPX = 0.0;
        double centricGravityP = 0.0;
        for (int i=0; i<pitch_norm.x; i++)
        {
            this->rootMeanSquareUMP += pow(getv(pitch_norm, i) - this->meanValueUMP, 2);
            centricGravityPX += getv(pitch_norm, i) * (i+1);
            centricGravityP += getv(pitch_norm, i);
        }
        this->rootMeanSquareUMP = sqrt(this->rootMeanSquareUMP);
        freev(pitch_norm);
        this->centricGravityUMP = centricGravityPX / centricGravityP;

        this->pitchData = createMglData(pitch_smooth, this->pitchData, true);
        this->pitchData->Norm();
        qDebug() << "pitchData createMglData" << LOG_DATA;        

        derivative derivative_data = get_derivative_data(pitch_smooth, sptk_settings->dp->umpSmoothValue);
        vector derivative_pitch = derivative_data.data;
        this->pitchDataDerivative = createMglData(derivative_data.data, this->pitchDataDerivative, true);
        this->pitchDataDerivative->Norm();
        this->pitchDataDerivativeZero = derivative_data.zero;
        qDebug() << "pitchDataDerivative createMglData" << LOG_DATA;

        this->rt = (1.0 * this->f0max / this->f0min) - 1;

        this->octavData = new mglData(2);
        qDebug() << "this->f0max " << this->f0max << LOG_DATA;
        qDebug() << "this->f0min " << this->f0min << LOG_DATA;
        this->octavData->a[0] = this->rt;
        qDebug() << "this->octavData->a[0] " << this->octavData->a[0] << LOG_DATA;
        if(this->octavData->a[0] > OCTAVE_MAX_2) this->octavData->a[0] = OCTAVE_MAX_2;
        qDebug() << "octavData createMglData" << LOG_DATA;

        double mask_scale = 1.0 * this->simple_data->d_full_wave.x / this->simple_data->d_mask.x;
        qDebug() << "mask_scale " << mask_scale << LOG_DATA;

        if (sptk_settings->dp->showF0 || !sptk_settings->dp->showDerivativeF0)
        {
            vector origin_ump = copyv(pitch_smooth);
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

            qDebug() << "ump_mask " << ump_mask.x << LOG_DATA;
            qDebug() << "origin_ump " << origin_ump.x << LOG_DATA;

            this->umpData = createMglData(origin_ump, this->umpData);
            this->umpData->Norm();
            qDebug() << "umpData createMglData" << LOG_DATA;

            freev(origin_ump);
            qDebug() << "freev origin_ump" << LOG_DATA;

            this->umpMask = createMglData(ump_mask, this->umpMask, true);
            qDebug() << "umpMask createMglData" << LOG_DATA;
        }

        if (sptk_settings->dp->showDerivativeF0)
        {
            vector derivative_ump = copyv(derivative_pitch);
            vector derivative_ump_mask = makeUmp(
                &derivative_ump,
                &this->simple_data->d_mask,
                this->simple_data->md_p,
                this->simple_data->md_n,
                this->simple_data->md_t,
                mask_scale,
                sptk_settings->dp->portLen,
                sptk_settings->dp->useStripUmp
            );

            qDebug() << "derivative_ump_mask " << derivative_ump_mask.x << LOG_DATA;
            qDebug() << "derivative_ump " << derivative_ump.x << LOG_DATA;

            this->umpDerivativeData = createMglData(derivative_ump, this->umpDerivativeData);
            this->umpDerivativeData->Norm();
            qDebug() << "umpData createMglData" << LOG_DATA;

            freev(derivative_ump);
            qDebug() << "freev derivative_ump" << LOG_DATA;

            if (this->umpMask == NULL)
            {
                this->umpMask = createMglData(derivative_ump_mask, this->umpMask, true);
                qDebug() << "umpMask createMglData" << LOG_DATA;
            }
        }

        freev(pitch_cutted);
        qDebug() << "freev pitch_cutted" << LOG_DATA;
        freev(pitch_smooth);
        qDebug() << "freev pitch_smooth" << LOG_DATA;

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
        this->proximity_curve_local = 0;
        this->proximity_average = 0;
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
        if (sptk_settings->dp->useForDP == DP_USE_SPECTRUM)
        {
            int speksize = sptk_settings->spec->leng / 2 + 1;
            qDebug() << "data->d_spec " << this->simple_data->d_spec.x << LOG_DATA;
            qDebug() << "dataSec->d_spec " << dataSec->d_spec.x << LOG_DATA;
            firstSignal = new SpectrSignal(copyv(this->simple_data->d_spec_proc), speksize);
            secondSignal = new SpectrSignal(copyv(dataSec->d_spec_proc), speksize);
        } else if (sptk_settings->dp->useForDP == DP_USE_CEPSTRUM) {
            int speksize = sptk_settings->lpc->cepstrum_order + 1;
            qDebug() << "data->d_cepstrum " << this->simple_data->d_cepstrum.x << LOG_DATA;
            qDebug() << "dataSec->d_cepstrum " << dataSec->d_cepstrum.x << LOG_DATA;
            firstSignal = new SpectrSignal(copyv(this->simple_data->d_cepstrum), speksize);
            secondSignal = new SpectrSignal(copyv(dataSec->d_cepstrum), speksize);
        } else if (sptk_settings->dp->useForDP == DP_USE_CEPSTRUM_A0) {
            int speksize = sptk_settings->lpc->cepstrum_order + 1;
            int additional_data = speksize * sptk_settings->dp->dpA0Coeficient;
            int analysed_data = speksize + additional_data;
            qDebug() << "speksize " << speksize << LOG_DATA;
            qDebug() << "data->d_cepstrum " << this->simple_data->d_cepstrum.x << LOG_DATA;
            qDebug() << "dataSec->d_cepstrum " << dataSec->d_cepstrum.x << LOG_DATA;
            vector simple_merged_data = mergev(this->simple_data->d_cepstrum, this->simple_data->d_intensive_norm, additional_data);
            firstSignal = new SpectrSignal(copyv(simple_merged_data), analysed_data);
            freev(simple_merged_data);
            vector merged_data = mergev(dataSec->d_cepstrum, dataSec->d_intensive_norm, additional_data);
            secondSignal = new SpectrSignal(copyv(merged_data), analysed_data);
            freev(merged_data);
        } else if (sptk_settings->dp->useForDP == DP_USE_CEPSTRUM_LOGF0) {
            int speksize = sptk_settings->lpc->cepstrum_order + 1;
            int additional_data = speksize * sptk_settings->dp->dpF0Coeficient;
            int analysed_data = speksize + additional_data;
            qDebug() << "data->d_cepstrum " << this->simple_data->d_cepstrum.x << LOG_DATA;
            qDebug() << "dataSec->d_cepstrum " << dataSec->d_cepstrum.x << LOG_DATA;
            vector simple_merged_data = mergev(this->simple_data->d_cepstrum, this->simple_data->d_pitch_log, additional_data);
            firstSignal = new SpectrSignal(copyv(simple_merged_data), analysed_data);
            freev(simple_merged_data);
            vector merged_data = mergev(dataSec->d_cepstrum, dataSec->d_pitch_log, additional_data);
            secondSignal = new SpectrSignal(copyv(merged_data), analysed_data);
            freev(merged_data);
        } else if (sptk_settings->dp->useForDP == DP_USE_CEPSTRUM_A0_LOGF0) {
            int speksize = sptk_settings->lpc->cepstrum_order + 1;
            int additional_data_a0 = speksize * sptk_settings->dp->dpA0Coeficient;
            int additional_data_f0 = speksize * sptk_settings->dp->dpF0Coeficient;
            int analysed_data = speksize + additional_data_a0 + additional_data_f0;
            qDebug() << "data->d_cepstrum " << this->simple_data->d_cepstrum.x << LOG_DATA;
            qDebug() << "dataSec->d_cepstrum " << dataSec->d_cepstrum.x << LOG_DATA;
            vector simple_merged_data = mergev(this->simple_data->d_cepstrum, this->simple_data->d_pitch_log, additional_data_f0);
            simple_merged_data = mergev(simple_merged_data, this->simple_data->d_intensive_norm, additional_data_a0);
            firstSignal = new SpectrSignal(copyv(simple_merged_data), analysed_data);
            freev(simple_merged_data);
            vector merged_data = mergev(dataSec->d_cepstrum, dataSec->d_pitch_log, additional_data_f0);
            merged_data = mergev(merged_data, dataSec->d_intensive_norm, additional_data_a0);
            secondSignal = new SpectrSignal(copyv(merged_data), analysed_data);
            freev(merged_data);
        } else if (sptk_settings->dp->useForDP == DP_USE_CEPSTRUM_DA0) {
            int speksize = sptk_settings->lpc->cepstrum_order + 1;
            int additional_data = speksize * sptk_settings->dp->dpDA0Coeficient;
            int analysed_data = speksize + additional_data;
            qDebug() << "speksize " << speksize << LOG_DATA;
            qDebug() << "data->d_cepstrum " << this->simple_data->d_cepstrum.x << LOG_DATA;
            qDebug() << "dataSec->d_cepstrum " << dataSec->d_cepstrum.x << LOG_DATA;
            vector simple_merged_data = mergev(this->simple_data->d_cepstrum, this->simple_data->d_derivative_intensive_norm, additional_data);
            firstSignal = new SpectrSignal(copyv(simple_merged_data), analysed_data);
            freev(simple_merged_data);
            vector merged_data = mergev(dataSec->d_cepstrum, dataSec->d_derivative_intensive_norm, additional_data);
            secondSignal = new SpectrSignal(copyv(merged_data), analysed_data);
            freev(merged_data);
        } else if (sptk_settings->dp->useForDP == DP_USE_CEPSTRUM_A0_DA0) {
            int speksize = sptk_settings->lpc->cepstrum_order + 1;
            int additional_data_a0 = speksize * sptk_settings->dp->dpA0Coeficient;
            int additional_data_da0 = speksize * sptk_settings->dp->dpDA0Coeficient;
            int analysed_data = speksize + additional_data_a0 + additional_data_da0;
            qDebug() << "data->d_cepstrum " << this->simple_data->d_cepstrum.x << LOG_DATA;
            qDebug() << "dataSec->d_cepstrum " << dataSec->d_cepstrum.x << LOG_DATA;
            vector simple_merged_data = mergev(this->simple_data->d_cepstrum, this->simple_data->d_derivative_intensive_norm, additional_data_da0);
            simple_merged_data = mergev(simple_merged_data, this->simple_data->d_intensive_norm, additional_data_a0);
            firstSignal = new SpectrSignal(copyv(simple_merged_data), analysed_data);
            freev(simple_merged_data);
            vector merged_data = mergev(dataSec->d_cepstrum, dataSec->d_derivative_intensive_norm, additional_data_da0);
            merged_data = mergev(merged_data, dataSec->d_intensive_norm, additional_data_a0);
            secondSignal = new SpectrSignal(copyv(merged_data), analysed_data);
            freev(merged_data);
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
            qDebug() << "showError" << LOG_DATA;
            this->errorData = createMglData(errorVector, this->errorData);
            this->errorData->Norm();
        }

        if (sptk_settings->dp->showTime)
        {
            qDebug() << "showTime" << LOG_DATA;
            this->timeData = createMglData(timeVector, this->timeData);
            this->timeData->Norm();
        }

        int startPos = endPos - getv(timeVector, endPos);
        qDebug() << "endPos" << endPos << LOG_DATA;
        qDebug() << "startPos" << startPos << LOG_DATA;
        vector dpVector = zerov(errorVector.x);
        setMark(&dpVector, startPos);
        setMark(&dpVector, endPos);
        this->dpData = createMglData(dpVector, this->dpData, true);
        freev(dpVector);

        freev(errorVector);

        qDebug() << "getting mapping " << endPos << LOG_DATA;
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

        this->userRelativeRegF0 = 1.0 * (this->userf0max + this->userf0min) / 2.0;
        this->userRelativeDiapF0 = 1.0 * this->userf0max / this->userf0min;
        this->userRelativeTempo = pitch_cutted.x;

        vector intensive_cutted = copyv(dataSec->d_intensive);
        applyMask(&intensive_cutted, &this->simple_data->d_mask);
        this->userRelativeVolume = midv(intensive_cutted);
        freev(intensive_cutted);

        vector pitch_smooth;
        if (sptk_settings->dp->umpSmoothType == 0)
        {
            pitch_smooth = vector_smooth_lin(pitch_cutted, sptk_settings->dp->umpSmoothValue);
        } else if (sptk_settings->dp->umpSmoothType == 1) {
            pitch_smooth = vector_smooth_mid(pitch_cutted, sptk_settings->dp->umpSmoothValue);
        } else {
            pitch_smooth = copyv(pitch_cutted);
        }

        vector pitch_norm = normalizev(pitch_smooth, 0.0, 1.0);
        this->userMeanValueUMP = midv(pitch_norm);

        this->userRootMeanSquareUMP = 0.0;
        double centricGravityPX = 0.0;
        double centricGravityP = 0.0;
        for (int i=0; i<pitch_norm.x; i++)
        {
            this->userRootMeanSquareUMP += pow(getv(pitch_norm, i) - this->userMeanValueUMP, 2);
            centricGravityPX += getv(pitch_norm, i) * i;
            centricGravityP += getv(pitch_norm, i);
        }
        this->userRootMeanSquareUMP = sqrt(this->userRootMeanSquareUMP);
        freev(pitch_norm);
        this->userCentricGravityUMP = centricGravityPX / centricGravityP;

        this->ru = (1.0 * this->userf0max / this->userf0min) - 1;

        this->secOctavData = new mglData(2);
        this->secOctavData->a[1] = this->ru;
        if(this->secOctavData->a[1] > OCTAVE_MAX_2) this->secOctavData->a[1] = OCTAVE_MAX_2;

        if(sptk_settings->dp->showF0)
        {
            this->secPitchData = createMglData(pitch_smooth, this->secPitchData, true);
        }

        derivative derivative_data = get_derivative_data(pitch_smooth, sptk_settings->dp->umpSmoothValue);
        vector derivative_pitch = derivative_data.data;
        this->secPitchDataDerivative = createMglData(derivative_data.data, this->secPitchDataDerivative, true);
        this->secPitchDataDerivative->Norm();
        this->secPitchDataDerivativeZero = derivative_data.zero;
        qDebug() << "secPitchDataDerivative createMglData" << LOG_DATA;

        this->proximity_range = round( 100.0 * MIN(this->rt, this->ru) / MAX(this->rt, this->ru) );
        this->proximity_range_mark = calculateMark(proximity_range, sptk_settings->dp->mark_level, sptk_settings->dp->mark_delimeter);

        double mask_scale = 1.0 * this->simple_data->d_full_wave.x / this->simple_data->d_mask.x;
        qDebug() << "mask_scale " << mask_scale << LOG_DATA;

        this->proximity_curve_correlation = 0.0;
        this->proximity_curve_integral = 0.0;
        this->proximity_curve_local = 0.0;
        this->proximity_average = 0.0;
        this->proximity_curve_shape = 0.0;
        this->proximity_shape_mark = 0.0;

        if (sptk_settings->dp->showF0 || !sptk_settings->dp->showDerivativeF0)
        {
            vector sec_ump = copyv(pitch_smooth);

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

            this->proximity_curve_correlation = calculateCurvesSimilarityCorrelation(ump, sec_ump);
            this->proximity_curve_integral = calculateCurvesSimilarityAverageDistance(ump, sec_ump);
            this->proximity_curve_local = calculateCurvesSimilarityMaxLocalDistance(ump, sec_ump);
            this->proximity_average = round((this->proximity_curve_correlation + this->proximity_curve_integral + this->proximity_curve_local) / 3.0);

            switch (sptk_settings->dp->errorType) {
            case 0:
                this->proximity_curve_shape = proximity_curve_correlation;
                break;
            case 1:
                this->proximity_curve_shape = proximity_curve_integral;
                break;
            case 2:
                this->proximity_curve_shape = proximity_curve_local;
                break;
            case 3:
                this->proximity_curve_shape = proximity_average;
                break;
            }

            this->proximity_shape_mark = calculateMark(proximity_curve_shape, sptk_settings->dp->mark_level, sptk_settings->dp->mark_delimeter);

            freev(sec_ump);
        }

        if (sptk_settings->dp->showDerivativeF0)
        {
            vector derivative_sec_ump = copyv(derivative_pitch);

            makeUmp(
                &derivative_sec_ump,
                &this->simple_data->d_mask,
                this->simple_data->md_p,
                this->simple_data->md_n,
                this->simple_data->md_t,
                mask_scale,
                sptk_settings->dp->portLen,
                sptk_settings->dp->useStripUmp
            );

            this->secUmpDerivativeData = createMglData(derivative_sec_ump, this->secUmpDerivativeData);
            this->secUmpDerivativeData->Norm();

            vector derivative_ump;
            derivative_ump.x = this->umpDerivativeData->nx;
            derivative_ump.v = this->umpDerivativeData->a;

            double derivative_proximity_curve_correlation = calculateCurvesSimilarityCorrelation(derivative_ump, derivative_sec_ump);
            double derivative_proximity_curve_integral = calculateCurvesSimilarityAverageDistance(derivative_ump, derivative_sec_ump);
            double derivative_proximity_curve_local = calculateCurvesSimilarityMaxLocalDistance(derivative_ump, derivative_sec_ump);

            double derivative_proximity_average = round((derivative_proximity_curve_correlation + derivative_proximity_curve_integral + derivative_proximity_curve_local) / 3.0);

            double derivative_proximity_curve_shape;
            switch (sptk_settings->dp->errorType) {
            case 0:
                derivative_proximity_curve_shape = derivative_proximity_curve_correlation;
                break;
            case 1:
                derivative_proximity_curve_shape = derivative_proximity_curve_integral;
                break;
            case 2:
                derivative_proximity_curve_shape = derivative_proximity_curve_local;
                break;
            case 3:
                derivative_proximity_curve_shape = derivative_proximity_average;
                break;
            }

            double derivative_proximity_shape_mark = calculateMark(derivative_proximity_curve_shape, sptk_settings->dp->mark_level, sptk_settings->dp->mark_delimeter);

            this->proximity_curve_correlation += derivative_proximity_curve_correlation;
            this->proximity_curve_integral += derivative_proximity_curve_integral;
            this->proximity_curve_local += derivative_proximity_curve_local;
            this->proximity_average += derivative_proximity_average;
            this->proximity_curve_shape += derivative_proximity_curve_shape;
            this->proximity_shape_mark += derivative_proximity_shape_mark;

            if (sptk_settings->dp->showF0)
            {
                this->proximity_curve_correlation /= 2.0;
                this->proximity_curve_integral /= 2.0;
                this->proximity_curve_local /= 2.0;
                this->proximity_average /= 2.0;
                this->proximity_curve_shape /= 2.0;
                this->proximity_shape_mark /= 2.0;
            }

            freev(derivative_sec_ump);
        }

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

QMap<QString, QVariant> DrawerDP::getStatisticData()
{
    QMap<QString, QVariant> data = Drawer::getStatisticData();

    data["Template F0 min"] = QVariant(this->f0min);
    data["Template F0 max"] = QVariant(this->f0max);
    data["Record F0 min"] = QVariant(this->userf0min);
    data["Record F0 max"] = QVariant(this->userf0max);
    data["Relative Tempo"] = QVariant(this->userRelativeTempo / this->relativeTempo * 100);
    data["Relative Volume"] = QVariant(this->userRelativeVolume / this->relativeVolume * 100);
    data["Relative Register F0"] = QVariant(this->userRelativeRegF0 / this->relativeRegF0 * 100);
    data["Relative Diapason F0"] = QVariant(this->userRelativeDiapF0 / this->relativeDiapF0 * 100);
    data["MeanValue UMP Recorded"] = QVariant(this->userMeanValueUMP);
    data["MeanValue UMP Template"] = QVariant(this->meanValueUMP);
    data["Root Mean Square UMP Recorded"] = QVariant(this->userRootMeanSquareUMP);
    data["Root Mean Square UMP Template"] = QVariant(this->rootMeanSquareUMP);
    data["Relative Root Mean Square UMP(%)"] = QVariant(this->userRootMeanSquareUMP / this->rootMeanSquareUMP * 100);
    data["CenterGravity UMP Recorded"] = QVariant(this->userCentricGravityUMP);
    data["CenterGravity UMP Template"] = QVariant(this->centricGravityUMP);

    return data;
}


