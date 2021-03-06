#include "prop_map.h"
#include "object.h"

#include <stdlib.h>
#include <string.h>

// the amount by which we increase the map's capacity each time
#define LOAD_FACTOR 2

struct PM_KeyValPair pm_kvp_zero() {
    return (struct PM_KeyValPair) {
            .key_len = 0,
            .key = NULL,
            .val = 0
    };
}
struct PM_Bucket pm_bk_zero() {
    struct PM_Bucket ret;
    ret.count = 0;
    for (size_t i = 0; i < PM_MAX_BUCKET_DEPTH; ++i) {
        ret.items[i] = pm_kvp_zero();
    }
    return ret;
}

size_t pm_hash(const char *key) {
    // taken from http://stackoverflow.com/a/7666577
    size_t hash = 5381;
    char c;
    while ((c = *key++) != 0) {
        hash = ((hash << 5) + hash) ^ c;
    }
    return hash;
}

// doesn't do any preexistence-checking, just bounds.
// will produce duplicate keys if not used carefully!
// but also marginally faster.
bool pm_raw_add(struct PropMap *pm, char *key, PM_VALUE_TYPE val) {
    size_t bucket_idx = pm_hash(key) % pm->bucket_count;
    struct PM_Bucket *bk = &pm->buckets[bucket_idx];
    if (bk->count == PM_MAX_BUCKET_DEPTH) return false;
    bk->items[bk->count] = (struct PM_KeyValPair) {
            .val = val,
            .key = key,
            .key_len = strlen(key) + 1
    };
    ++bk->count;
    if (bk->count == PM_PREF_BUCKET_DEPTH) ++pm->bk_gr_pref;
    ++pm->item_count;
    return true;
}

ERROR pm_new(size_t width, struct PropMap *into) {
    struct PM_Bucket *buckets = malloc(sizeof(*buckets) * width);
    if (!buckets) {
        return PM_CTOR_MALLOC_FAIL;
    }
    for (size_t i = 0; i < PM_MAX_BUCKET_DEPTH; ++i) {
        buckets[i] = pm_bk_zero();
    }
    *into = (struct PropMap) {
            .bucket_count = width,
            .bk_gr_pref = 0,
            .item_count = 0,
            .buckets = buckets,
    };
    return NO_ERROR;
}

ERROR pm_copy(const struct PropMap copying, struct PropMap *into) {
    struct PropMap ret;
    if (pm_new(copying.bucket_count, &ret) != NO_ERROR) {
        return PM_COPY_CREATE_FAIL;
    }
    for (size_t bidx = 0; bidx < copying.bucket_count; ++bidx) {
        for (size_t iidx = 0; iidx < copying.buckets[bidx].count; ++iidx) {
            struct PM_KeyValPair item = copying.buckets[bidx].items[iidx];
            char *new_key = malloc(item.key_len);
            if (!new_key) return PM_COPY_MALLOC_FAIL;
            strncpy(new_key, item.key, item.key_len);
            pm_raw_add(&ret, new_key, item.val);
        }
    }
    *into = ret;
    return NO_ERROR;
}

ERROR pm_set(struct PropMap *pm, const char *k, PM_VALUE_TYPE val) {
    size_t key_len = strlen(k) + 1;
    size_t key_hash_raw = pm_hash(k);
    size_t idx = key_hash_raw % pm->bucket_count;
    struct PM_Bucket *bucket = &pm->buckets[idx];
    for (size_t i = 0; i < bucket->count; ++i) {
        if (bucket->items[i].key_len == key_len &&
                strncmp(k, bucket->items[i].key, key_len) == 0) {
            if (ctno_claim(val) != NO_ERROR) {
                return PM_SET_CLAIM_FAIL;
            }
            ctno_free(bucket->items[i].val);
            bucket->items[i].val = val;
            return NO_ERROR;
        }
    }
    if (bucket->count == PM_MAX_BUCKET_DEPTH ||
            pm->bk_gr_pref > (pm->bucket_count / 2)) {
        if (pm_rehash(pm, pm->bucket_count * LOAD_FACTOR) != NO_ERROR) {
            return PM_SET_REHASH_FAIL;
        }
        // since things are in different places now, we have to reorganize
        idx = key_hash_raw % pm->bucket_count;
        bucket = &pm->buckets[idx];
        // note that we're not searching to see if it exists again, since
        // we already checked if it's in there.
    }
    char *key = malloc(key_len);
    if (!key) return PM_SET_MALLOC_FAIL;
    strncpy(key, k, key_len);
    if (ctno_claim(val) != NO_ERROR) {
        return PM_SET_CLAIM_FAIL;
    }
    bucket->items[bucket->count] = (struct PM_KeyValPair) {
            .key = key,
            .key_len = key_len,
            .val = val
    };
    ++bucket->count;
    // == instead of > because we add one at a time and this way
    // it only once (instead of once per additional item!)
    if (bucket->count == PM_PREF_BUCKET_DEPTH) ++pm->bk_gr_pref;
    ++pm->item_count;
    return NO_ERROR;
}

