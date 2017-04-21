#include "method_map.h"

#include <string.h>

#define LOAD_FACTOR 2

struct MM_KeyValPair mm_kvp_zero() {
    return (struct MM_KeyValPair) {
            .key_len = 0,
            .key = NULL,
            .func = (struct Runnable) {
                    .code.c = NULL,
                    .type = RNT_C_CODE
            }
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
        hash = ((hash << 5) + hash) ^ c;
    }
    return hash;
}

ERROR mm_new(size_t width, struct MethodMap *into) {
    struct MM_Bucket *buckets = malloc(sizeof(*buckets) * width);
    if (!buckets) {
        return MM_CTOR_MALLOC_FAIL;
    }
    for (size_t i = 0; i < width; ++i) {
        buckets[i] = mm_bk_zero();
    }
    *into = (struct MethodMap) {
            .bucket_count = width,
            .bk_gr_pref = 0,
            .item_count = 0,
            .buckets = buckets,
            .refcount = 1
    };
    return NO_ERROR;
}

ERROR mm_claim(struct MethodMap *mm) {
    // TODO Remove this check in prod.
    // in test, I don't care about methods, so I set them to NULL
    // in prod, every object must have methods; if it doesn't, it
    // should fail.
    if (mm == NULL) return NO_ERROR;
    ++mm->refcount;
    return NO_ERROR;
}

ERROR mm_set(struct MethodMap *mm, const char *key, MM_VALUE_TYPE f) {
    size_t key_len = strlen(key);
    size_t idx = mm_hash(key) % mm->bucket_count;
    struct MM_Bucket *bucket = &mm->buckets[idx];
    for (size_t i = 0; i < bucket->count; ++i) {
        if (bucket->items[i].key_len == key_len &&
            strncmp(key, bucket->items[i].key, key_len) == 0) {
            bucket->items[i].func = f;
            return NO_ERROR;
        }
    }
    if (bucket->count == MM_MAX_BUCKET_DEPTH ||
        mm->bk_gr_pref > (mm->bucket_count / 2)) {
        if (mm_rehash(mm, mm->bucket_count * LOAD_FACTOR) != NO_ERROR) {
            return MM_SET_REHASH_FAIL;
        }
        // since things are in different places now, we have to reorganize
        idx = mm_hash(key) % mm->bucket_count;
        bucket = &mm->buckets[idx];
        // note that we're not searching to see if it exists again, since
        // we already checked if it's in there.
    }
    char *key_c = malloc(key_len + 1);
    if (!key) return MM_SET_MALLOC_FAIL;
    strncpy(key_c, key, key_len + 1);
    bucket->items[bucket->count] = (struct MM_KeyValPair) {
            .key = key_c,
            .key_len = key_len,
            .func = f
    };
    ++bucket->count;
    // == instead of > because we add one at a time and this way
    // it only once (instead of once per additional item!)
    if (bucket->count == MM_PREF_BUCKET_DEPTH) ++mm->bk_gr_pref;
    ++mm->item_count;
    return NO_ERROR;
}

ERROR mm_get(const struct MethodMap mm, const char *key, MM_VALUE_TYPE *out) {
    if (mm.item_count == 0) return MM_GET_NO_KEY_FAIL;
    size_t idx = mm_hash(key) % mm.bucket_count;
    size_t key_len = strlen(key);
    struct MM_Bucket bucket = mm.buckets[idx];
    for (size_t i = 0; i < bucket.count; ++i) {
        if (bucket.items[i].key_len == key_len &&
            strncmp(bucket.items[i].key, key, key_len) == 0) {
            *out = bucket.items[i].func;
            return NO_ERROR;
        }
    }
    return MM_GET_NO_KEY_FAIL;
}

