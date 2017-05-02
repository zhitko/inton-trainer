#include <stdlib.h>
#include <string>
#include <string.h>

#include "buffer.h"

buffer * makeBuffer(unsigned int size)
{
    buffer * buf = (buffer *) malloc(sizeof(buffer));
    buf->size = size;
    buf->prev = NULL;
    buf->next = NULL;
    buf->buffer_data = malloc(size);
    short * tmp = (short*) buf->buffer_data;
    for(int i=0; i<size/sizeof(short); i++, tmp++) *tmp = 0;
    return buf;
}

void freeBuffer(buffer * buff, bool recursive)
{
    if(!buff)
    {
        free(buff->buffer_data);
        if(recursive) freeBuffer(buff->next, recursive);
        free(buff);
    }
}

int getBufferData(buffer * buff, void **data, bool recursive)
{
    int size = 0;
    *data = NULL;
    do
    {
        int initSize = size;
        size += buff->size;
        if(*data)
            *data = (void *) realloc(*data, size);
        else
            *data = (void *) malloc(size);
        memcpy((char*)*data + initSize, buff->buffer_data, buff->size);
        buff = buff->next;

    } while(recursive && buff);
    return size;
}
