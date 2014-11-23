#ifndef MODEL_H
#define MODEL_H

#include "vector.h"

typedef struct {
    size_t count;
    size_t allocated;
    vector_t *vectors;
} model_t;

#define model_foreach(model, i, v) \
    for (i = 0, v = model.vectors; i < model.count; i++, v++)

model_t model();
model_t model_clone(model_t m);
void model_add(model_t *m, vector_t v);
void model_scale(model_t *m, vector_t scale);
void model_translate(model_t *m, vector_t translate);
void model_rotate(model_t *m, vector_t rotate);
void model_rotate_around(model_t *m, vector_t rotate, vector_t center);
void model_free(model_t *m);

#endif // MODEL_H
