#include "../tests.h"

#include "../prop_map.h"
#include "../object.h"

// TODO update this to use Object instead of ints

void full_print(const struct PropMap pm){
    printf("items: %zu\n", pm.item_count);
    printf("bk_gr_pref: %zu\n", pm.bk_gr_pref);
    printf("buckets: %zu\n", pm.bucket_count);
    for (size_t i = 0; i < pm.bucket_count; ++i) {
        struct PM_Bucket bk = pm.buckets[i];
        printf("  bucket %zu: %zu items\n", i, bk.count);
        for (size_t j = 0; j < bk.count; ++j) {
            struct PM_KeyValPair kvp = bk.items[j];
            printf("    #%zu: '%s' -> %p\n", j, kvp.key, kvp.val);
        }
    }
    puts("");
}

void test_prop_map() {
    // TODO Convert to success counter
    struct PropMap pm = pm_new(8);
    full_print(pm);
    int num = 1;
    struct Object val = ctno_from(num, NULL);
    pm_set(&pm, "foobar", &val);
    pm_set(&pm, "asdf", &val);
    pm_set(&pm, "z", &val);
    pm_set(&pm, "crum", &val);
    full_print(pm);
    pm_set(&pm, "00000000", &val);
    printf("%p\n", pm_get(pm, "00000000"));
    printf("%p\n", pm_get(pm, "z"));
    full_print(pm);
    pm_set(&pm, "0000000012345678", &val);
    pm_set(&pm, "12345678", &val);
    pm_set(&pm, "abc4567887654321", &val);
    pm_set(&pm, "ab345678", &val);
    pm_set(&pm, "foobaric12345678", &val);
    pm_set(&pm, "a2345678", &val);
    pm_set(&pm, "ich bin euer sch", &val);
    full_print(pm);
    pm_set(&pm, "I am a nice guy!", &val);
    pm_set(&pm, "a2345678", &val);
    pm_set(&pm, "ac345678", &val);
    pm_set(&pm, "ad345678", &val);
    full_print(pm);
    printf("get z: %p\n", pm_get(pm, "z"));
    printf("is_key z: %d\n", pm_is_key(pm, "z"));
    pm_remove(&pm, "z");
    printf("get z: %p\n", pm_get(pm, "z"));
    printf("is_key z: %d\n", pm_is_key(pm, "z"));
    pm_free(&pm);
}