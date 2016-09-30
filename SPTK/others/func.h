#include "vector.h"
#include "SPTK.h"

#ifndef SPTK_FUNC
#define SPTK_FUNC

#define MASK_LIMIT 0.9

vector vector_intensive(vector, FRAME_SETTINGS*);
vector vector_avg_intensive(vector, ENERGY_SETTINGS*);
vector vector_mid(vector, int);
vector vector_fill_empty(vector);
vector vector_log(vector);
vector vector_cut_by_mask(vector, vector);
vector vector_invert_mask(vector);
vector make_mask(int, int, int*, int*);
vector vector_resize(vector,int);
vector zero_to_nan(vector);

#endif
