/*  Standard C Libraries  */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef HAVE_STRING_H
#  include <string.h>
#else
#  include <strings.h>
#  ifndef HAVE_STRRCHR
#     define strrchr rindex
#  endif
#endif

#include "SPTK.h"
#include "intensive.h"
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
        for(int j=(i*frameLength);j<((i+1)*frameLength);j++)
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

vector vector_mid_intensive(vector data, ENERGY_SETTINGS * settings)
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

vector vector_log(vector data)
{
    vector result = makev(data.x);
    for(int i=0;i<data.x;i++)
        result.v[i] = log10(data.v[i])*2000;
    return result;
}


void vector_cut_by_mask(vector data, vector mask)
{
    double t = 0.9;

    double min_value = DBL_MAX;
    double max_value = -DBL_MAX;
    for(int i=0; i<data.x && i<mask.x; i++ )
    {
        if(mask.v[i] + t > 1)
        {
            if(data.v[i] > max_value) max_value = data.v[i];
            if(data.v[i] < min_value) min_value = data.v[i];
        }
    }
    for(int i=0; i<data.x; i++ )
    {
        if(data.v[i] > max_value) data.v[i] = max_value;
        if(data.v[i] < min_value) data.v[i] = min_value;
    }
}

