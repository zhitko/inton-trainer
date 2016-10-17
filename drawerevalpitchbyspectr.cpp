#include "drawerevalpitchbyspectr.h"

#include <QFile>
#include <QDebug>

#include "settingsdialog.h"

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

DrawerEvalPitchBySpectr::DrawerEvalPitchBySpectr() :
    DrawerEvalPitch()
{
}

DrawerEvalPitchBySpectr::~DrawerEvalPitchBySpectr()
{
    qDebug() << "DrawerEval removed";
}

void DrawerEvalPitchBySpectr::Proc(QString fname)
{    
    if(first)
    {
        qDebug() << "Drawer::Proc";
        DrawerEvalPitch::Proc(fname);
        first = false;
    }
    else
    {
        qDebug() << "DrawerEval::Proc";
        this->secFileName = fname;

        GraphData dataSec = ProcWave2Data(this->secFileName);
        dataSec.d_pitch = vector_fill_empty(dataSec.d_pitch);

//        vectorToData(dataSec.d_full_wave, &secWaveData);
        qDebug() << "waveData New Filled";

        vectorToData(dataSec.d_pitch, &secPitchDataOrig);
        secPitchDataOrig.Norm(GRAPH_Y_VAL_MAX);
        qDebug() << "pitchData New Filled";

        vector pitchOrig = initv(data->d_pitch.x, pitchData.a);
        (*pitchOrig.v) = 0;

        vector pitch = initv(dataSec.d_pitch.x, secPitchDataOrig.a);
        (*pitch.v) = 0;

        qDebug() << "Pitch sizes " << pitchOrig.x << " " << pitch.x;

        qDebug() << "Start DP";
        SPTK_SETTINGS * sptk_settings = SettingsDialog::getSPTKsettings();
        int speksize = sptk_settings->spec->leng / 2 + 1;
        qDebug() << "Start DP " << (sptk_settings->spec->leng / 2 + 1);
        qDebug() << "Start DP " << data->d_spec_proc.x;
        qDebug() << "Start DP " << dataSec.d_spec_proc.x;
        SpectrDP dp(new SpectrSignal(copyv(data->d_spec_proc), speksize),
                    new SpectrSignal(copyv(dataSec.d_spec_proc), speksize));

        // Wave processing

        vector n_wave = getSignalWithMask(data->n_mask, &dp, dataSec.d_full_wave);
        vectorToData(n_wave, &nSecWaveData);
        freev(n_wave);

        vector p_wave = getSignalWithMask(data->p_mask, &dp, dataSec.d_full_wave);
        vectorToData(p_wave, &pSecWaveData);
        freev(p_wave);

        vector t_wave = getSignalWithMask(data->t_mask, &dp, dataSec.d_full_wave);
        vectorToData(t_wave, &tSecWaveData);
        freev(t_wave);

        vector pnt_wave = getSignalWithMask(data->pnt_mask, &dp, dataSec.d_full_wave);
        vectorToData(pnt_wave, &secWaveData);
        freev(pnt_wave);

        // Pitch processing

        vector scalledPitch = scaleVectorByDPResults(pitch, &dp);

        vector pntMask = vector_invert_mask(data->pnt_mask);
        vector pntMaskScalled = vector_resize(pntMask, scalledPitch.x);
        vector pntMaskScalledInverted = vector_invert_mask(pntMaskScalled);
        vector cuttedPitch = vector_cut_by_mask(scalledPitch, pntMaskScalled);
        vector newPitch = vector_interpolate_by_mask(
                    cuttedPitch,
                    pntMaskScalledInverted,
                    sptk_settings->plotF0->interpolation_edges,
                    sptk_settings->plotF0->interpolation_type
                    );
        qDebug() << "pitch_interpolate";
        freev(scalledPitch);
        freev(pntMask);
        freev(pntMaskScalled);
        freev(pntMaskScalledInverted);
        freev(cuttedPitch);

        this->result = calcResultMark(newPitch,pitchOrig, dp.getSignalMask()->value.globalError);
        qDebug() << "Stop DP";

        vectorToData(newPitch, &secPitchData);
        secPitchData.Norm(GRAPH_Y_VAL_MAX);
        qDebug() << "pitchData New Filled";

        freev(newPitch);
        freeGraphData(dataSec);
        qDebug() << "New Data Processed";
    }
}
