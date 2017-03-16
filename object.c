#include <stdlib.h>
#include <string.h>
#include "object.h"
#include "tokenizer.h"

struct Object ctno_literal(const void *data, const size_t data_size,
                           enum TypeId id, struct MethodMap *meths) {
    void *new_data = malloc(data_size);
    if (!new_data) {
        return (struct Object) { .error = 1 };
    }
    memcpy(new_data, data, data_size);
    return (struct Object) {
            .data.literal = (struct LiteralData) {
                    .size = data_size,
                    .type_id = id,
                    .value = new_data
            },
            .is_literal = true,
            .methods = meths,
            .error = 0,
            .refcount = 1
    };
}

struct Object ctno_dynamic(struct PropMap pm, struct MethodMap *methods) {
    struct PropMap copy = pm_copy(pm);
    return (struct Object) {
            .data.properties = copy,
            .is_literal = false,
            .methods = methods,
            .error = 0,
            .refcount = 1
    };
}

struct Object ctno_copy(struct Object copying) {
    if (copying.error) return (struct Object) { .error = 1 };
    struct Object ret = (struct Object) {
            .error = 0,
            .is_literal = copying.is_literal,
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

bool check_for_cycles(struct Object *checking, struct Object *in) {
    if (in->is_literal) return false;
    if (checking == in) return true;
    for (size_t i = 0; i < in->data.properties.bucket_count; ++i) {
        struct PM_Bucket *bk = &in->data.properties.buckets[i];
        for (size_t j = 0; j < bk->count; ++j) {
            if (check_for_cycles(checking, bk->items[j].val)) {
                return true;
            }
        }
    }
    return false;
}

bool ctno_set_prop(struct Object *to, const char *key,
                   struct Object *adding) {
    if (!to || !adding || !key) return false;
    if (to->is_literal) return false;
    struct Object *old = pm_get(to->data.properties, key);
    if (check_for_cycles(to, adding)) return false;
    if (!pm_set(&to->data.properties, key, adding)) return false;
    ctno_claim(adding);
    ctno_free(old);
    return true;
}

struct Object *ctno_get_prop(const struct Object to, const char *key) {
    if (to.is_literal) return NULL;
    return pm_get(to.data.properties, key);
}

struct Object *ctno_claim(struct Object *claiming) {
    ++claiming->refcount;
    return claiming;
}

void ctno_free(struct Object *freeing) {
    if (freeing) {
        --freeing->refcount;
        if (!freeing->refcount) {
            mm_free(freeing->methods);
            freeing->methods = NULL;
            if (freeing->is_literal) {
                free(freeing->data.literal.value);
                freeing->data.literal.value = NULL;
            } else {
                struct PropMap *pm = &freeing->data.properties;
                for (size_t bidx = 0; bidx < pm->bucket_count; ++bidx) {
                    struct PM_Bucket *bk = &pm->buckets[bidx];
                    for (size_t iidx = 0; iidx < bk->count; ++iidx) {
                        ctno_free(bk->items[iidx].val);
                    }
                }
                pm_free(pm);
            }
        }
    }
}

struct Object tkn_value(struct Token tkn) {
    // TODO this thing's body
    return (struct Object) { .error = 1 };
}
