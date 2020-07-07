#include "vector.h"

#include <stdlib.h>
#include <string.h>

static void vkt_ExpandVec(vkt_vector *vec)
{
    vec->capacity *= 2;
    vec->elements = realloc(vec->elements, vec->capacity);

    if (vec->size >= vec->capacity)
        vkt_ExpandVec(vec);
}

vkt_vector vkt_CreateVector(size_t ele_size, void (*func)(void *))
{
    vkt_vector vec;
    vec.element_size = ele_size;
    vec.size = 0;
    vec.capacity = 4;
    vec.elements = calloc(vec.capacity, ele_size);
    vec.free_func = func;

    return vec;
}

void vkt_DestroyVector(vkt_vector *vec)
{
    if (vec->free_func)
        for (char *i = (char *)vkt_VecIterBegin(vec); i != (char *)vkt_VecIterEnd(vec); i += vec->element_size)
            vec->free_func((void *)i);

    free(vec->elements);
    free(vec);
}

void vkt_PushBack(vkt_vector *vec, void *ele)
{
    if (vec->size + 1 >= vec->capacity)
        vkt_ExpandVec(vec);

    char *target = (char *)vec->elements + vec->size * vec->element_size;
    memmove(target, ele, vec->element_size);
    vec->size++;
}

void vkt_PopBack(vkt_vector *vec)
{
    if (vec->free_func)
        vec->free_func(vkt_VecGet(vec, vec->size));

    vec->size--;
}

void *vkt_VecIterBegin(vkt_vector *vec)
{
    return vec->elements;
}

void *vkt_VecIterEnd(vkt_vector *vec)
{
    return (void *)((char *)vec->elements + vec->size * vec->element_size);
}

void vkt_VecIterate(vkt_vector *vec, void (*func)(void *))
{
    for (char *i = (char *)vkt_VecIterBegin(vec); i != (char *)vkt_VecIterEnd(vec); i += vec->element_size)
        func((void *)i);
}

void *vkt_VecGet(vkt_vector *vec, uint32_t index)
{
    return (void *)((char *)vec->elements + index * vec->element_size);
}
