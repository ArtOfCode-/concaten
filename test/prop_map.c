#include "../tests.h"

#include <stdio.h>

#include "../prop_map.h"

void full_print(struct PropMap *pm) {
    printf("items:   %zu\n", pm->item_count);
    printf("buckets: %zu\n", pm->bucket_count);
    for (size_t i = 0; i < pm->bucket_count; ++i) {
        struct Bucket bk = pm->buckets[i];
        printf("  bucket %zu: %zu items\n", i, bk.count);
        for (size_t j = 0; j < bk.count; ++j) {
            struct KeyValPair kvp = bk.items[j];
            printf("    #%zu: '%s' -> %d\n", j, kvp.key, kvp.val);
        }
    }
    puts("");
}

void short_print(struct PropMap *pm) {
    printf("items: %zu\n", pm->item_count);
    for (size_t i = 0; i < pm->bucket_count; ++i) {
        struct Bucket bk = pm->buckets[i];
        for (size_t j = 0; j < bk.count; ++j) {
            printf("  '%s' -> %d\n", bk.items[j].key, bk.items[j].val);
        }
    }
    puts("");
}

void test_prop_map() {
    struct PropMap pm = pm_new(8);
    pm_set(&pm, "foobar", __LINE__);
    pm_set(&pm, "asdf", __LINE__);
    pm_set(&pm, "z", __LINE__);
    pm_set(&pm, "crum", __LINE__);
    full_print(&pm);
    pm_set(&pm, "00000000", __LINE__);
    pm_set(&pm, "0000000012345678", __LINE__);
    full_print(&pm);
    pm_set(&pm, "12345678", __LINE__);
    pm_set(&pm, "abc4567887654321", __LINE__);
    full_print(&pm);
    pm_set(&pm, "ab345678", __LINE__);
    pm_set(&pm, "foobaric12345678", __LINE__);
    full_print(&pm);
    pm_set(&pm, "a2345678", __LINE__);
    pm_set(&pm, "ich bin euer sch", __LINE__);
    full_print(&pm);
    pm_set(&pm, "I am a nice guy!", __LINE__);
    full_print(&pm);
}