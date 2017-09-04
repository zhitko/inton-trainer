/*  Standard C Libraries  */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "func.h"
#include "float.h"

vector vector_intensive(vector data, FRAME_SETTINGS * settings)
{
    int frameLength = settings->leng,
        resultLength = data.x / frameLength;
    vector result = makev(resultLength);

    double middle = 0.0;
    for(int i=0;i<resultLength;i++)
    {
        middle = 0.0;
        for(int j=(i*frameLength);j<((i+1)*frameLength) && j<data.x;j++)
            middle += fabs(getv(data, j));
        middle /= frameLength;
        setv(result, i, middle);
    }

    return result;
}

vector vector_smooth_lin(vector data, int frame)
{
    int resultLength = data.x;
    vector result = makev(resultLength);

    double middle;
    for(int i=0; i<resultLength; i++)
    {
        middle = 0.0;
        for(int j=0; j < frame; j++)
        {
            middle += getv(data, j + i);
        }
        setv(result, i, middle/frame);
    }

    return result;

}

int compare (const void * a, const void * b)
{
  return ( *(double*)a - *(double*)b );
}

vector vector_smooth_mid(vector data, int frame)
{
    int resultLength = data.x;
    vector result = makev(resultLength);

    double* middle = malloc(frame * sizeof(double));
    for(int i=0;i<resultLength;i++)
    {
        for(int j=0; j < frame; j++)
        {
            middle[j] = fabs(getv(data, i+j));
        }
        qsort (middle, frame, sizeof(double), compare);
        setv(result, i, middle[frame/2]);
    }

    return result;
}

vector vector_fill_empty(vector data)
{
    int i;
    double v = getv(data, 0);
    for(i = 0; i<data.x; i++)
        if(getv(data, i) == 0)
            setv(data, i, v);
        else
            v = getv(data, i);
    return data;
}

vector vector_pow_log(vector data, int factor, double min)
{
    vector data_norm = normalizev(data, 0.0, 1.0);
    vector data_log = zerov(data_norm.x);
    for(int i=0;i<data.x;i++)
    {
        if (getv(data_norm, i) > 0.1)
            setv(data_log, i, pow(log(getv(data_norm, i))+1, factor));
        if (getv(data_log, i) < min)
            setv(data_log, i, 0.0);
    }
    freev(data_norm);
    return data_log;
}

vector vector_pow_exp(vector data, int factor, double min)
{
    vector data_norm = normalizev(data, 0.0, 1.0);
    vector data_log = makev(data_norm.x);
    for(int i=0;i<data.x;i++)
    {
        setv(data_log, i, pow(exp(getv(data_norm, i)-1), factor));
        if (getv(data_log, i) < min)
            setv(data_log, i, 0.0);
    }
    freev(data_norm);
    return data_log;
}

vector zero_to_nan(vector data)
{
    for(int i=0;i<data.x;i++)
        if (getv(data, i) == 0)
            setv(data, i, NAN);
    return data;
}

vector vector_cut_by_mask(vector data, vector mask)
{
    vector result = zerov(data.x);
    for (int i=0; i<data.x && i<mask.x; i++ )
    {
        if(getv(mask, i) > MASK_LIMIT)
        {
            setv(result, i, getv(data, i));
        }
    }
    return result;
}

vector vector_strip_by_mask(vector data, vector mask)
{
    int len = 0;
    for (int i=0; i<data.x && i<mask.x; i++ )
    {
        if(getv(mask, i) > MASK_LIMIT)
        {
            len++;
        }
    }

    vector result = zerov(len);
    int pos = 0;
    for (int i=0; i<data.x && i<mask.x && pos<len; i++ )
    {
        if(getv(mask, i) > MASK_LIMIT)
        {
            setv(result, pos, getv(data, i));
            pos++;
        }
    }
    return result;
}

vector vector_invert_mask(vector mask)
{
    vector inverted = makev(mask.x);
    for(int i=0; i<mask.x; i++ )
    {
        if(getv(mask, i) > MASK_LIMIT)
        {
            setv(inverted, i, 0.0);
        }
        else
        {
            setv(inverted, i, 1.0);
        }
    }
    return inverted;
}

vector make_mask(int length, int count, int * points_from, int * points_length)
{
    vector mask = zerov(length);

    for (int i=0; i<count; i++)
    {
        for (int j=points_from[i]; j<(points_from[i]+points_length[i]); j++)
        {
            setv(mask, j, 1);
        }
    }

    return mask;
}

vector vector_resize(vector orig, int new_size)
{
    int orig_size = orig.x;
    vector result = zerov(new_size);

    for (int i=0; i<new_size; i++)
    {
        int scaled_i = (i*orig_size)/new_size;
        setv(result, i, getv(orig, scaled_i));
    }

    return result;
}

