#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "object.h"

ERROR ctno_literal(const void *data, const size_t data_size,
                   enum LiteralType type, struct MethodMap *meths,
                   struct Object *into) {
    void *new_data = malloc(data_size);
    if (!new_data) {
        return CTNO_CTOR_MALLOC_FAIL;
    }
    memcpy(new_data, data, data_size);
    *into = (struct Object) {
            .data.literal = (struct LiteralData) {
                    .size = data_size,
                    .value = new_data,
                    .type = type
            },
            .is_literal = true,
            .methods = meths,
            .refcount = 1
    };
    return NO_ERROR;
}

ERROR ctno_dynamic(struct PropMap pm, struct MethodMap *methods,
                   struct Object *into) {
    struct PropMap copy;
    if (pm_copy(pm, &copy) != NO_ERROR) return CTNO_CTOR_PM_COPY_FAIL;
    *into = (struct Object) {
            .data.properties = copy,
            .is_literal = false,
            .methods = methods,
            .refcount = 1
    };
    return NO_ERROR;
}

ERROR ctno_copy(const struct Object *copying, struct Object *into) {
    if (mm_claim(copying.methods) != NO_ERROR) return CTNO_COPY_CLAIM_FAIL;
    struct Object ret = (struct Object) {
            .is_literal = copying.is_literal,
            .methods = copying.methods,
            .refcount = 1
    };
    if (copying.is_literal) {
        const size_t width = copying.data.literal.size;
        void *d_c = malloc(width);
        if (!d_c) return CTNO_COPY_DATA_FAIL;
        memcpy(d_c, copying.data.literal.value, width);
        ret.data.literal.size = width;
        ret.data.literal.value = d_c;
    } else {
        struct PropMap pm_c;
        if (pm_copy(copying.data.properties, &pm_c) != NO_ERROR) {
            return CTNO_COPY_PROPS_FAIL;
        }
        for (size_t bidx = 0; bidx < pm_c.bucket_count; ++bidx) {
            struct PM_Bucket *bk = &pm_c.buckets[bidx];
            for (size_t iidx = 0; iidx < bk->count; ++iidx) {
                if (ctno_claim(bk->items[iidx].val) != NO_ERROR) {
                    return CTNO_COPY_CLAIM_FAIL;
                }
            }
        }
        ret.data.properties = pm_c;
    }
    *into = ret;
    return NO_ERROR;
}

bool ctno_eq(const struct Object *lhs, const struct Object *rhs) {
    if (lhs.is_literal != rhs.is_literal) {
        return false;
    } else if (lhs.is_literal) {
        const struct LiteralData ld = lhs.data.literal;
        const struct LiteralData rd = rhs.data.literal;
        return ld.type == rd.type && ld.size == rd.size &&
               strncmp(ld.value, rd.value, ld.size) == 0;
    } else {
        const struct PropMap lp = lhs.data.properties;
        const struct PropMap rp = rhs.data.properties;
        if (lp.bucket_count != rp.bucket_count ||
                lp.item_count != rp.item_count) {
            return false;
        }
        for (size_t bidx = 0; bidx < lp.bucket_count; ++bidx) {
            const struct PM_Bucket lpb = lp.buckets[bidx];
            const struct PM_Bucket rpb = rp.buckets[bidx];
            if (lpb.count != rpb.count) return false;
            for (size_t iidx = 0; iidx < lpb.count; ++iidx) {
                struct PM_KeyValPair li = lpb.items[iidx];
                struct PM_KeyValPair ri = rpb.items[iidx];
                if (li.key_len != ri.key_len ||
                        strncmp(li.key, ri.key, li.key_len) != 0 ||
                        !ctno_eq(li.val, ri.val)) return false;
            }
        }
        return true;
    }
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

ERROR ctno_set_prop(struct Object *to, const char *key,
                   struct Object *adding) {
    if (to->is_literal) return CTNO_SET_LITERAL_FAIL;
    struct Object *old;
    ERROR err = pm_get(to->data.properties, key, &old);
    if (err != NO_ERROR && err != PM_GET_NO_KEY_FAIL) {
        return CTNO_SET_PM_GET_FAIL;
    } else {
        old = NULL;
    }
    if (check_for_cycles(to, adding)) return CTNO_SET_CYCLE_FAIL;
    if (ctno_claim(adding) != NO_ERROR) return CTNO_SET_CLAIM_FAIL;
    if (pm_set(&to->data.properties, key, adding) != NO_ERROR) {
        return CTNO_SET_ADD_FAIL;
    }
    ctno_free(old);
    return NO_ERROR;
}

ERROR ctno_get_prop(const struct Object *from, const char *key,
                    struct Object **into) {
    if (from.is_literal) return CTNO_GET_LITERAL_FAIL;
    struct Object *got;
    ERROR err = pm_get(from.data.properties, key, &got);
    if (err != NO_ERROR) {
        if (err != PM_GET_NO_KEY_FAIL) {
            return CTNO_GET_PM_GET_FAIL;
        } else {
            return CTNO_GET_NO_KEY_FAIL;
        }
    }
    *into = got;
    return NO_ERROR;
}

ERROR ctno_claim(struct Object *claiming) {
    if (claiming->refcount == SIZE_MAX) return CTNO_CLAIM_MAX_REFCOUNT;
    ++claiming->refcount;
    return NO_ERROR;
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
            free(freeing); // ensures that we didn't ctno_free(&sth);
        }
    }
}
