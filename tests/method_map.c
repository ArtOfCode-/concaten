#include "../tests.h"

#include "../method_map.h"

#define test_f(num) \
    void f##num(int i, char *s) { printf("f" str(num) ": %d; %s\n", i, s); }

test_f(0)  test_f(1)  test_f(2)  test_f(3)  test_f(4)
test_f(5)  test_f(6)  test_f(7)  test_f(8)  test_f(9)
test_f(10) test_f(11) test_f(12) test_f(13) test_f(14)
test_f(15) test_f(16) test_f(17) test_f(18) test_f(19)

#define a_set(key, value) \
    tassert(mm_set(&mm, key, value) == NO_ERROR, "assignment failed");
#define a_is_key(key) \
    tassert(mm_is_key(mm, key), key " isn't a key!")
#define a_not_key(key) \
    tassert(!mm_is_key(mm, key), key " is a key!")
#define a_is_val(val) \
    tassert(mm_is_value(mm, val), str(val) " isn't a value!")
#define a_not_val(val) \
    tassert(!mm_is_value(mm, val), str(val) " is a value!")
#define a_get_val(key, expect) \
    tassert(mm_get(mm, key, &val) == NO_ERROR, "failed to get for " key); \
    tassert(val == expect, "unexpected value for " key)
#define a_no_get_val(key) \
    tassert(mm_get(mm, key, &val) != NO_ERROR, "successfully got for " key)
#define a_remove(key) \
    tassert(mm_remove(&mm, key) == NO_ERROR, "failed to remove")

struct TestResult test_method_map() {
    size_t successes = 0, total = 0;
    // very low number of buckets initially so it has to rehash
    // ...eventually. I hate the pigeonhole principle.
    struct MethodMap mm;
    MM_VALUE_TYPE val;
    tassert(mm_new(2, &mm) == NO_ERROR, "failed to initialize");
    a_set("first", f0);
    a_set("second", f1);
    a_set("third", f2);
    a_set("fourth", f3);
    a_set("fifth", f4);
    a_is_key("fifth");
    a_not_key("bar");
    a_not_key("tenth");
    a_set("sixth", f5);
    a_set("seventh", f6);
    a_set("eighth", f7);
    a_is_key("eighth");
    a_get_val("eighth", f7);
    a_remove("eighth");
    a_not_key("eighth");
    a_no_get_val("eighth");
    a_set("ninth", f8);
    a_set("tenth", f9);
    a_is_val(f1);
    a_not_val(f12);
    a_get_val("third", f2);
    a_no_get_val("eleventh");
    a_not_key("eleventh");
    a_set("eleventh", f10);
    a_set("twelfth", f11);
    a_set("thirteenth", f12);
    a_get_val("thirteenth", f12);
    a_remove("thirteenth");
    a_set("fourteenth", f13);
    a_set("fifteenth", f14);
    a_not_key("thirteenth");
    a_no_get_val("thirteenth");
    a_is_key("fifth");
    a_not_key("bar");
    a_is_key("tenth");
    a_set("sixteenth", f15);
    a_set("seventeenth", f16);
    a_set("eighteenth", f17);
    a_set("nineteenth", f18);
    a_set("twentieth", f19);
    a_is_val(f1);
    a_is_val(f13);
    mm_free(&mm);
    
    return (struct TestResult) { .successes = successes, .total = total };
}
