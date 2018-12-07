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
 * @param mask_scale
 * @param part_len
 * @param useStripUmp
 * @return
 */
vector makeUmp(
        vector * data,
        vector * mask,
        MaskData mask_p,
        MaskData mask_n,
        MaskData mask_t,
        double mask_scale,
        int part_len,
        int useStripUmp
);

#endif // UMP_H
