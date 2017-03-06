#ifndef CONCATEN_METHOD_MAP_H
#define CONCATEN_METHOD_MAP_H

#include <stddef.h>
#include <stdbool.h>

// we'll switch this to data_stack, scope_stack, token_stack once
// those are written; for now, we want a compileable and testable
// version, so we need existing types as arguments.
#define MM_VALUE_ARGS int, char *
typedef void (*MM_FUNC_TYPE)(MM_VALUE_ARGS);

// same definitions as in prop_map.h
#define MM_MAX_BUCKET_DEPTH 8
#define MM_PREF_BUCKET_DEPTH 4

struct MM_KeyValPair {
    const char *key;
    size_t key_len;
    void (*func)(MM_VALUE_ARGS);
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
    
    int error;
};

struct MethodMap mm_new(size_t);
struct MethodMap *mm_claim(struct MethodMap *);
bool mm_set(struct MethodMap *, const char *, MM_FUNC_TYPE);
MM_FUNC_TYPE mm_get(const struct MethodMap, const char *);
// NB: `true` means it was removed, `false` means it wasn't there
bool mm_remove(struct MethodMap *, const char *);
bool mm_is_key(const struct MethodMap, const char *);
bool mm_is_value(const struct MethodMap, MM_FUNC_TYPE);
bool mm_rehash(struct MethodMap *, size_t);
void mm_free(struct MethodMap *);


#endif //CONCATEN_METHOD_MAP_H
