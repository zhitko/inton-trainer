#include "drawerevalpitch.h"

#include <QFile>
#include <QDebug>

#include "settingsdialog.h"

#include "DP/vectordp.h"
#include "DP/spectrdp.h"

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
    #include "./SPTK/others/func.h"
    #include "./SPTK/others/interpolation.h"
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

    bool isCompare = !this->secFileName.isEmpty();

    gr->DefaultPlotParam();
    gr->Clf();

    if(!isCompare) gr->AddLegend(QString("F0 образца (без обработки)").toLocal8Bit().data(),"-b1");
    gr->AddLegend(QString("F0 образца").toLocal8Bit().data(),"-B3");
    if(isCompare) gr->AddLegend(QString("F0 записи").toLocal8Bit().data(),"-G3");
    gr->Legend(0,"-A");

    qDebug() << "waveData";
    gr->MultiPlot(1, 12, 0, 1, 1, "#");
    gr->SetRange('y', 0, 1);
    gr->Plot(waveData, "B");
    gr->Plot(pWaveData, "y1");
    gr->Plot(nWaveData, "q1");
    gr->Plot(tWaveData, "c1");

    qDebug() << "pitchData";
    gr->MultiPlot(1, 12, 4, 1, 6, "#");
    gr->Puts(mglPoint(-0.9,1),QString("%1").arg(data->pitch_max).toLocal8Bit().data());
    gr->SetRange('y', 0, GRAPH_Y_VAL_MAX);
    gr->Plot(pitchData, "-B3");
    if(!isCompare) gr->Plot(pitchDataOriginal, "-b1");
    gr->Axis("Y", "");
    gr->Grid("y", "W", "");
    gr->Puts(mglPoint(-0.9,-1),QString("%1").arg(data->pitch_min).toLocal8Bit().data());

    qDebug() << "scaledMaskData";
    gr->SetRange('y', 0, 1);
    gr->Plot(maskData, "-G1");

    if(isCompare){
        qDebug() << "secWaveData";
        gr->MultiPlot(1, 12, 1, 1, 1, "#");
        gr->SetRange('y', 0, 1);
        gr->Plot(secWaveData, "G");
        gr->Plot(pSecWaveData, "y1");
        gr->Plot(nSecWaveData, "q1");
        gr->Plot(tSecWaveData, "c1");

        gr->MultiPlot(1, 12, 3, 1, 1, "#");
        gr->Puts(mglPoint(0,0),QString("Совпадение: \\big{#r{%1}}%").arg(this->result).toLocal8Bit().data(), ":C", 30);

        qDebug() << "secPitchData";
        gr->MultiPlot(1, 12, 4, 1, 6, "#");
        gr->SetRange('y', 0, GRAPH_Y_VAL_MAX);
        gr->Plot(secPitchData, "-G3");
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
        first = false;
    }
    else
    {
        qDebug() << "DrawerEval::Proc";
        this->secFileName = fname;

        GraphData dataSec = ProcWave2Data(this->secFileName);
        dataSec.d_pitch = vector_fill_empty(dataSec.d_pitch);

        vectorToData(dataSec.d_full_wave, &secWaveData);
        qDebug() << "waveData New Filled";

        vectorToData(dataSec.d_pitch, &secPitchDataOrig);
        secPitchDataOrig.Norm(GRAPH_Y_VAL_MAX);
        qDebug() << "pitchData New Filled";

        vector pitchOrig;
        pitchOrig.v = pitchData.a;
        pitchOrig.x = data->d_pitch.x;
        (*pitchOrig.v) = 0;

        vector pitchOrigNorm = normalizev(pitchOrig, 0.0, 10.0);

        vector pitch;
        pitch.v = secPitchDataOrig.a;
        pitch.x = dataSec.d_pitch.x;
        (*pitch.v) = 0;

        vector pitchNorm = normalizev(pitch, 0.0, 10.0);

        qDebug() << "Pitch sizes " << pitchOrig.x << " " << pitch.x;

        qDebug() << "Start DP";
        VectorDP dp(new VectorSignal(copyv(pitchOrigNorm)), new VectorSignal(copyv(pitchNorm)));
        vector newPitch = dp.getScaledSignal()->getArray();
        this->result = calcResultMark(newPitch,pitchOrigNorm, dp.getSignalMask()->value.globalError);

        qDebug() << "Stop DP";

        vectorToData(newPitch, &secPitchData);
        secPitchData.Norm(GRAPH_Y_VAL_MAX);
        qDebug() << "pitchData New Filled";

        freev(newPitch);
        freeGraphData(dataSec);
        qDebug() << "New Data Processed";
    }
}
