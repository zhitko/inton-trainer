#ifndef DERIVATIVE_H
#define DERIVATIVE_H

#include "./sptk/vector.h"

typedef struct {
    vector data;
    double zero;
} derivative;

derivative get_derivative_data(vector, int);

#endif // DERIVATIVE_H
