#ifndef BUFFER_H
#define BUFFER_H

#include "defines.h"

typedef struct buffer{
    buffer * next, * prev;
    void * buffer_data;
    unsigned int size;
} buffer;

buffer * makeBuffer(unsigned int size);
void freeBuffer(buffer * buff, bool recursive = false);
int getBufferData(buffer * buff, void **data, bool recursive = false);

#endif // BUFFER_H
