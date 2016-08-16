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
//    if(isCompare) gr->AddLegend(QString("Оригинальня F0 записи").toLocal8Bit().data(),"-n1");
    gr->Legend(0,"-A");

    qDebug() << "waveData";
    gr->MultiPlot(1, 12, 0, 1, 1, "#");
    gr->SetRange('y', 0, GRAPH_Y_VAL_MAX);
    gr->Plot(waveData, "-B");

    qDebug() << "pitchData";
    gr->MultiPlot(1, 12, 4, 1, 6, "#");
    gr->SetRange('y', 0, GRAPH_Y_VAL_MAX);
    gr->Plot(pitchData, "-B3");
    if(!isCompare) gr->Plot(pitchDataOriginal, "-b1");
    gr->Axis("Y", "");
    gr->Grid("y", "W", "");

    if(isCompare){
        qDebug() << "secWaveData";
        gr->MultiPlot(1, 12, 1, 1, 1, "#");
        gr->SetRange('y', 0, GRAPH_Y_VAL_MAX);
        gr->Plot(secWaveData, "G");

        gr->MultiPlot(1, 12, 3, 1, 1, "#");
        gr->Puts(mglPoint(0,0),QString("Совпадение: \\big{#r{%1}}%").arg(this->result).toLocal8Bit().data(), ":C", 30);

        qDebug() << "secPitchData";
        gr->MultiPlot(1, 12, 4, 1, 6, "#");
        gr->SetRange('y', 0, GRAPH_Y_VAL_MAX);
        gr->Plot(secPitchData, "-G3");

//        qDebug() << "secPitchData";
//        gr->MultiPlot(1, 12, 4, 1, 6, "#");
//        gr->SetRange('y', 0, GRAPH_Y_VAL_MAX);
//        gr->Plot(secPitchDataOrig, "-n1");
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

        vectorToData(dataSec.d_wave, &secWaveData);
        secWaveData.Norm(GRAPH_Y_VAL_MAX);
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
//        VectorDP dp(new VectorSignal(copyv(pitchOrig)), new VectorSignal(copyv(pitch)));
        VectorDP dp(new VectorSignal(copyv(pitchOrigNorm)), new VectorSignal(copyv(pitchNorm)));
        vector newPitch = dp.getScaledSignal()->getArray();
//        this->result = dp.getSignalMask()->value.globalError;
//        this->result = calcResultMark(newPitch,pitchOrig, dp.getSignalMask()->value.globalError);
        this->result = calcResultMark(newPitch,pitchOrigNorm, dp.getSignalMask()->value.globalError);

        qDebug() << "Stop DP";

//        qDebug() << "Start DP";
//        SPTK_SETTINGS * sptk_settings = SettingsDialog::getSPTKsettings();
//        int speksize = sptk_settings->spec->leng / 2 + 1;
//        SpectrDP dp(new SpectrSignal(copyv(data->d_spec), speksize),
//                    new SpectrSignal(copyv(dataSec.d_spec), speksize));
//        VectorSignal data(makev(dataSec.d_spec.x/speksize));
//        for(int i=0; i<pitch.x; i++) data.setValueAt(pitch.v[i], i);
//        vector newPitch = ((VectorSignal*)dp.applyMask<double>(&data))->getArray();
//        this->result = dp.getSignalMask()->value.globalError;
//        qDebug() << "Stop DP";

        vectorToData(newPitch, &secPitchData);
        secPitchData.Norm(GRAPH_Y_VAL_MAX);
        qDebug() << "pitchData New Filled";

        freev(newPitch);
        freeGraphData(dataSec);
        qDebug() << "New Data Processed";
    }
}
