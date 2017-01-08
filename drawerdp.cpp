#include "drawerdp.h"

#include <QFile>
#include <QDebug>

#include "settingsdialog.h"

#include "DP/continuousdp.h"

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
}

DrawerDP::DrawerDP() :
    Drawer(),
    secFileName(""),
    first(true)
{
    this->dpData = NULL;
    this->secWaveData = NULL;
    this->errorData = NULL;
    this->timeData = NULL;
}

DrawerDP::~DrawerDP()
{
    qDebug() << "DrawerDP removed";
    if (this->dpData) delete this->dpData;
    if (this->secWaveData) delete this->secWaveData;
    if (this->errorData) delete this->errorData;
    if (this->timeData) delete this->timeData;
}

int DrawerDP::Draw(mglGraph *gr)
{
    qDebug() << "start drawing";

    bool isCompare = !this->secFileName.isEmpty();

    gr->DefaultPlotParam();
    gr->Clf();

    qDebug() << "waveData";
    gr->MultiPlot(1, 12, 0, 1, 1, "#");
    gr->SetRange('y', 0, 1);
    gr->Plot(*waveData, "B");
    gr->Plot(*pWaveData, "y1");
    gr->Plot(*nWaveData, "q1");
    gr->Plot(*tWaveData, "c1");

    gr->MultiPlot(1, 12, 4, 1, 6, "#");
    gr->SetRange('y', 0, 1);
    gr->Axis("Y", "");
    gr->Grid("y", "W", "");

    if(isCompare){
        qDebug() << "secWaveData";
        gr->MultiPlot(1, 12, 1, 1, 1, "#");
        gr->SetRange('y', 0, 1);
        gr->Plot(*secWaveData, "G");
        gr->Plot(*dpData, "R9");

        gr->MultiPlot(1, 12, 3, 1, 1, "#");
        gr->Puts(mglPoint(0,0),QString("Max: %1 Min: %2").arg(this->errorMax).arg(this->errorMin).toLocal8Bit().data(), ":C", 24);

        qDebug() << "errorData";
        gr->MultiPlot(1, 12, 4, 1, 6, "#");
        gr->SetRange('y', 0, 1);
        gr->Plot(*errorData, "-B3");
        gr->Plot(*timeData, "-R3");
    }

    qDebug() << "finish drawing";
    return 0;
}

void setMark(vector vec, int pos)
{
    vec.v[pos] = 1.0;
    if (pos != 0)
    {
        vec.v[pos-1] = 0.0001;
    }
    if (vec.x != pos + 1)
    {
        vec.v[pos+1] = 0.0001;
    }
}

void DrawerDP::Proc(QString fname)
{
    if(first)
    {
        qDebug() << "Drawer::Proc";
        Drawer::Proc(fname);
        first = false;
    }
    else
    {
        qDebug() << "DrawerDP::Proc";
        this->secFileName = fname;

        GraphData dataSec = ProcWave2Data(this->secFileName);

        secWaveData = createMglData(dataSec.d_full_wave, secWaveData);
        qDebug() << "waveData New Filled";

        qDebug() << "Start DP";
        SPTK_SETTINGS * sptk_settings = SettingsDialog::getSPTKsettings();
        int speksize = sptk_settings->spec->leng / 2 + 1;
        ContinuousDP dp(
            new SpectrSignal(copyv(this->data->d_spec), speksize),
            new SpectrSignal(copyv(dataSec.d_spec), speksize),
            1,
            sptk_settings->dp->continiusLimit
        );
        dp.calculate();
        qDebug() << "Stop DP";
        vector errorVector = dp.getErrorVector();
        int minPos = minv(errorVector);
        this->errorMax = errorVector.v[maxv(errorVector)];
        this->errorMin = errorVector.v[minPos];
        errorData = createMglData(errorVector, errorData);
        errorData->Norm();

        vector timeVector = dp.getTimeVector();
        timeData = createMglData(timeVector, timeData);
        timeData->Norm();

        qDebug() << "errorVector " << errorVector.x;
        qDebug() << "timeVector " << timeVector.x;

        vector dpVector = zerov(errorVector.x);
        setMark(dpVector, minPos);
        setMark(dpVector, minPos - timeVector.v[minPos]);
        dpData = createMglData(dpVector, dpData, true);

        freev(errorVector);
        freev(timeVector);
        freev(dpVector);
        freeGraphData(dataSec);
        qDebug() << "New Data Processed";
    }
}


