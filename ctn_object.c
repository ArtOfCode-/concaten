#include <stdlib.h>
#include <string.h>
#include "ctn_object.h"

struct Object ctno_literal(const void *data, size_t data_size, struct MethodMap *methods) {
    void *new_data = malloc(data_size);
    if (!new_data) {
        return (struct Object) { .error = 1 };
    }
    memcpy(new_data, data, data_size);
    return (struct Object) {
            .data.literal.size = data_size,
            .data.literal.value = new_data,
            .is_literal = true,
            .methods = methods,
            .error = 0,
            .refcount = 0
    };
}

struct Object ctno_dynamic(struct PropMap pm, struct MethodMap *methods) {
    struct PropMap copy = pm_copy(pm);
    return (struct Object) {
            .data.properties = copy,
            .is_literal = false,
            .methods = methods,
            .error = 0,
            .refcount = 0
    };
}

struct Object ctno_copy(struct Object copying) {
    if (copying.error) return (struct Object) { .error = 1 };
    struct Object ret = (struct Object) {
            .error = 0,
            .is_literal = copying.is_literal,
            // intentionally not copied
            .methods = mm_claim(copying.methods),
            .refcount = 0
    };
    if (copying.is_literal) {
        const size_t width = copying.data.literal.size;
        void *d_c = malloc(width);
        if (!d_c) return (struct Object) { .error = 1 };
        ret.data.literal.size = width;
        ret.data.literal.value = d_c;
    } else {
        struct PropMap pm_c = pm_copy(copying.data.properties);
        ret.data.properties = pm_c;
    }
    return ret;
}

struct Object *ctno_claim(struct Object *claiming) {
    ++claiming->refcount;
    return claiming;
}

void ctno_free(struct Object *freeing) {
    --freeing->refcount;
    if (!freeing->refcount) {
        mm_free(freeing->methods);
        freeing->methods = NULL;
        if (freeing->is_literal) {
            free(freeing->data.literal.value);
            freeing->data.literal.value = NULL;
        } else {
            pm_free(&freeing->data.properties);
        }
    }
}
