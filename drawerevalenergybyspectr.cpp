#include "drawerevalenergybyspectr.h"

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

DrawerEvalEnergyBySpectr::DrawerEvalEnergyBySpectr() :
    DrawerEvalEnergy()
{
}

DrawerEvalEnergyBySpectr::~DrawerEvalEnergyBySpectr()
{
    qDebug() << "DrawerEvalEnergy removed";
}

void DrawerEvalEnergyBySpectr::Proc(QString fname)
{
    if(first)
    {
        qDebug() << "Drawer::Proc";
        DrawerEvalEnergy::Proc(fname);
        first = false;
    }
    else
    {
        qDebug() << "DrawerEval::Proc";
        this->secFileName = fname;

        GraphData dataSec = ProcWave2Data(this->secFileName);

        vectorToData(dataSec.d_full_wave, &secWaveData);
        qDebug() << "waveData New Filled";

        vectorToData(dataSec.d_intensive, &secIntensiveDataOrig);
        secIntensiveDataOrig.Norm(GRAPH_Y_VAL_MAX);
        qDebug() << "secIntensiveDataOrig New Filled";

        vector intensiveOrig;
        intensiveOrig.v = intensiveData.a;
        intensiveOrig.x = data->d_intensive.x;
        qDebug() << "intensiveOrig " << intensiveOrig.x;
        (*intensiveOrig.v) = 0;

        vector intensive;
        intensive.v = secIntensiveDataOrig.a;
        intensive.x = dataSec.d_intensive.x;
        qDebug() << "intensive " << intensive.x;
        (*intensive.v) = 0;

        qDebug() << "Start DP";
        VectorDP dp(new VectorSignal(copyv(intensiveOrig)), new VectorSignal(copyv(intensive)));
        qDebug() << "Start DP";
        vector newIntensive = dp.getScaledSignal()->getArray();
        this->result = calcResultMark(newIntensive,intensiveOrig, dp.getSignalMask()->value.globalError);
        qDebug() << "Stop DP";

        vectorToData(newIntensive, &secIntensiveData);
        qDebug() << "secIntensiveDataOrig New Filled";

        freev(newIntensive);
        freeGraphData(dataSec);
        qDebug() << "New Data Processed";
    }
}
