#include "derivative.h"

#include "math.h"
#include "./sptk/others/func.h"

derivative get_derivative_data(vector data, int smooth)
{
    derivative result;

    vector der = derivativev(data);
    result.data = vector_smooth_lin(der, smooth);
    freev(der);

    double max = fabs(getv(result.data, maxv(result.data)));
    double min = fabs(getv(result.data, minv(result.data)));
    result.zero = min/(max+min);

    return result;
}
