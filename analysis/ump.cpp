#include "ump.h"

#include <QDebug>
#include "defines.h"

extern "C" {
    #include "./sptk/others/func.h"
}


int compare_mask_details (const void * a, const void * b)
{
  return ( ((MaskDetails*)a)->from - ((MaskDetails*)b)->from );
}

vector makeUmp(
        vector * data,
        vector * mask,
        MaskData mask_p,
        MaskData mask_n,
        MaskData mask_t,
        double mask_scale,
        int part_len,
        int useStripUmp,
        int keepRatio
) {
    const int TYPE_P = 1;
    const int TYPE_N = 2;
    const int TYPE_T = 3;
    int len_p = mask_p.pointsFrom.x;
    int len_n = mask_n.pointsFrom.x;
    int len_t = mask_t.pointsFrom.x;
    int len = len_p + len_n + len_t;
    MaskDetails * details = new MaskDetails[len];

    for(int i=0; i<len_p; i++)
    {
        details[i] = MaskDetails{
                getiv(mask_p.pointsFrom, i)/mask_scale,
                getiv(mask_p.pointsLength, i)/mask_scale,
                TYPE_P
        };
        qDebug() << "details P from " << details[i].from << LOG_DATA;
    }

    for(int i=0; i<len_n; i++)
    {
        details[len_p + i] = MaskDetails{
                getiv(mask_n.pointsFrom, i)/mask_scale,
                getiv(mask_n.pointsLength, i)/mask_scale,
                TYPE_N
        };
        qDebug() << "details N from " << details[len_p + i].from << LOG_DATA;
    }

    for(int i=0; i<len_t; i++)
    {
        details[len_p + len_n + i] = MaskDetails{
                getiv(mask_t.pointsFrom, i)/mask_scale,
                getiv(mask_t.pointsLength, i)/mask_scale,
                TYPE_T
        };
        qDebug() << "details T from " << details[len_p + len_n + i].from << LOG_DATA;
    }

    qsort(details, len, sizeof(MaskDetails), compare_mask_details);

    int merge = 0;
    for(int i=0; i<(len-1); i++)
        if(details[i].type == details[i+1].type && (details[i].type == TYPE_P || details[i].type == TYPE_T))
            merge ++;

    int merge_len = len - merge;
    MaskDetails * merged_details = new MaskDetails[merge_len];

    merged_details[0] = details[0];
    int ii = 0;

    for(int i=1; i<len; i++)
    {
        if(details[i-1].type == details[i].type && (details[i].type == TYPE_P || details[i].type == TYPE_T))
        {
            if (useStripUmp)
                merged_details[ii].len += details[i].len;
            else
                merged_details[ii].len = details[i].from + details[i].len - merged_details[ii].from;
        } else {
            ii++;
            merged_details[ii] = details[i];
        }
    }

    delete details;

    if(!useStripUmp)
    {
        for(int i=1; i<merge_len; i++)
        {
            int t1 = merged_details[i-1].type;
            int t2 = merged_details[i].type;

            if((t1 == TYPE_P || t1 == TYPE_T) && t2 == TYPE_N)
            {
                merged_details[i-1].len = merged_details[i].from - merged_details[i-1].from;
            } else if(t1 == TYPE_N && (t2 == TYPE_P || t2 == TYPE_T))
            {
                int d = merged_details[i].from - (merged_details[i-1].from + merged_details[i-1].len);
                merged_details[i].from -= d;
                merged_details[i].len += d;
            } else if(t1 == TYPE_N && t2 == TYPE_N)
            {
                int d = (merged_details[i].from - (merged_details[i-1].from + merged_details[i-1].len))/2;
                merged_details[i-1].len += d;
                merged_details[i].from -= d;
                merged_details[i].len += d;
            }
        }
    }

    int clone = 0;
    for(int i=1; i<(merge_len-1); i++)
    {
        int t1 = merged_details[i-1].type;
        int t2 = merged_details[i].type;
        int t3 = merged_details[i+1].type;
        if(t1 == TYPE_N && t3 == TYPE_N && (t2 == TYPE_P || t2 == TYPE_T))
            clone++;
    }

    int clone_len = merge_len + clone;

    vector ump_mask = zerov(clone_len*MASK_LEN);

    if (clone_len < 1) return ump_mask;

    MaskDetails * clone_details = new MaskDetails[clone_len];

    ii = 0;
    clone_details[0] = merged_details[0];

    if(merged_details[0].type == TYPE_N)
    {
        setv(ump_mask, 0, MASK_MIN);
        for(int i=1; i<MASK_LEN; i++)
        {
            setv(ump_mask, i,MASK_MAX);
        }
        setv(ump_mask, MASK_LEN, MASK_MIN);
    }

    for(int i=1; i<(merge_len-1); i++)
    {
        int t1 = merged_details[i-1].type;
        int t2 = merged_details[i].type;
        int t3 = merged_details[i+1].type;
        if(t1 == TYPE_N && t3 == TYPE_N && (t2 == TYPE_P || t2 == TYPE_T))
        {
            ii++;
            clone_details[ii] = merged_details[i];
            clone_details[ii].len = merged_details[i].len / 2;
            ii++;
            clone_details[ii] = merged_details[i];
            clone_details[ii].from = clone_details[ii-1].from + clone_details[ii-1].len;
            clone_details[ii].len = clone_details[ii-1].len;
        } else {
            ii++;
            clone_details[ii] = merged_details[i];

            if(clone_details[ii].type == TYPE_N)
            {
                setv(ump_mask, ii*MASK_LEN, MASK_MIN);
                for(int i=1; i<MASK_LEN; i++)
                {
                    setv(ump_mask, ii*MASK_LEN+i, MASK_MAX);
                }
                setv(ump_mask, (ii+1)*MASK_LEN, MASK_MIN);
            }
        }
    }

    clone_details[clone_len-1] = merged_details[merge_len-1];
    if(clone_details[clone_len-1].type == TYPE_N)
    {
        ii++;
        setv(ump_mask, ii*MASK_LEN, MASK_MIN);
        for(int i=1; i<MASK_LEN; i++)
        {
            setv(ump_mask, ii*MASK_LEN+i, MASK_MAX);
        }
        setv(ump_mask, (ii+1)*MASK_LEN, MASK_MIN);
    }

    delete merged_details;

    qDebug() << "data " << data->x << LOG_DATA;
    qDebug() << "mask " << mask->x << LOG_DATA;
    vector strip_data;
    strip_data = copyv(*data);
    qDebug() << "strip_data " << strip_data.x << LOG_DATA;

    qDebug() << "clone_len " << clone_len << LOG_DATA;
    qDebug() << "part_len " << part_len << LOG_DATA;

    vector result = zerov(part_len*clone_len);

    double scale = 1.0 * mask->x / data->x;

    qDebug() << "scale " << scale << LOG_DATA;

    ii = 0;
    int p = 0;
    int result_index = 0;
    int resize_to = part_len;
    int clone_total_len = 0;
    for(int i=0; i<clone_len; i++) clone_total_len += clone_details[i].len/scale;

    for(int i=0; i<clone_len; i++)
    {
        int len = clone_details[i].len/scale;
        vector in = zerov(len);
        for(int j=0; j<len; j++)
        {
            p = clone_details[i].from/scale + j;
            setv(in, j, getv(strip_data, p));
        }
        qDebug() << "len " << in.x << " - " << clone_details[i].from << LOG_DATA;

        if (keepRatio)
        {
            resize_to = result.x * (1.0 * len / clone_total_len);
        }

        vector out = vector_resize(in, resize_to);
        for (int j=0; j<out.x; j++)
        {
            setv(result, result_index, getv(out, j));
            result_index++;
        }
        freev(in);
        freev(out);

        ii += len;
    }

    freev(strip_data);
    qDebug() << "freev strip_data " << LOG_DATA;

    freev(*data);
    qDebug() << "freev data " << LOG_DATA;
    data->v = result.v;
    data->x = result.x;
    qDebug() << "data set " << LOG_DATA;

    delete clone_details;
    qDebug() << "delete clone_details " << LOG_DATA;

    return ump_mask;
}
