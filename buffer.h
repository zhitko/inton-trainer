#ifndef BUFFER_H
#define BUFFER_H

typedef struct buffer{
    buffer * next, * prev;
    void * buffer;
    unsigned int size;
} buffer;

buffer * makeBuffer(unsigned int size);
void freeBuffer(buffer * buff, bool recursive = false);
int getBufferData(buffer * buff, void **data, bool recursive = false);

#endif // BUFFER_H
