#ifndef CONCATEN_METHOD_MAP_H
#define CONCATEN_METHOD_MAP_H

#include <stddef.h>
#include <stdbool.h>
#include "error.h"

// we'll switch this to data_stack, scope_stack, token_stack once
// those are written; for now, we want a compileable and testable
// version, so we need existing types as arguments.
typedef void (*MM_VALUE_TYPE)(int, char *);

// same definitions as in prop_map.h
#define MM_MAX_BUCKET_DEPTH 8
#define MM_PREF_BUCKET_DEPTH 4

struct MM_KeyValPair {
    const char *key;
    size_t key_len;
    MM_VALUE_TYPE func;
};
struct MM_Bucket {
    size_t count;
    struct MM_KeyValPair items[MM_MAX_BUCKET_DEPTH];
};
struct MethodMap {
    size_t bucket_count;
    size_t bk_gr_pref;
    size_t item_count;
    struct MM_Bucket *buckets;
    size_t refcount;
};

ERROR mm_new(size_t, struct MethodMap *);
ERROR mm_claim(struct MethodMap *);
ERROR mm_set(struct MethodMap *, const char *, MM_VALUE_TYPE);
ERROR mm_get(const struct MethodMap, const char *, MM_VALUE_TYPE *);
// NB: `true` means it was removed, `false` means it wasn't there
ERROR mm_remove(struct MethodMap *, const char *);
bool mm_is_key(const struct MethodMap, const char *);
bool mm_is_value(const struct MethodMap, MM_VALUE_TYPE);
ERROR mm_rehash(struct MethodMap *, size_t);
void mm_free(struct MethodMap *);


#endif //CONCATEN_METHOD_MAP_H
