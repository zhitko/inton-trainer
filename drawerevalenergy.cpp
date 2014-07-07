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

    qDebug() << "waveData";
    gr->MultiPlot(1, 10, 0, 1, 1, "#");
    gr->SetRange('y', waveMin, waveMax);
    gr->Plot(waveData, "-G");

    qDebug() << "enegryData";
    gr->MultiPlot(1, 10, 3, 1, 6, "#");
    gr->SetRange('y', 0, ENERGY_MAX);
    gr->Plot(intensiveData, "-G4");
    gr->Axis("Y", "");
    gr->Grid("y", "W", "");

    if(!this->secFileName.isEmpty()){
        qDebug() << "secWaveData";
        gr->MultiPlot(1, 10, 1, 1, 1, "#");
        gr->SetRange('y', waveMin, waveMax);
        gr->Plot(secWaveData, "B");

        qDebug() << "secEnegryData";
        gr->MultiPlot(1, 10, 3, 1, 6, "#");
        gr->SetRange('y', 0, ENERGY_MAX);
        gr->Plot(secIntensiveData, "-B4");

        qDebug() << "secEnegryDataOrig";
        gr->MultiPlot(1, 10, 3, 1, 6, "#");
        gr->SetRange('y', 0, ENERGY_MAX);
        gr->Plot(secIntensiveDataOrig, ".B1");
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
        qDebug() << "waveData New Filled";

        vectorToData(dataSec.d_intensive, &secIntensiveDataOrig);
        secIntensiveDataOrig.Norm(ENERGY_MAX);
        qDebug() << "secIntensiveDataOrig New Filled";

        vector intensive;
        intensive.v = secIntensiveDataOrig.a;
        intensive.x = dataSec.d_intensive.x;

        qDebug() << "Start DP";
        VectorDP dp(data->d_intensive, intensive);
        vector newIntensive = dp.getScaledSignal();
        this->result = dp.getSignalMask()->value.globalError;
        qDebug() << "Stop DP";

        vectorToData(newIntensive, &secIntensiveData);
//        secIntensiveData.Norm();
        qDebug() << "secIntensiveDataOrig New Filled";

        freev(newIntensive);
        freeGraphData(dataSec);
        qDebug() << "New Data Processed";
    }
}
