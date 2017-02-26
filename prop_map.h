#ifndef CONCATEN_PROP_MAP_H
#define CONCATEN_PROP_MAP_H


#define PROP_MAP_VAL int
#define PROP_MAP_INVALID -1

struct StrToValPair {
    const char *key;
    size_t key_len;
    int val;
};
struct Bucket {
    size_t size;
    StrToValPair *items;
};
struct PropMap {
    size_t bucket_count;
    Bucket *buckets;
};

struct PropMap pm_new();
bool pm_add(struct PropMap *, char *name, )

// TODO finish this

#endif //CONCATEN_PROP_MAP_H
