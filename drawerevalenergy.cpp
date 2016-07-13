#include "drawerevalenergy.h"

#include <QFile>
#include <QDebug>

#include "settingsdialog.h"

#include "DP/vectordp.h"

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

DrawerEvalEnergy::DrawerEvalEnergy() :
    Drawer(),
    secFileName(""),
    result(0),
    first(true)
{
}

DrawerEvalEnergy::~DrawerEvalEnergy()
{
    qDebug() << "DrawerEvalEnergy removed";
}

int DrawerEvalEnergy::Draw(mglGraph *gr)
{
    qDebug() << "start drawing";

    gr->DefaultPlotParam();
    gr->Clf();

    gr->AddLegend(QString("Энергия образца").toLocal8Bit().data(),"-g1");
    gr->AddLegend(QString("Ln(F0)").toLocal8Bit().data(),"-r1");
    gr->AddLegend(QString("Ср. энергия").toLocal8Bit().data(),"-m1");
    gr->AddLegend(QString("Энергия записи").toLocal8Bit().data(),"-B2");
//    gr->AddLegend(QString("Ср. энергия записи").toLocal8Bit().data(),"jB1");
    gr->AddLegend(QString("Оригинальня энергия записи").toLocal8Bit().data(),"-n1");
    gr->Legend(0,"-A");

    qDebug() << "waveData";
    gr->MultiPlot(1, 12, 0, 1, 1, "#");
    gr->SetRange('y', waveMin, waveMax);
    gr->Plot(waveData, "-G");

    qDebug() << "enegryData";
    gr->MultiPlot(1, 12, 4, 1, 6, "#");
    gr->SetRange('y', 0, GRAPH_Y_VAL_MAX);
    gr->Plot(intensiveData, "-g1");
    gr->Plot(midIntensiveData, "-m1");
    gr->Plot(logData, "-r1");

    gr->Axis("Y", "");
    gr->Grid("y", "W", "");

    gr->MultiPlot(1, 12, 4, 1, 6, "#");

    if(!this->secFileName.isEmpty()){
        qDebug() << "secWaveData";
        gr->MultiPlot(1, 12, 1, 1, 1, "#");
        gr->SetRange('y', waveMin, waveMax);
        gr->Plot(secWaveData, "B");

        gr->MultiPlot(1, 12, 3, 1, 1, "#");
        gr->Puts(mglPoint(0,0),QString("Your score: \\big{#r{%1}}").arg(this->result).toUtf8().data(), ":C", 50);

        qDebug() << "secEnegryData";
        gr->MultiPlot(1, 12, 4, 1, 6, "#");
        gr->SetRange('y', 0, GRAPH_Y_VAL_MAX);
        gr->Plot(secIntensiveData, "-B2");
//        gr->Plot(secMidIntensiveData, "jB1");

        qDebug() << "secEnegryDataOrig";
        gr->MultiPlot(1, 12, 4, 1, 6, "#");
        gr->SetRange('y', 0, GRAPH_Y_VAL_MAX);
        gr->Plot(secIntensiveDataOrig, "-n1");
    }

    qDebug() << "finish drawing";
    return 0;
}

void DrawerEvalEnergy::Proc(QString fname)
{
    if(first)
    {
        qDebug() << "Drawer::Proc";
        Drawer::Proc(fname);
        first = false;
    }
    else
    {
        qDebug() << "DrawerEval::Proc";
        this->secFileName = fname;

        GraphData dataSec = ProcWave2Data(this->secFileName);

        vectorToData(dataSec.d_wave, &secWaveData);
        double min = secWaveData.Min("x").a[0];
        if(min < waveMin) waveMin = min;
        double max = secWaveData.Max("x").a[0];
        if(max > waveMax) waveMax = max;
        qDebug() << "waveData New Filled";

        vectorToData(dataSec.d_intensive, &secIntensiveDataOrig);
        secIntensiveDataOrig.Norm(GRAPH_Y_VAL_MAX);
        qDebug() << "secIntensiveDataOrig New Filled";

        vector intensiveOrig;
        intensiveOrig.v = intensiveData.a;
        intensiveOrig.x = data->d_intensive.x;
        (*intensiveOrig.v) = 0;

        vector intensive;
        intensive.v = secIntensiveDataOrig.a;
        intensive.x = dataSec.d_intensive.x;
        (*intensive.v) = 0;

        qDebug() << "Start DP";
        VectorDP dp(new VectorSignal(copyv(intensiveOrig)), new VectorSignal(copyv(intensive)));
        vector newIntensive = dp.getScaledSignal()->getArray();
        this->result = dp.getSignalMask()->value.globalError;
        qDebug() << "Stop DP";

        vectorToData(newIntensive, &secIntensiveData);
        qDebug() << "secIntensiveDataOrig New Filled";

        freev(newIntensive);
        freeGraphData(dataSec);
        qDebug() << "New Data Processed";
    }
}
