#include "../vector.h"
#include "../SPTK.h"

#ifndef SPTK_PITCH
#define SPTK_PITCH

vector sptk_pitch(vector data, PITCH_SETTINGS * settings);
vector sptk_pitch_spec(vector data, PITCH_SETTINGS * settings, int count);

#endif
