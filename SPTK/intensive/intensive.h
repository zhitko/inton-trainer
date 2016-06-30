#include "vector.h"
#include "SPTK.h"

#ifndef SPTK_INTENSIVE
#define SPTK_INTENSIVE

vector sptk_intensive(vector data, FRAME_SETTINGS * settings);
vector sptk_mid_intensive(vector data, ENERGY_SETTINGS * settings);
vector sptk_fill_empty(vector data);
vector sptk_log(vector data);

#endif
