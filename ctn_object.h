#ifndef CONCATEN_CTN_OBJECT_H
#define CONCATEN_CTN_OBJECT_H

#include "prop_map.h"

struct Object {
    union {
        struct PropMap properties;
        void *value;
    } data;
    bool is_literal;
    struct MethodMap methods;
};

struct Object ctno_new(void *, size_t);
struct Object ctno_copy(const struct Object);

struct Object ctno_free(struct Object *);

#endif //CONCATEN_CTN_OBJECT_H
