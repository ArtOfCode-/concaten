#ifndef CONCATEN_PROP_MAP_H
#define CONCATEN_PROP_MAP_H

#include <stddef.h>
#include <stdbool.h>

// Note: I'm writing this so it can be used with a different type, which is significantly more
// complex, and that I don't want to spend the time to write just yet. I'm using `int` in its
// place. These macros are defined so it's easy to switch the types out.
#define PROP_MAP_VAL int
#define PROP_MAP_INVALID -1

struct Bucket;
struct PropMap {
    size_t bucket_count;
    size_t item_count;
    // size_t (*hash)(const char *) // private function in .c
    struct Bucket *buckets;

    int error;
};

struct PropMap pm_new(size_t);
bool pm_set(struct PropMap *, const char *, PROP_MAP_VAL);
// TODO try_get_value?
PROP_MAP_VAL pm_get(const struct PropMap, const char *);
// NB: `true` means it was removed, `false` means it wasn't there
bool pm_remove(struct PropMap *, const char *);
bool pm_is_key(const struct PropMap, const char *);
bool pm_is_value(const struct PropMap, PROP_MAP_VAL);
bool pm_rehash(struct PropMap *, size_t);
void pm_free(struct PropMap *);

#endif //CONCATEN_PROP_MAP_H
