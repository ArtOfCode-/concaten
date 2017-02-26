#include "prop_map.h"

#include <stdlib.h>
#include <string.h>

// the amount by which we increase the map's capacity each time
#define LOAD_FACTOR 2

struct KeyValPair kvp_zero() {
    return (struct KeyValPair) {
            .key_len = 0,
            .key = NULL,
            .val = 0
    };
}
struct Bucket bk_zero() {
    struct Bucket ret;
    ret.count = 0;
    for (size_t i = 0; i < PM_MAX_BUCKET_DEPTH; ++i) {
        ret.items[i] = kvp_zero();
    }
    return ret;
}

size_t hash(const char *key) {
    // this will do for now.
    // sure, it will be an utter failure if we try to add more
    // than 8 items of the same length to the map, but that's
    // something to fix later.
    return strlen(key);
}

struct PropMap pm_new(size_t width) {
    struct Bucket *buckets = malloc(width * sizeof(struct Bucket));
    if (!buckets) {
        return (struct PropMap) { .error = 1 };
    }
    for (size_t i = 0; i < PM_MAX_BUCKET_DEPTH; ++i) {
        buckets[i] = bk_zero();
    }
    return (struct PropMap) {
            .bucket_count = width,
            .item_count = 0,
            .buckets = buckets,
            .error = 0
    };
}

bool pm_set(struct PropMap *pm, const char *key, PM_VALUE_TYPE val) {
    if (!pm || !key || val == PM_INVALID_VALUE) return false;
    size_t key_len = strlen(key);
    size_t idx = hash(key) % pm->bucket_count;
    struct Bucket *bucket = &pm->buckets[idx];
    for (size_t i = 0; i < bucket->count; ++i) {
        if (bucket->items[i].key_len == key_len &&
                strcmp(key, bucket->items[i].key) == 0) {
            bucket->items[i].val = val;
            return true;
        }
    }
    if (bucket->count == PM_MAX_BUCKET_DEPTH) {
        if (!pm_rehash(pm, pm->bucket_count * LOAD_FACTOR)) {
            return false;
        }
        // since things are in different places now, we have to reorganize
        idx = hash(key) % pm->bucket_count;
        bucket = &pm->buckets[idx];
    }
    bucket->items[bucket->count] = (struct KeyValPair) {
            .key = key,
            .key_len = strlen(key),
            .val = val
    };
    ++bucket->count;
    ++pm->item_count;
    return true;
}

PM_VALUE_TYPE pm_get(const struct PropMap pm, const char *key) {
    if (pm.item_count == 0) return PM_INVALID_VALUE;
    size_t idx = hash(key) % pm.bucket_count;
    size_t key_len = strlen(key);
    struct Bucket bucket = pm.buckets[idx];
    for (size_t i = 0; i < bucket.count; ++i) {
        if (bucket.items[i].key_len == key_len &&
                strcmp(bucket.items[i].key, key) == 0) {
            return bucket.items[i].val;
        }
    }
    return PM_INVALID_VALUE;
}

bool pm_remove(struct PropMap *pm, const char *finding) {
    if (pm->item_count == 0) return false;
    size_t idx = hash(finding) % pm->bucket_count;
    struct Bucket *bucket = &pm->buckets[idx];
    size_t finding_len = strlen(finding);
    // this should work as-is for edge cases like empty buckets and found
    // items being at the very end or beginning of their buckets.
    size_t removed;
    for (removed = 0; removed < bucket->count; ++removed) {
        if (bucket->items[removed].key_len == finding_len &&
                strcmp(finding, bucket->items[removed].key) == 0) {
            break;
        }
    }
    if (removed == bucket->count) {
        return false;
    }
    for (size_t move = removed + 1; move < bucket->count; ++move) {
        bucket->items[move - 1] = bucket->items[move];
    }
    --bucket->count;
    --pm->item_count;
    return true;
}

bool pm_is_key(const struct PropMap pm, const char *key) {
    size_t idx = hash(key) % pm.bucket_count;
    struct Bucket *bucket = &pm.buckets[idx];
    size_t finding_len = strlen(key);
    for (size_t i = 0; i < bucket->count; ++i) {
        if (bucket->items[i].key_len == finding_len && 
                strcmp(key, bucket->items[i].key) == 0) {
            return true;
        }
    }
    return false;
}

bool pm_is_value(const struct PropMap pm, PM_VALUE_TYPE v) {
    for (size_t bucket_idx = 0; bucket_idx < pm.bucket_count; ++bucket_idx) {
        const struct Bucket bk = pm.buckets[bucket_idx];
        for (size_t in_idx = 0; in_idx < bk.count; ++in_idx) {
            if (bk.items[in_idx].val == v) return true;
        }
    }
    return false;
}

// doesn't do any preexistence-checking, just bounds.
// will produce duplicate keys if not used carefully!
// but also marginally faster.
bool raw_add(struct PropMap *pm, const char *key, PM_VALUE_TYPE val) {
    size_t bucket_idx = hash(key) % pm->bucket_count;
    struct Bucket *bk = &pm->buckets[bucket_idx];
    if (bk->count == PM_MAX_BUCKET_DEPTH) return false;
    bk->items[bk->count] = (struct KeyValPair) {
            .val = val,
            .key = key,
            .key_len = strlen(key)
    };
    ++bk->count;
    ++pm->item_count;
    return true;
}

bool pm_rehash(struct PropMap *pm, size_t new_size) {
    if (new_size < pm->bucket_count) return false;
    struct PropMap new = pm_new(new_size);
    if (new.error) return false;
    for (size_t bucket_idx = 0; bucket_idx < pm->bucket_count; ++bucket_idx) {
        struct Bucket bk = pm->buckets[bucket_idx];
        for (size_t i = 0; i < bk.count; ++i) {
            if (!raw_add(&new, bk.items[i].key, bk.items[i].val)) {
                return false;
            }
        }
    }
    *pm = new;
    return true;
}

void pm_free(struct PropMap *pm) {
    free(pm->buckets);
    pm->buckets = NULL;
}
