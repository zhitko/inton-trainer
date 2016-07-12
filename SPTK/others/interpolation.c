/*  Standard C Libraries  */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "interpolation.h"
#include "float.h"

#include <gsl/gsl_errno.h>
#include <gsl/gsl_spline.h>

typedef struct {
    int start_index;
    int end_index;
    int n;
    double* x;
    double* y;
} segment;

typedef struct {
    int n;
    segment * seg;
} segments;

void free_segments(segments segs)
{
    for(int i=0; i<segs.n; i++)
    {
        free(segs.seg[i].x);
        free(segs.seg[i].y);
    }
//    free(segs.seg);
}

/***
 * What is segment:
 * mask                    | 00000000000000000111111111111100000000000000000 |
 *                         |       signal    |  no signal  |   signal        |
 * signal data             | ~~|~~~~~~~|~~~~~|_____________|~~~~~|~~~~~~~|~~ |
 * params                  |   | edges | cut |             | cut | edges |   |
 * stored for intrpolation |   |-------|                         |-------|   |
 * will interpolate        |           |-------------------------|           |
 * start_index             |                 X                               |
 * end_index               |                               X                 |
 * start_data              |   X                                             |
 * start_data_to           |           X                                     |
 * end_data                |                                     X           |
 ***/

segments add_new_segment(segments segs, int start_index, int end_index, int edges, int cut, vector data)
{
    int start_data = start_index - edges - cut;
    if(start_data < 0) start_data = 0;

    int start_data_to = start_index - cut;
    if(start_data_to < 0) start_data_to = 0;

    int end_data = end_index + cut;
    if(end_data > data.x) end_data = data.x - 1;

    int new_size = segs.n + 1;
    segs.seg = realloc(segs.seg, sizeof(segment)*new_size);

    int data_size = edges * 2;

    segs.seg[segs.n].start_index = start_data_to;
    segs.seg[segs.n].end_index = end_data;
    segs.seg[segs.n].n = data_size;
    segs.seg[segs.n].x = calloc(data_size, sizeof(double));
    segs.seg[segs.n].y = calloc(data_size, sizeof(double));

    for(int i=0; i<edges; i++)
    {
        int x = i + start_data;
        segs.seg[segs.n].x[i] = x;
        segs.seg[segs.n].y[i] = data.v[x];

        x = edges + i + end_data;
        segs.seg[segs.n].x[i+edges] = x;
        segs.seg[segs.n].y[i+edges] = data.v[x];
    }

    segs.n = new_size;
    return segs;
}

segments vector_split_by_mask(vector data, vector mask, int edges, int cut)
{
    segments segs;
    segs.n = 0;

    int start_index = -1;
    int end_index = -1;

    for(int i=0; i<data.x; i++)
    {
        if(start_index == -1 && mask.v[i] == 1.0)
        {
            start_index = i;
            continue;
        }
        if(start_index != -1 && mask.v[i] != 1.0)
        {
            end_index = i;
            segs = add_new_segment(segs, start_index, end_index, edges, cut, data);

            start_index = -1;
            end_index = -1;
            continue;
        }
    }

//    if(start_index != -1)
//    {
//        end_index = data.x - 1;
//        segs = add_new_segment(segs, start_index, end_index, edges, 0, data);
//    }

    return segs;
}

vector vector_interpolate(vector data, vector mask, int cut, const gsl_interp_type * T)
{
    int edges = gsl_interp_type_min_size(T);
    segments segs = vector_split_by_mask(data, mask, edges, cut);

    vector result = copyv(data);

    for(int i=0; i<segs.n; i++)
    {
        gsl_interp_accel *acc = gsl_interp_accel_alloc ();
        gsl_spline *liner = gsl_spline_alloc(T, segs.seg[i].n);
        gsl_spline_init (liner, segs.seg[i].x, segs.seg[i].y, segs.seg[i].n);

        for(int j=segs.seg[i].start_index; j<segs.seg[i].end_index; j++)
        {
            result.v[j] = gsl_spline_eval (liner, j, acc);
        }

        gsl_spline_free (liner);
        gsl_interp_accel_free (acc);
    }

    free_segments(segs);

    return result;
}

vector vector_interpolate_by_mask(vector data, vector mask, int cut, int type)
{
    vector result;
    switch (type) {
    case 1: // Linear interpolation
        result = vector_interpolate(data, mask, cut, gsl_interp_linear);
        break;
    case 2: // Cubic spline (тatural boundary conditions)
        result = vector_interpolate(data, mask, cut, gsl_interp_cspline);
        break;
    case 3: // Cubic spline (periodic boundary conditions)
        result = vector_interpolate(data, mask, cut, gsl_interp_cspline_periodic);
        break;
    case 4: // Non-rounded Akima spline (natural boundary conditions)
        result = vector_interpolate(data, mask, cut, gsl_interp_akima);
        break;
    case 5: // Non-rounded Akima spline (periodic boundary conditions)
        result = vector_interpolate(data, mask, cut, gsl_interp_akima_periodic);
        break;
    case 6: // Steffen’s method
        result = vector_interpolate(data, mask, cut, gsl_interp_steffen);
        break;
    default:
        result = copyv(data);
        break;
    }
    return result;
}
