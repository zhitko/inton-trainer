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

        qDebug() << "Start MOVE";
        SPTK_SETTINGS * sptk_settings = SettingsDialog::getSPTKsettings();
        vector scalledPitch;

        if (sptk_settings->move->type == 1)
        {
            int speksize = sptk_settings->spec->leng / 2 + 1;
            SpectrDP dp(new SpectrSignal(copyv(data->d_spec_proc), speksize),
                        new SpectrSignal(copyv(dataSec.d_spec_proc), speksize),
                        sptk_settings->dp->globalLimit,
                        sptk_settings->dp->localLimit
                        );

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

            scalledPitch = scaleVectorByDPResults(pitch, &dp);
        } else if (sptk_settings->move->type == 0) {
            qDebug() << "moom " << dataSec.d_full_wave.x << " " << data->n_mask.x << " " << data->pnt_mask.x << " " << data->t_mask.x;

            vector scalled_n_mask = vector_resize(data->n_mask, dataSec.d_full_wave.x);
            vector n_wave = vector_cut_by_mask(dataSec.d_full_wave, scalled_n_mask);
            vectorToData(n_wave, &nSecWaveData);
            freev(n_wave);
            freev(scalled_n_mask);

            vector scalled_p_mask = vector_resize(data->p_mask, dataSec.d_full_wave.x);
            vector p_wave = vector_cut_by_mask(dataSec.d_full_wave, scalled_p_mask);
            vectorToData(p_wave, &pSecWaveData);
            freev(p_wave);
            freev(scalled_p_mask);

            vector scalled_t_mask = vector_resize(data->t_mask, dataSec.d_full_wave.x);
            vector t_wave = vector_cut_by_mask(dataSec.d_full_wave, scalled_t_mask);
            vectorToData(t_wave, &tSecWaveData);
            freev(t_wave);
            freev(scalled_t_mask);

            vector scalled_pnt_mask = vector_resize(data->pnt_mask, dataSec.d_full_wave.x);
            vector pnt_wave = vector_cut_by_mask(dataSec.d_full_wave, scalled_pnt_mask);
            vectorToData(pnt_wave, &secWaveData);
            freev(pnt_wave);
            freev(scalled_pnt_mask);

            scalledPitch = vector_resize(pitch, pitchOrig.x);
        }

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

        this->result = calcResultMark(newPitch,pitchOrig);
        qDebug() << "Stop DP";

        vectorToData(newPitch, &secPitchData);
        secPitchData.Norm(GRAPH_Y_VAL_MAX);
        qDebug() << "pitchData New Filled";

        freev(newPitch);
        freeGraphData(dataSec);
        qDebug() << "New Data Processed";
    }
}
