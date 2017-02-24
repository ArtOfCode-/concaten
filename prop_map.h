#ifndef CONCATEN_PROP_MAP_H
#define CONCATEN_PROP_MAP_H

struct StrToValPair {
    const char *key;
    size_t key_len;
    int val; // TODO once Object is written, use it
};
struct Bucket {
    size_t size;
    StrToValPair *items;
};
struct PropMap {
    size_t bucket_count;
    Bucket *buckets;
};

// TODO finish this

#endif //CONCATEN_PROP_MAP_H
