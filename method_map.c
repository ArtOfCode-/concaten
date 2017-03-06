#include "method_map.h"

#include <stdlib.h>
#include <string.h>

#define LOAD_FACTOR 2

struct MM_KeyValPair mm_kvp_zero() {
    return (struct MM_KeyValPair) {
            .key_len = 0,
            .key = NULL,
            .func = NULL
    };
}
struct MM_Bucket mm_bk_zero() {
    struct MM_Bucket ret;
    ret.count = 0;
    for (size_t i = 0; i < MM_MAX_BUCKET_DEPTH; ++i) {
        ret.items[i] = mm_kvp_zero();
    }
    return ret;
}

size_t mm_hash(const char *key) {
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
        buckets[i] = mm_bk_zero();
    }
    return (struct MethodMap) {
            .bucket_count = width,
            .bk_gr_pref = 0,
            .item_count = 0,
            .buckets = buckets,
            .error = 0
    };
}

struct MethodMap *mm_claim(struct MethodMap *mm) {
    ++mm->refcount;
    return mm;
}

bool mm_set(struct MethodMap *mm, const char *key, MM_FUNC_TYPE f) {
    if (!mm || !key) return false;
    size_t key_len = strlen(key);
    size_t idx = mm_hash(key) % mm->bucket_count;
    struct MM_Bucket *bucket = &mm->buckets[idx];
    for (size_t i = 0; i < bucket->count; ++i) {
        if (bucket->items[i].key_len == key_len &&
            strcmp(key, bucket->items[i].key) == 0) {
            bucket->items[i].func = f;
            return true;
        }
    }
    if (bucket->count == MM_MAX_BUCKET_DEPTH ||
        mm->bk_gr_pref > (mm->bucket_count / 2)) {
        if (!mm_rehash(mm, mm->bucket_count * LOAD_FACTOR)) {
            return false;
        }
        // since things are in different places now, we have to reorganize
        idx = mm_hash(key) % mm->bucket_count;
        bucket = &mm->buckets[idx];
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
    if (bucket->count == MM_PREF_BUCKET_DEPTH) ++mm->bk_gr_pref;
    ++mm->item_count;
    return true;
}

MM_FUNC_TYPE mm_get(const struct MethodMap mm, const char *key) {
    if (mm.item_count == 0) return NULL;
    size_t idx = mm_hash(key) % mm.bucket_count;
    size_t key_len = strlen(key);
    struct MM_Bucket bucket = mm.buckets[idx];
    for (size_t i = 0; i < bucket.count; ++i) {
        if (bucket.items[i].key_len == key_len &&
            strcmp(bucket.items[i].key, key) == 0) {
            return bucket.items[i].func;
        }
    }
    return NULL;
}

bool mm_remove(struct MethodMap *mm, const char *finding) {
    if (mm->item_count == 0) return false;
    size_t idx = mm_hash(finding) % mm->bucket_count;
    struct MM_Bucket *bucket = &mm->buckets[idx];
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
    --mm->item_count;
    return true;
}

bool mm_is_key(const struct MethodMap mm, const char *key) {
    size_t idx = mm_hash(key) % mm.bucket_count;
    struct MM_Bucket *bucket = &mm.buckets[idx];
    size_t finding_len = strlen(key);
    for (size_t i = 0; i < bucket->count; ++i) {
        if (bucket->items[i].key_len == finding_len &&
            strcmp(key, bucket->items[i].key) == 0) {
            return true;
        }
    }
    return false;
}

bool mm_is_value(const struct MethodMap mm, MM_FUNC_TYPE f) {
    for (size_t bucket_idx = 0; bucket_idx < mm.bucket_count; ++bucket_idx) {
        const struct MM_Bucket bk = mm.buckets[bucket_idx];
        for (size_t in_idx = 0; in_idx < bk.count; ++in_idx) {
            if (bk.items[in_idx].func == f) return true;
        }
    }
    return false;
}

// doesn't do any preexistence-checking, just bounds.
// will produce duplicate keys if not used carefully!
// but also marginally faster.
bool mm_raw_add(struct MethodMap *mm, const char *key, MM_FUNC_TYPE val) {
    size_t bucket_idx = mm_hash(key) % mm->bucket_count;
    struct MM_Bucket *bk = &mm->buckets[bucket_idx];
    if (bk->count == MM_MAX_BUCKET_DEPTH) return false;
    bk->items[bk->count] = (struct MM_KeyValPair) {
            .func = val,
            .key = key,
            .key_len = strlen(key)
    };
    ++bk->count;
    if (bk->count == MM_PREF_BUCKET_DEPTH) ++mm->bk_gr_pref;
    ++mm->item_count;
    return true;
}

bool mm_rehash(struct MethodMap *mm, size_t new_size) {
    if (new_size < mm->bucket_count) return false;
    struct MethodMap new = mm_new(new_size);
    if (new.error) return false;
    for (size_t bucket_idx = 0; bucket_idx < mm->bucket_count; ++bucket_idx) {
        struct MM_Bucket bk = mm->buckets[bucket_idx];
        for (size_t i = 0; i < bk.count; ++i) {
            if (!mm_raw_add(&new, bk.items[i].key, bk.items[i].func)) {
                return false;
            }
        }
    }
    *mm = new;
    return true;
}

void mm_free(struct MethodMap *mm) {
    --mm->refcount;
    if (!mm->refcount) {
        free(mm->buckets);
        mm->buckets = NULL;
    }
}
