#include "vector.h"
#include "defines.h"

#ifndef SPTK_FUNC
#define SPTK_FUNC

vector vector_intensive(vector, int, int);
vector vector_smooth_mid(vector, int);
vector vector_smooth_lin(vector, int);
vector vector_fill_empty(vector);
vector vector_pow_exp(vector, int, double);
vector vector_pow_log(vector, int, double);
vector vector_cut_by_mask(vector, vector);
vector vector_strip_by_mask(vector, vector);
vector vector_invert_mask(vector);
vector vector_mask_and(vector, vector);
vector make_mask(int, int, int*, int*);
vector vector_resize(vector,int);
vector zero_to_nan(vector);
vector vector_mid(vector, int, int);
vector vector_derivative(vector);
vector vector_normalize_optional_zeros(vector, double, double, int);

#endif
