#include "drawerevalpitch.h"

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

DrawerEvalPitch::DrawerEvalPitch() :
    Drawer(),
    secFileName(""),
    result(0),
    first(true)
{
}

DrawerEvalPitch::~DrawerEvalPitch()
{
    qDebug() << "DrawerEval removed";
}

int DrawerEvalPitch::Draw(mglGraph *gr)
{
    qDebug() << "start drawing";

    gr->DefaultPlotParam();
    gr->Clf();

    qDebug() << "waveData";
    gr->MultiPlot(1, 11, 0, 1, 1, "#");
    gr->SetRange('y', waveMin, waveMax);
    gr->Plot(waveData, "-G");

    qDebug() << "pitchData";
    gr->MultiPlot(1, 11, 4, 1, 6, "#");
    gr->SetRange('y', 0, GRAPH_Y_VAL_MAX);
    gr->Plot(pitchData, "-G4");
    gr->Axis("Y", "");
    gr->Grid("y", "W", "");

    if(!this->secFileName.isEmpty()){
        qDebug() << "secWaveData";
        gr->MultiPlot(1, 11, 1, 1, 1, "#");
        gr->SetRange('y', waveMin, waveMax);
        gr->Plot(secWaveData, "B");

        gr->MultiPlot(1, 11, 3, 1, 1, "#");
        gr->Puts(mglPoint(0,0),QString("Your score: \\big{#r{%1}}").arg(this->result).toUtf8().data(), ":C", 50);

        qDebug() << "secPitchData";
        gr->MultiPlot(1, 11, 4, 1, 6, "#");
        gr->SetRange('y', 0, GRAPH_Y_VAL_MAX);
        gr->Plot(secPitchData, "-B4");

        qDebug() << "secPitchData";
        gr->MultiPlot(1, 11, 4, 1, 6, "#");
        gr->SetRange('y', 0, GRAPH_Y_VAL_MAX);
        gr->Plot(secPitchDataOrig, ".B1");
    }

    qDebug() << "finish drawing";
    return 0;
}

void DrawerEvalPitch::Proc(QString fname)
{    
    if(first)
    {
        qDebug() << "Drawer::Proc";
        Drawer::Proc(fname);
        pitchData.Norm(GRAPH_Y_VAL_MAX);
        first = false;
    }
    else
    {
        qDebug() << "DrawerEval::Proc";
        this->secFileName = fname;

        GraphData dataSec = ProcWave2Data(this->secFileName);
        dataSec.d_pitch = sptk_fill_empty(dataSec.d_pitch);

        vectorToData(dataSec.d_wave, &secWaveData);
        double min = secWaveData.Min("x").a[0];
        if(min < waveMin) waveMin = min;
        double max = secWaveData.Max("x").a[0];
        if(max > waveMax) waveMax = max;
        qDebug() << "waveData New Filled";

        vectorToData(dataSec.d_pitch, &secPitchDataOrig);
        secPitchDataOrig.Norm(GRAPH_Y_VAL_MAX);
        qDebug() << "pitchData New Filled";

        vector pitchOrig;
        pitchOrig.v = pitchData.a;
        pitchOrig.x = data->d_pitch.x;
        (*pitchOrig.v) = 0;

        vector pitch;
        pitch.v = secPitchDataOrig.a;
        pitch.x = dataSec.d_pitch.x;
        (*pitch.v) = 0;

        qDebug() << "Pitch sizes " << pitchOrig.x << " " << pitch.x;

        qDebug() << "Start DP";
        VectorDP dp(new VectorSignal(copyv(pitchOrig)), new VectorSignal(copyv(pitch)));
        vector newPitch = dp.getScaledSignal()->getArray();
        this->result = dp.getSignalMask()->value.globalError;
        qDebug() << "Stop DP";

        vectorToData(newPitch, &secPitchData);
        qDebug() << "pitchData New Filled";

        freev(newPitch);
        freeGraphData(dataSec);
        qDebug() << "New Data Processed";
    }
}
