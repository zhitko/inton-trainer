#include "vector.h"

#ifndef SPTK_INTERPOLATION
#define SPTK_INTERPOLATION

vector vector_interpolate_by_mask(vector, vector, int, int);
void vector_interpolate_part(vector*, int, int, int);

#endif