ERROR mm_remove(struct MethodMap *mm, const char *finding) {
    if (mm->item_count == 0) return MM_RMV_NO_KEY_FAIL;
    size_t idx = mm_hash(finding) % mm->bucket_count;
    struct MM_Bucket *bucket = &mm->buckets[idx];
    size_t finding_len = strlen(finding);
    // this should work as-is for edge cases like empty buckets and found
    // items being at the very end or beginning of their buckets.
    size_t removed;
    for (removed = 0; removed < bucket->count; ++removed) {
        if (bucket->items[removed].key_len == finding_len &&
            strncmp(finding, bucket->items[removed].key, finding_len) == 0) {
            break;
        }
    }
    if (removed == bucket->count) {
        return MM_RMV_NO_KEY_FAIL;
    }
    for (size_t move = removed + 1; move < bucket->count; ++move) {
        bucket->items[move - 1] = bucket->items[move];
    }
    --bucket->count;
    --mm->item_count;
    return NO_ERROR;
}

bool mm_is_key(const struct MethodMap mm, const char *key) {
    size_t idx = mm_hash(key) % mm.bucket_count;
    struct MM_Bucket *bucket = &mm.buckets[idx];
    size_t key_len = strlen(key);
    for (size_t i = 0; i < bucket->count; ++i) {
        if (bucket->items[i].key_len == key_len &&
            strncmp(key, bucket->items[i].key, key_len) == 0) {
            return true;
        }
    }
    return false;
}

bool mm_is_value(const struct MethodMap mm, MM_VALUE_TYPE f) {
    for (size_t bucket_idx = 0; bucket_idx < mm.bucket_count; ++bucket_idx) {
        const struct MM_Bucket bk = mm.buckets[bucket_idx];
        for (size_t in_idx = 0; in_idx < bk.count; ++in_idx) {
            if (rn_eq(bk.items[in_idx].func, f)) return true;
        }
    }
    return false;
}

// doesn't do any preexistence-checking, just bounds.
// will produce duplicate keys if not used carefully!
// but also marginally faster.
ERROR mm_raw_add(struct MethodMap *mm, const char *key, MM_VALUE_TYPE val) {
    size_t bucket_idx = mm_hash(key) % mm->bucket_count;
    struct MM_Bucket *bk = &mm->buckets[bucket_idx];
    if (bk->count == MM_MAX_BUCKET_DEPTH) return MM_NESTED_REHASH_FAIL;
    bk->items[bk->count] = (struct MM_KeyValPair) {
            .func = val,
            .key = key,
            .key_len = strlen(key)
    };
    ++bk->count;
    if (bk->count == MM_PREF_BUCKET_DEPTH) ++mm->bk_gr_pref;
    ++mm->item_count;
    return NO_ERROR;
}

ERROR mm_rehash(struct MethodMap *mm, size_t new_size) {
    if (new_size < mm->bucket_count) return MM_RH_BAD_SIZE_FAIL;
    struct MethodMap new;
    if (mm_new(new_size, &new) != NO_ERROR) {
        return MM_RH_CREATE_FAIL;
    }
    for (size_t bucket_idx = 0; bucket_idx < mm->bucket_count; ++bucket_idx) {
        struct MM_Bucket bk = mm->buckets[bucket_idx];
        for (size_t i = 0; i < bk.count; ++i) {
            ERROR err = mm_raw_add(&new, bk.items[i].key, bk.items[i].func);
            if (err != NO_ERROR) {
                return err;
            }
        }
    }
    mm_free(mm);
    *mm = new;
    return NO_ERROR;
}

void mm_free(struct MethodMap *mm) {
    if (mm) {
        --mm->refcount;
        if (!mm->refcount) {
            for (size_t bidx = 0; bidx < mm->bucket_count; ++bidx) {
                struct MM_Bucket bk = mm->buckets[bidx];
                for (size_t iidx = 0; iidx < bk.count; ++iidx) {
                    free((char *) bk.items[iidx].key);
                    rn_free(&bk.items[iidx].func);
                }
            }
            free(mm->buckets);
            mm->buckets = NULL;
        }
    }
}
