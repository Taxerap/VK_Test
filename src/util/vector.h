#ifndef _VK_TEST_VECTOR_H_
#define _VK_TEST_VECTOR_H_

#include <inttypes.h>
#include <stddef.h>

typedef struct vkt_vector_t
{
    void *elements;
    size_t element_size;
    uint32_t size;
    uint32_t capacity;
    void (*free_func)(void *);
} vkt_vector;

vkt_vector vkt_CreateVector(size_t, void (*)(void *));
void vkt_DestroyVector(vkt_vector *);
void vkt_PushBack(vkt_vector *, void *);
void vkt_PopBack(vkt_vector *);
void *vkt_VecIterBegin(vkt_vector *);
void *vkt_VecIterEnd(vkt_vector *);
void vkt_VecIterate(vkt_vector *, void (*)(void *));
void *vkt_VecGet(vkt_vector *, uint32_t index);

#endif
