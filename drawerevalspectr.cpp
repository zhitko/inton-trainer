#include "drawerevalspectr.h"

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
}

DrawerEvalSpectr::DrawerEvalSpectr() :
    Drawer(),
    secFileName(""),
    result(0),
    first(true)
{
}

DrawerEvalSpectr::~DrawerEvalSpectr()
{
    qDebug() << "DrawerEval removed";
}

int DrawerEvalSpectr::Draw(mglGraph *gr)
{
    qDebug() << "start drawing";

    gr->DefaultPlotParam();
    gr->Clf();

    qDebug() << "waveData";
    gr->MultiPlot(1, 23, 0, 1, 1, "#");
    gr->SetRange('y', 0, GRAPH_Y_VAL_MAX);
    gr->Plot(waveData, "-G");

    specData.Norm(0, 1);
    gr->MultiPlot(1, 23, 4, 1, 6, "#");
    if(stereo) gr->Rotate(50,60);
    QString colors = QString("w{w,%1}k").arg(QString::number(0));
    gr->SetDefScheme(colors.toStdString().c_str());
    gr->Surf(specData);

    if(!this->secFileName.isEmpty()){
        qDebug() << "secWaveData";
        gr->MultiPlot(1, 23, 1, 1, 1, "#");
        gr->SetRange('y', 0, GRAPH_Y_VAL_MAX);
        gr->Plot(secWaveData, "B");

        gr->MultiPlot(1, 23, 3, 1, 1, "#");
        gr->Puts(mglPoint(0,0),QString("Your score: \\big{#r{%1}}").arg(this->result).toUtf8().data(), ":C", 50);

        secSpecDataOrig.Norm(0, 1);
        gr->MultiPlot(1, 23, 11, 1, 6, "#");
        if(stereo) gr->Rotate(50,60);
        gr->SetDefScheme(colors.toStdString().c_str());
        gr->Surf(secSpecDataOrig);

        secSpecData.Norm(0, 1);
        gr->MultiPlot(1, 23, 17, 1, 6, "#");
        if(stereo) gr->Rotate(50,60);
        gr->SetDefScheme(colors.toStdString().c_str());
        gr->Surf(secSpecData);
    }

    qDebug() << "finish drawing";
    return 0;
}

void DrawerEvalSpectr::Proc(QString fname)
{
    if(first)
    {
        qDebug() << "Drawer::Proc";
        Drawer::Proc(fname);
        first = false;
    }
    else
    {
        MathGLSettings * mathgl_settings = SettingsDialog::getMathGLSettings();
        SPTK_SETTINGS * sptk_settings = SettingsDialog::getSPTKsettings();

        qDebug() << "DrawerEval::Proc";
        this->secFileName = fname;

        GraphData dataSec = ProcWave2Data(this->secFileName);

        vectorToData(dataSec.d_wave, &secWaveData);
        secWaveData.Norm(GRAPH_Y_VAL_MAX);
        qDebug() << "waveData New Filled";

        int speksize = sptk_settings->spec->leng / 2 + 1;
        int specX = dataSec.d_spec.x/speksize;
        int specY = speksize;
        secSpecDataOrig.Create(specX, specY);
        for(long j=0;j<specY;j++)
            for(long i=0;i<specX;i++)
            {
                long i0 = i+specX*j;
                long i1 = j+specY*i;
                secSpecDataOrig.a[i0] = dataSec.d_spec.v[i1];
            }
        secSpecDataOrig.Squeeze(mathgl_settings->quality, 1);

        qDebug() << "Start DP";
        SpectrDP dp(new SpectrSignal(copyv(data->d_spec), speksize),
                    new SpectrSignal(copyv(dataSec.d_spec), speksize));
        VectorSignal data(makev(dataSec.d_spec.x/speksize));
        vector newSpec = dp.getScaledSignal()->getArray();
        this->result = dp.getSignalMask()->value.globalError;
        qDebug() << "Stop DP";

        specX = newSpec.x/speksize;
        specY = speksize;
        secSpecData.Create(specX, specY);
        for(long j=0;j<specY;j++)
            for(long i=0;i<specX;i++)
            {
                long i0 = i+specX*j;
                long i1 = j+specY*i;
                secSpecData.a[i0] = newSpec.v[i1];
            }
        secSpecData.Squeeze(mathgl_settings->quality, 1);

        freeGraphData(dataSec);
        qDebug() << "New Data Processed";
    }
}
