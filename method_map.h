#ifndef CONCATEN_METHOD_MAP_H
#define CONCATEN_METHOD_MAP_H

// we'll switch this to data_stack, scope_stack, token_stack once
// those are written; for now, we want a compileable and testable
// version, so we need existing types as arguments.
#define MM_VALUE_ARGS int, char *

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
    struct PM_KeyValPair items[MM_MAX_BUCKET_COUNT];
};
struct MethodMap {
    size_t bucket_count;
    size_t bk_gr_pref;
    size_t item_count;
    struct MM_Bucket *buckets;
    
    int error;
};


#endif //CONCATEN_METHOD_MAP_H
