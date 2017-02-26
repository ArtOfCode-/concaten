#ifndef CONCATEN_PROP_MAP_H
#define CONCATEN_PROP_MAP_H

// Note: I'm writing this so it can be used with a different type, which is significantly more
// complex, and that I don't want to spend the time to write just yet. I'm using `int` in its
// place. These macros are defined so it's easy to switch the types out.
#define PROP_MAP_VAL int
#define PROP_MAP_INVALID -1

struct StrToValPair {
    const char *key;
    size_t key_len;
    PROP_MAP_VAL val;
};
struct Bucket {
    size_t size;
    StrToValPair items[];
};
struct PropMap {
    size_t bucket_count;
    size_t item_count;
    // size_t (*hash)(const char *) // private function in .c
    Bucket buckets[];
};

struct PropMap pm_new(size_t);
bool pm_add(struct PropMap *, const char *, PROP_MAP_VAL);
bool pm_remove(struct PropMap *, const char *);
bool pm_is_key(struct PropMap, const char *);
bool pm_is_value(struct PropMap, PROP_MAP_VAL);
PROP_MAP_VAL pm_get(struct PropMap, const char *);
// TODO finish this

#endif //CONCATEN_PROP_MAP_H
