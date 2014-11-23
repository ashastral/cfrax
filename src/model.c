#include <stdlib.h>
#include <string.h>

#include "model.h"

static size_t allocate_size = 0x100;
static model_t empty = {0, 0, NULL};

model_t model()
{
    return empty;
}

model_t model_clone(model_t m)
{
    model_t clone = empty;
    clone.count = m.count;
    clone.allocated = m.allocated;
    clone.vectors = malloc(sizeof(*clone.vectors) * clone.allocated);
    memcpy(clone.vectors, m.vectors, sizeof(*clone.vectors) * clone.allocated);
    return clone;
}

void model_add(model_t *m, vector_t v)
{
    m->count++;
    while (m->count > m->allocated) {
        m->allocated += allocate_size;
        m->vectors = realloc(m->vectors, sizeof(*m->vectors) * m->allocated);
    }
    m->vectors[m->count - 1] = v;
}

static void model_each_vector(
        model_t *m,
        vector_t (*func)(vector_t, vector_t),
        vector_t other)
{
    int i;
    for (i = 0; i < m->count; i++) {
        m->vectors[i] = func(m->vectors[i], other);
    }
}

void model_scale(model_t *m, vector_t scale)
{
    model_each_vector(m, vector_scale, scale);
}

void model_translate(model_t *m, vector_t translate)
{
    model_each_vector(m, vector_translate, translate);
}

void model_rotate(model_t *m, vector_t rotate)
{
    model_each_vector(m, vector_rotate, rotate);
}

void model_rotate_around(model_t *m, vector_t rotate, vector_t center)
{
    model_translate(m, vector_negate(center));
    model_rotate(m, rotate);
    model_translate(m, center);
}

void model_free(model_t *m)
{
    free(m->vectors);
    m->vectors = NULL;
}
