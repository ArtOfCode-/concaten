#include "../test.h"

#include "../prop_map.h"
#include "../object.h"

#define a_set(key, val) \
    tassert(pm_set(&pm, key, val) == NO_ERROR, "failed to set")

struct TestResult test_prop_map() {
    size_t successes = 0, total = 0;
    struct PropMap pm;
    tassert(pm_new(8, &pm) == NO_ERROR, "failed to initialize");
    tassert(pm.item_count == 0, "why are there items wtf");
    integral num = 1;
    struct Object val;
    tassert(ctno_literal(&num, sizeof(num), LTL_integral, NULL, &val) ==
                    NO_ERROR,
            "failed to initialize val");
    struct Object *ret;
    a_set("foobar", &val);
    tassert(pm_get(pm, "foobar", &ret) == NO_ERROR,
            "failed to get expected value");
    a_set("asdf", &val);
    a_set("z", &val);
    a_set("crum", &val);
    tassert(pm.item_count == 4, "wrong item count");
    tassert(pm_is_key(pm, "foobar"), "failed to find existing key");
    tassert(pm_is_value(pm, &val), "how the hell, it's been added 4 times");
    tassert(pm_get(pm, "z", &ret) == NO_ERROR, "failed to get existing item");
    tassert(ret == &val, "got wrong value");
    tassert(pm_get(pm, "a", &ret) == PM_GET_NO_KEY_FAIL, "got fake key");
    tassert(!pm_is_key(pm, "a"), "found nonexistent key");
    a_set("00000000", &val);
    tassert(pm_get(pm, "00000000", &ret) == NO_ERROR, "failed to get");
    tassert(ret == &val, "got wrong value");
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
    tassert(pm_get(pm, "z", &ret) == NO_ERROR, "failed to get existing item");
    tassert(&val == ret, "failed to get correct value");
    tassert(ret == &val, "got wrong value");
    tassert(pm_remove(&pm, "z") == NO_ERROR, "failed to remove");
    tassert(pm_get(pm, "z", &ret) == PM_GET_NO_KEY_FAIL, "got removed key");
    tassert(!pm_is_key(pm, "z"), "found removed key");
    tassert(pm_is_value(pm, &val), "lost value after removal");
    pm_free(&pm);
    ctno_free(&val);
    
    return (struct TestResult) { .successes = successes, .total = total };
}
