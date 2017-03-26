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
            middle += fabs(data.v[j]);
        middle /= frameLength;
        result.v[i] = middle;
    }

    return result;
}

int compare (const void * a, const void * b)
{
  return ( *(double*)a - *(double*)b );
}

vector vector_avg_intensive(vector data, ENERGY_SETTINGS * settings)
{
    int frameLength = settings->leng,
        resultLength = data.x;
    vector result = makev(resultLength);

    double* middle = malloc(frameLength * sizeof(double));
    for(int i=0;i<resultLength;i++)
    {
        for(int j=0; j < frameLength; j++)
        {
            int position = i+j-frameLength/2;
            if( position < 0 || position > resultLength )
                middle[j] = 0.0;
            else
                middle[j] = fabs(data.v[position]);
        }
        qsort (middle, frameLength, sizeof(double), compare);
        result.v[i] = middle[frameLength/2];
    }

    return result;
}

vector vector_mid(vector data, int frame)
{
    int resultLength = data.x;
    vector result = makev(resultLength);

    double* middle = malloc(frame * sizeof(double));
    for(int i=0;i<resultLength;i++)
    {
        for(int j=0; j < frame; j++)
        {
            int position = i+j-frame/2;
            if( position < 0 || position > resultLength )
                middle[j] = 0.0;
            else
                middle[j] = fabs(data.v[position]);
        }
        qsort (middle, frame, sizeof(double), compare);
        result.v[i] = middle[frame/2];
    }

    return result;
}

vector vector_fill_empty(vector data)
{
    int i;
    double v = data.v[0];
    for(i = 0; i<data.x; i++)
        if(data.v[i] == 0) data.v[i] = v;
        else v = data.v[i];
    return data;
}

vector vector_pow_log(vector data, int factor, double min)
{
    vector data_norm = normalizev(data, 0.0, 1.0);
    vector data_log = zerov(data_norm.x);
    for(int i=0;i<data.x;i++)
    {
        if (data_norm.v[i] > 0.1) data_log.v[i] = pow(log(data_norm.v[i])+1, factor);
        if (data_log.v[i] < min) data_log.v[i] = 0.0;
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
        data_log.v[i] = pow(exp(data_norm.v[i]-1), factor);
        if (data_log.v[i] < min) data_log.v[i] = 0.0;
    }
    freev(data_norm);
    return data_log;
}

vector zero_to_nan(vector data)
{
    for(int i=0;i<data.x;i++)
        if (data.v[i] == 0)
            data.v[i] = NAN;
    return data;
}

vector vector_cut_by_mask(vector data, vector mask)
{
    vector result = zerov(data.x);
    for (int i=0; i<data.x && i<mask.x; i++ )
    {
        if(mask.v[i] > MASK_LIMIT)
        {
            result.v[i] = data.v[i];
        }
    }
    return result;
}

vector vector_strip_by_mask(vector data, vector mask)
{
    int len = 0;
    for (int i=0; i<data.x && i<mask.x; i++ )
    {
        if(mask.v[i] > MASK_LIMIT)
        {
            len++;
        }
    }

    vector result = zerov(len);
    int pos = 0;
    for (int i=0; i<data.x && i<mask.x && pos<len; i++ )
    {
        if(mask.v[i] > MASK_LIMIT)
        {
            result.v[pos] = data.v[i];
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
        if(mask.v[i] > MASK_LIMIT)
        {
            inverted.v[i] = 0.0;
        }
        else
        {
            inverted.v[i] = 1.0;
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
            mask.v[j] = 1;
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
        result.v[i] = orig.v[scaled_i];
    }

    return result;
}

