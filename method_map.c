#include "method_map.h"

#include <stdlib.h>
#include <string.h>

#define LOAD_FACTOR 2

struct MM_KeyValPair kvp_zero() {
    return (struct MM_KeyValPair) {
            .key_len = 0,
            .key = NULL,
            .func = NULL
    };
}
struct MM_Bucket bk_zero() {
    struct MM_Bucket ret;
    ret.count = 0;
    for (size_t i = 0; i < MM_MAX_BUCKET_DEPTH; ++i) {
        ret.items[i] = kvp_zero();
    }
    return ret;
}

size_t hash(const char *key) {
    // taken from http://stackoverflow.com/a/7666577/1863564
    size_t hash = 5381;
    char c;
    while ((c = *key++) != 0) {
        hash = (hash * 33) ^ c;
    }
    return hash;
}

struct MethodMap mm_new(size_t width) {
    struct MM_Bucket *buckets = malloc(width * sizeof(struct MM_Bucket));
    if (!buckets) {
        return (struct MethodMap) { .error = 1 };
    }
    for (size_t i = 0; i < MM_MAX_BUCKET_DEPTH; ++i) {
        buckets[i] = bk_zero();
    }
    return (struct MethodMap) {
            .bucket_count = width,
            .bk_gr_pref = 0,
            .item_count = 0,
            .buckets = buckets,
            .error = 0
    };
}

bool mm_set(struct MethodMap *pm, const char *key, mm_func f) {
    if (!pm || !key) return false;
    size_t key_len = strlen(key);
    size_t idx = hash(key) % pm->bucket_count;
    struct MM_Bucket *bucket = &pm->buckets[idx];
    for (size_t i = 0; i < bucket->count; ++i) {
        if (bucket->items[i].key_len == key_len &&
            strcmp(key, bucket->items[i].key) == 0) {
            bucket->items[i].func = f;
            return true;
        }
    }
    if (bucket->count == MM_MAX_BUCKET_DEPTH ||
        pm->bk_gr_pref > (pm->bucket_count / 2)) {
        if (!mm_rehash(pm, pm->bucket_count * LOAD_FACTOR)) {
            return false;
        }
        // since things are in different places now, we have to reorganize
        idx = hash(key) % pm->bucket_count;
        bucket = &pm->buckets[idx];
        // note that we're not searching to see if it exists again, since
        // we already checked if it's in there.
    }
    bucket->items[bucket->count] = (struct MM_KeyValPair) {
            .key = key,
            .key_len = strlen(key),
            .func = f
    };
    ++bucket->count;
    // == instead of > because we add one at a time and this way
    // it only once (instead of once per additional item!)
    if (bucket->count == MM_PREF_BUCKET_DEPTH) ++pm->bk_gr_pref;
    ++pm->item_count;
    return true;
}


