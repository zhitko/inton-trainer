#include <stdio.h>
#include <stdlib.h>

#include "SPTK.h"

PITCH_SETTINGS *initPitchSettings()
{
    PITCH_SETTINGS * pitch = malloc(sizeof(PITCH_SETTINGS));
    pitch->max_freq = 240;
    pitch->min_freq = 60;
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
    settings->window = initWindowSettings();
    settings->lpc = initLpcSettings();
    settings->spec = initSpecSettings();
    return settings;
}
