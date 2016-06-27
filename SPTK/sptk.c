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

FRAME_SETTINGS *initEnergyFrameSettings()
{
    FRAME_SETTINGS * frame = malloc(sizeof(FRAME_SETTINGS));
    frame->leng = 5;
    frame->shift = 1;
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
    return lpc;
}

SPEC_SETTINGS *initSpecSettings()
{
    SPEC_SETTINGS * spec = malloc(sizeof(SPEC_SETTINGS));
    spec->leng = 512;
    spec->order = 25;
    return spec;
}

SPTK_SETTINGS *initSptkSettings()
{
    SPTK_SETTINGS * settings = malloc(sizeof(SPTK_SETTINGS));

    settings->pitch = initPitchSettings();
    settings->frame = initFrameSettings();
    settings->energyFrame = initEnergyFrameSettings();
    settings->window = initWindowSettings();
    settings->lpc = initLpcSettings();
    settings->spec = initSpecSettings();
    return settings;
}
