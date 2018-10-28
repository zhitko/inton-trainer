#include <stdio.h>
#include <stdlib.h>

#include "SPTK.h"

PITCH_SETTINGS *initPitchSettings()
{
    PITCH_SETTINGS * pitch = malloc(sizeof(PITCH_SETTINGS));
    pitch->MAX_FREQ = 240;        // 240  Maximum fundamental frequency to search for (Hz)
    pitch->MIN_FREQ = 60;         // 60   Minimum fundamental frequency to search for (Hz)
    pitch->FRAME_SHIFT = 100;     // 80   Frame shift
    pitch->SAMPLE_FREQ = 8.0;     // 16.0 Samplingfrequency (kHz)
    pitch->ATYPE = 0;             // 0    Algorithm used for extraction of pitch. [0: RAPT, 1: SWIPE]
    pitch->OTYPE = 1;             // 0    Output format. [0:pitch, 1:F0, 2:log(F0)]
    pitch->THRESH_RAPT = 0.0;     // 0.0  Voiced/unvoiced threhold (used only for RAPT algorithm)
    pitch->THRESH_SWIPE = 0.3;    // 0.3  Voiced/unvoiced threhold (used only for SWIPE algorithm)
    return pitch;
}

FRAME_SETTINGS *initFrameSettings()
{
    FRAME_SETTINGS * frame = malloc(sizeof(FRAME_SETTINGS));
    frame->leng = 512;
    frame->shift = 256;
    return frame;
}

WINDOW_SETTINGS *initWindowSettings()
{
    WINDOW_SETTINGS * window = malloc(sizeof(WINDOW_SETTINGS));
    window->leng = 512;
    window->window_type = BLACKMAN;
    return window;
}

LPC_SETTINGS *initLpcSettings()
{
    LPC_SETTINGS * lpc = malloc(sizeof(LPC_SETTINGS));
    lpc->leng = 512;
    lpc->order = 25;
    lpc->cepstrum_order = 25;
    return lpc;
}

SPEC_SETTINGS *initSpecSettings()
{
    SPEC_SETTINGS * spec = malloc(sizeof(SPEC_SETTINGS));
    spec->leng = 512;
    spec->order = 25;
    spec->factor = 10;
    spec->min = 0.1;
    spec->proc = 0;
    return spec;
}

PLOT_SETTINGS *initPlotSettings()
{
    PLOT_SETTINGS * plot = malloc(sizeof(PLOT_SETTINGS));
    plot->frame = 10;
    plot->interpolation_type = 1;
    plot->normF0MinMax = 1;
    return plot;
}

DP_SETTINGS *initDpSettings()
{
    DP_SETTINGS * dp = malloc(sizeof(DP_SETTINGS));
    dp->continiusLimit = 100;
    dp->continiusKD = 0.5;
    dp->continiusKV = 1.0;
    dp->continiusKH = 0.1;
    dp->continiusKT = 20;
    dp->showA0 = 1;
    dp->showF0 = 1;
    dp->showOriginalF0 = 0;
    dp->showError = 0;
    dp->showTime = 0;
    dp->errorType = 0;
    dp->useForDP = 1;
    dp->dpA0Coeficient = 1.0;
    dp->dpF0Coeficient = 1.0;
    dp->dpDA0Coeficient = 1.0;
    dp->showPortr = 1;
    dp->portLen = 5;
    dp->umpSmoothType = 0;
    dp->umpSmoothValue = 5;
    dp->useStripUmp = 0;
    dp->recordingSeconds = 3;
    dp->recordingFrameSeconds = 0.2;
    dp->recordingFrameAfter = 1;
    dp->recordingFrameBefore = 5;
    dp->recordingMaxFiles = 10;
    dp->recordingType = 3;
    dp->mark_delimeter = 5.0;
    dp->mark_level = 50.0;
    dp->mark_labels = "F (Failure), D (Insufficient), C (Mediocre), B (Satisfactory), A (Excellent)";
    dp->smooth_frame = 1;
    dp->show_marks = 1;
    dp->markoutA0limit = 10;
    return dp;
}

SPTK_SETTINGS *initSptkSettings()
{
    SPTK_SETTINGS * settings = malloc(sizeof(SPTK_SETTINGS));

    settings->pitch = initPitchSettings();
    settings->frame = initFrameSettings();
    settings->window = initWindowSettings();
    settings->lpc = initLpcSettings();
    settings->spec = initSpecSettings();
    settings->plotEnergy = initPlotSettings();
    settings->plotF0 = initPlotSettings();
    settings->dp = initDpSettings();
    return settings;
}
