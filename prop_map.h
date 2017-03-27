#ifndef CONCATEN_PROP_MAP_H
#define CONCATEN_PROP_MAP_H

#include <stddef.h>
#include <stdbool.h>
#include "error.h"

// Note: I'm writing this so it can be used with a different type, which is significantly more
// complex, and that I don't want to spend the time to write just yet. I'm using `int` in its
// place. These macros are defined so it's easy to switch the types out.
struct Object;
typedef struct Object *PM_VALUE_TYPE;

// if any 1 bucket is > this size, rehash
#define PM_MAX_BUCKET_DEPTH 8
// if more than 1/2 the buckets are > this size, rehash
#define PM_PREF_BUCKET_DEPTH 4

struct PM_KeyValPair {
    char *key;
    size_t key_len;
    PM_VALUE_TYPE val;
};
struct PM_Bucket {
    // count = last index + 1
    size_t count;
    struct PM_KeyValPair items[PM_MAX_BUCKET_DEPTH];
};
struct PropMap {
    size_t bucket_count;
    size_t bk_gr_pref;
    size_t item_count;
    struct PM_Bucket *buckets;
};

ERROR pm_new(size_t, struct PropMap *);
ERROR pm_copy(const struct PropMap, struct PropMap *);
ERROR pm_set(struct PropMap *, const char *, PM_VALUE_TYPE);
ERROR pm_get(const struct PropMap, const char *, PM_VALUE_TYPE *);
// NB: `true` means it was removed, `false` means it wasn't there
ERROR pm_remove(struct PropMap *, const char *);
bool pm_is_key(const struct PropMap, const char *);
bool pm_is_value(const struct PropMap, PM_VALUE_TYPE);
ERROR pm_rehash(struct PropMap *, size_t);
void pm_free(struct PropMap *);

#endif //CONCATEN_PROP_MAP_H
