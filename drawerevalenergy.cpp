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
    secWaveData = NULL;
    secIntensiveData = NULL;
    secIntensiveDataOrig = NULL;
}

DrawerEvalEnergy::~DrawerEvalEnergy()
{
    if (secWaveData) delete secWaveData;
    if (secIntensiveData) delete secIntensiveData;
    if (secIntensiveDataOrig) delete secIntensiveDataOrig;
    qDebug() << "DrawerEvalEnergy removed";
}

int DrawerEvalEnergy::Draw(mglGraph *gr)
{
    qDebug() << "start drawing";

    bool isCompare = !this->secFileName.isEmpty();

    gr->DefaultPlotParam();
    gr->Clf();

    if(!isCompare) gr->AddLegend(QString("Энергия образца (без обработки)").toLocal8Bit().data(),"-b1");
    gr->AddLegend(QString("Энергия образца").toLocal8Bit().data(),"-B3");
    if(isCompare) gr->AddLegend(QString("Энергия записи").toLocal8Bit().data(),"-G3");
    gr->Legend(0,"-A");

    qDebug() << "waveData";
    gr->MultiPlot(1, 12, 0, 1, 1, "#");
    gr->SetRange('y', 0, 1);
    gr->Plot(*waveData, "-B");
    gr->Plot(*pWaveData, "-y1");
    gr->Plot(*nWaveData, "-q1");
    gr->Plot(*tWaveData, "-c1");

    qDebug() << "enegryData";
    gr->MultiPlot(1, 12, 4, 1, 6, "#");
    gr->SetRange('y', 0, 1);
    if(!isCompare) gr->Plot(*intensiveDataOriginal, "-b1");
    gr->Plot(*intensiveData, "-B3");

    qDebug() << "scaledMaskData";
    gr->SetRange('y', 0, 1);
    gr->Plot(*maskData, "-G1");

    gr->Axis("Y", "");
    gr->Grid("y", "W", "");

    gr->MultiPlot(1, 12, 4, 1, 6, "#");

    if(isCompare){
        qDebug() << "secWaveData";
        gr->MultiPlot(1, 12, 1, 1, 1, "#");
        gr->SetRange('y', 0, 1);
        gr->Plot(*secWaveData, "G");

        gr->MultiPlot(1, 12, 3, 1, 1, "#");
        gr->Puts(mglPoint(0,0),QString("Совпадение: \\big{#r{%1}}%").arg(this->result).toLocal8Bit().data(), ":C", 30);

        qDebug() << "secEnegryData";
        gr->MultiPlot(1, 12, 4, 1, 6, "#");
        gr->SetRange('y', 0, 1);
        gr->Plot(*secIntensiveData, "-G3");
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

        GraphData * dataSec = ProcWave2Data(this->secFileName);

        secWaveData = createMglData(dataSec->d_full_wave, secWaveData);
        qDebug() << "waveData New Filled";

        secIntensiveDataOrig = createMglData(dataSec->d_intensive, secIntensiveDataOrig);
        secIntensiveDataOrig->Norm();
        qDebug() << "secIntensiveDataOrig New Filled";

        vector intensiveOrig;
        intensiveOrig.v = intensiveData->a;
        intensiveOrig.x = data->d_intensive.x;
        (*intensiveOrig.v) = 0;

        vector intensive;
        intensive.v = secIntensiveDataOrig->a;
        intensive.x = dataSec->d_intensive.x;
        (*intensive.v) = 0;

        qDebug() << "Start DP";
        VectorDP dp(new VectorSignal(copyv(intensiveOrig)), new VectorSignal(copyv(intensive)));
        vector newIntensive = dp.getScaledSignal()->getArray();
        this->result = calcResultMark(newIntensive,intensiveOrig);
        qDebug() << "Stop DP";

        secIntensiveData = createMglData(newIntensive, secIntensiveData);
        qDebug() << "secIntensiveDataOrig New Filled";

        freev(newIntensive);
        freeGraphData(dataSec);
        qDebug() << "New Data Processed";
    }
}
