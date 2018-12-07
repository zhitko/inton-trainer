#ifndef MASK_DATA_H
#define MASK_DATA_H

extern "C" {
    #include "./sptk/vector.h"
}

typedef struct {
    intvector pointsFrom;
    intvector pointsLength;
} MaskData;

#endif // MASK_DATA_H
