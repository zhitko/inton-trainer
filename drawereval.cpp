#include "drawereval.h"

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

DrawerEval::DrawerEval() :
    Drawer(),
    secFileName(""),
    result(0),
    first(true)
{
}

DrawerEval::~DrawerEval()
{
    qDebug() << "DrawerEval removed";
}

int DrawerEval::Draw(mglGraph *gr)
{
    qDebug() << "start drawing";

    gr->DefaultPlotParam();
    gr->Clf();

    qDebug() << "waveData";
    gr->MultiPlot(1, 10, 0, 1, 1, "#");
    gr->SetRange('y', waveMin, waveMax);
    gr->Plot(waveData, "-G");

    qDebug() << "pitchData";
    gr->MultiPlot(1, 10, 3, 1, 6, "#");
    gr->SetRange('y', pitchMin, pitchMax);
    gr->Plot(pitchData, "-G4");
    gr->Axis("Y", "");
    gr->Grid("y", "W", "");

    if(!this->secFileName.isEmpty()){
        qDebug() << "secWaveData";
        gr->MultiPlot(1, 10, 1, 1, 1, "#");
        gr->SetRange('y', waveMin, waveMax);
        gr->Plot(secWaveData, "B");

        qDebug() << "secPitchData";
        gr->MultiPlot(1, 10, 3, 1, 6, "#");
        gr->SetRange('y', pitchMin, pitchMax);
        gr->Plot(secPitchData, "-B4");

        qDebug() << "secPitchData";
        gr->MultiPlot(1, 10, 3, 1, 6, "#");
        gr->SetRange('y', pitchMin, pitchMax);
        gr->Plot(secPitchDataOrig, ".B1");
    }

    qDebug() << "finish drawing";
    return 0;
}

void DrawerEval::Proc(QString fname)
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
        dataSec.d_pitch = sptk_fill_empty(dataSec.d_pitch);

        vectorToData(dataSec.d_wave, &secWaveData);
        qDebug() << "waveData New Filled";

        vectorToData(dataSec.d_pitch, &secPitchDataOrig);
        double min = secPitchDataOrig.Min("x").a[0];
        double max = secPitchDataOrig.Max("x").a[0];
        if(pitchMin > min) pitchMin = min;
        if(pitchMax < max) pitchMax = max;
        qDebug() << "pitchData New Filled";

        qDebug() << "Start DP";
        VectorDP dp(data->d_pitch, dataSec.d_pitch);
        vector newPitch = dp.getScaledSignal();
        this->result = dp.getSignalMask()->value.globalError;
        qDebug() << "Stop DP";

        vectorToData(newPitch, &secPitchData);
        qDebug() << "pitchData New Filled";

        freev(newPitch);
        freeGraphData(dataSec);
        qDebug() << "New Data Processed";
    }
}
