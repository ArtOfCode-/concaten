#include "../tests.h"

#include "../prop_map.h"
#include "../object.h"

#define a_set(key, val) \
    tassert(pm_set(&pm, key, val), "failed to set")


struct ModuleTestResult test_prop_map() {
    size_t successes = 0, total = 0;
    struct PropMap pm = pm_new(8);
    tassert(pm.item_count == 0, "why are there items wtf");
    long num = 1;
    struct Object val = ctno_literal(&num, sizeof(long), TID_long, NULL);
    struct Object *ret;
    a_set("foobar", &val);
    tassert(pm_get(pm, "foobar") == &val, "failed to get expected value");
    a_set("asdf", &val);
    a_set("z", &val);
    a_set("crum", &val);
    tassert(pm.item_count == 4, "wrong item count");
    tassert(pm_is_key(pm, "foobar"), "failed to find existing key");
    tassert(pm_is_value(pm, &val), "how the hell, it's been added 4 times");
    tassert((ret = pm_get(pm, "z")), "failed to get existing item");
    tassert(ret == &val, "got wrong value");
    tassert(!pm_get(pm, "a"), "got by nonexistent key");
    tassert(!pm_is_key(pm, "a"), "found nonexistent key");
    a_set("00000000", &val);
    tassert(pm_get(pm, "00000000") == &val, "got wrong value");
    a_set("0000000012345678", &val);
    a_set("12345678", &val);
    a_set("abc4567887654321", &val);
    a_set("ab345678", &val);
    a_set("foobaric12345678", &val);
    a_set("a2345678", &val);
    a_set("ich bin euer sch", &val);
    a_set("I am a nice guy!", &val);
    a_set("a2345678", &val);
    a_set("ac345678", &val);
    a_set("ad345678", &val);
    tassert(pm_is_value(pm, &val), "failed to find existing value");
    tassert((ret = pm_get(pm, "z")), "failed to get existing item");
    tassert(ret == &val, "got wrong value");
    tassert(pm_remove(&pm, "z"), "failed to remove");
    tassert(!pm_get(pm, "z"), "got by removed key");
    tassert(!pm_is_key(pm, "z"), "found removed key");
    tassert(pm_is_value(pm, &val), "lost value after removal");
    pm_free(&pm);
    ctno_free(&val);
    
    return (struct ModuleTestResult) { .successes = successes, .total = total };
}