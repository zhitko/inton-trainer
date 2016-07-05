#include "vector.h"
#include "SPTK.h"

#ifndef SPTK_INTENSIVE
#define SPTK_INTENSIVE

vector vector_intensive(vector, FRAME_SETTINGS*);
vector vector_mid_intensive(vector, ENERGY_SETTINGS*);
vector vector_mid(vector, int);
vector vector_fill_empty(vector);
vector vector_log(vector);
void vector_cut_by_mask(vector, vector);

#endif