ERROR pm_get(const struct PropMap pm, const char *key, PM_VALUE_TYPE *out) {
    if (pm.item_count == 0) return PM_GET_NO_KEY_FAIL;
    size_t idx = pm_hash(key) % pm.bucket_count;
    size_t key_len = strlen(key) + 1;
    struct PM_Bucket bucket = pm.buckets[idx];
    for (size_t i = 0; i < bucket.count; ++i) {
        if (bucket.items[i].key_len == key_len &&
                strncmp(bucket.items[i].key, key, key_len) == 0) {
            *out = bucket.items[i].val;
            return NO_ERROR;
        }
    }
    return PM_GET_NO_KEY_FAIL;
}

ERROR pm_remove(struct PropMap *pm, const char *key) {
    if (pm->item_count == 0) return PM_RMV_NO_KEY_FAIL;
    size_t idx = pm_hash(key) % pm->bucket_count;
    struct PM_Bucket *bucket = &pm->buckets[idx];
    size_t key_len = strlen(key) + 1;
    // this should work as-is for edge cases like empty buckets and found
    // items being at the very end or beginning of their buckets.
    size_t removed;
    for (removed = 0; removed < bucket->count; ++removed) {
        if (bucket->items[removed].key_len == key_len &&
                strncmp(key, bucket->items[removed].key, key_len) == 0) {
            break;
        }
    }
    if (removed == bucket->count) {
        return PM_RMV_NO_KEY_FAIL;
    }
    free(bucket->items[removed].key);
    ctno_free(bucket->items[removed].val);
    for (size_t move = removed + 1; move < bucket->count; ++move) {
        bucket->items[move - 1] = bucket->items[move];
    }
    --bucket->count;
    --pm->item_count;
    return NO_ERROR;
}

bool pm_is_key(const struct PropMap pm, const char *key) {
    size_t idx = pm_hash(key) % pm.bucket_count;
    struct PM_Bucket *bucket = &pm.buckets[idx];
    size_t key_len = strlen(key) + 1;
    for (size_t i = 0; i < bucket->count; ++i) {
        if (bucket->items[i].key_len == key_len && 
                strncmp(key, bucket->items[i].key, key_len) == 0) {
            return true;
        }
    }
    return false;
}

bool pm_is_value(const struct PropMap pm, PM_VALUE_TYPE v) {
    for (size_t bucket_idx = 0; bucket_idx < pm.bucket_count; ++bucket_idx) {
        const struct PM_Bucket bk = pm.buckets[bucket_idx];
        for (size_t in_idx = 0; in_idx < bk.count; ++in_idx) {
            if (bk.items[in_idx].val == v) return true;
        }
    }
    return false;
}

ERROR pm_rehash(struct PropMap *pm, size_t new_size) {
    if (new_size < pm->bucket_count) return PM_RH_BAD_SIZE_FAIL;
    struct PropMap new;
    if (pm_new(new_size, &new) != NO_ERROR) return PM_RH_CREATE_FAIL;
    for (size_t bucket_idx = 0; bucket_idx < pm->bucket_count; ++bucket_idx) {
        struct PM_Bucket bk = pm->buckets[bucket_idx];
        for (size_t i = 0; i < bk.count; ++i) {
            if (!pm_raw_add(&new, bk.items[i].key, bk.items[i].val)) {
                return PM_NESTED_REHASH_FAIL;
            }
        }
    }
    free(pm);
    *pm = new;
    return NO_ERROR;
}

void pm_free(struct PropMap *pm) {
    if (pm) {
        for (size_t bidx = 0; bidx < pm->bucket_count; ++bidx) {
            struct PM_Bucket bk = pm->buckets[bidx];
            for (size_t iidx = 0; iidx < bk.count; ++iidx) {
                free(bk.items[iidx].key);
                ctno_free(bk.items[iidx].val);
            }
        }
        free(pm->buckets);
    }
}
