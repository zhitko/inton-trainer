#ifndef UMP_H
#define UMP_H

#include "analysis/mask_data.h"

extern "C" {
    #include "./sptk/vector.h"
}

typedef struct { int from; int len; int type; } MaskDetails;

/**
 * @brief Create UMP
 * @param data
 * @param mask
 * @param mask_p
 * @param mask_n
 * @param mask_t
 * @param mask_scale Number Scale result mask
 * @param part_len Number Segmants length
 * @param useStripUmp Boolean Remove data between segmants
 * @param keepRatio Boolean Keep segments ratio
 * @return
 */
vector makeUmp(
        MaskData *ump_mask,
        vector data,
        vector mask,
        MaskData mask_p,
        MaskData mask_n,
        MaskData mask_t,
        double mask_scale,
        int part_len,
        int useStripUmp,
        int keepRatio
);

#endif // UMP_H
