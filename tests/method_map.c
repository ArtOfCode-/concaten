#include "../test.h"

#include "../method_map.h"

#define test_x \
    X(00) X(01) \
    X(02) X(03) \
    X(04) X(05) \
    X(06) X(07) \
    X(08) X(09) \
    X(10) X(11) \
    X(12) X(13) \
    X(14) X(15) \
    X(16) X(17) \
    X(18) X(19)


#define X(num) \
    ERROR _f##num(struct DataStack *ds, struct ScopeStack *ss, \
                  struct TokenStack *ts) { \
        (void) ds; (void) ts; (void) ss; return NO_ERROR; \
    }
test_x
#undef X

struct TestResult test_method_map() {
    size_t successes = 0, total = 0;
    // very low number of buckets initially so it has to rehash
    // ...eventually. I hate the pigeonhole principle.
    struct MethodMap mm;
    MM_VALUE_TYPE val;
    tassert(mm_new(2, &mm) == NO_ERROR, "failed to initialize");
#define X(n) \
    struct Runnable f##n; \
    tassert(rn_from_c(_f##n, &f##n) == NO_ERROR, "failed to make f"str(n));
    test_x
#undef X
    tassert(mm_set(&mm, "first", f00) == NO_ERROR, "assignment failed");
    tassert(mm_set(&mm, "second", f01) == NO_ERROR, "assignment failed");
    tassert(mm_set(&mm, "third", f02) == NO_ERROR, "assignment failed");
    tassert(mm_set(&mm, "fourth", f03) == NO_ERROR, "assignment failed");
    tassert(mm_set(&mm, "fifth", f04) == NO_ERROR, "assignment failed");
    tassert(mm_is_key(mm, "fifth"), "fifth isn't a key!");
    tassert(!mm_is_key(mm, "bar"), "bar is a key!");
    tassert(!mm_is_key(mm, "tenth"), "tenth is a key!");
    tassert(mm_set(&mm, "sixth", f05) == NO_ERROR, "assignment failed");
    tassert(mm_set(&mm, "seventh", f06) == NO_ERROR, "assignment failed");
    tassert(mm_set(&mm, "eighth", f07) == NO_ERROR, "assignment failed");
    tassert(mm_is_key(mm, "eighth"), "eighth isn't a key!");
    tassert(mm_get(mm, "eighth", &val) == NO_ERROR, "eighth get fail");
    tassert(rn_eq(val, f07), "unexpected value for " "eighth");
    tassert(mm_remove(&mm, "eighth") == NO_ERROR, "failed to remove");
    tassert(!mm_is_key(mm, "eighth"), "eighth is a key!");
    tassert(mm_get(mm, "eighth", &val) != NO_ERROR, "got for eighth");
    tassert(mm_set(&mm, "ninth", f08) == NO_ERROR, "assignment failed");
    tassert(mm_set(&mm, "tenth", f09) == NO_ERROR, "assignment failed");
    tassert(mm_is_value(mm, f01), "f01 isn't a value!");
    tassert(!mm_is_value(mm, f12), "f12 is a value!");
    tassert(mm_get(mm, "third", &val) == NO_ERROR, "third get fail");
    tassert(rn_eq(val, f02), "unexpected value for " "third");
    tassert(mm_get(mm, "eleventh", &val) != NO_ERROR, "got for eleventh");
    tassert(!mm_is_key(mm, "eleventh"), "eleventh is a key!");
    tassert(mm_set(&mm, "eleventh", f10) == NO_ERROR, "assignment failed");
    tassert(mm_set(&mm, "twelfth", f11) == NO_ERROR, "assignment failed");
    tassert(mm_set(&mm, "thirteenth", f12) == NO_ERROR, "assignment failed");
    tassert(mm_get(mm, "thirteenth", &val) == NO_ERROR, "thirteenth get fail");
    tassert(rn_eq(val, f12), "unexpected value for " "thirteenth");
    tassert(mm_remove(&mm, "thirteenth") == NO_ERROR, "failed to remove");
    tassert(!mm_is_value(mm, f12), "f12 is a value!");
    tassert(mm_set(&mm, ">string", f13) == NO_ERROR, "assignment failed");
    tassert(mm_set(&mm, "fifteenth", f14) == NO_ERROR, "assignment failed");
    tassert(!mm_is_key(mm, "thirteenth"), "thirteenth is a key!");
    tassert(mm_get(mm, "thirteenth", &val) != NO_ERROR, "got for thirteenth");
    tassert(mm_is_key(mm, "fifth"), "fifth isn't a key!");
    tassert(!mm_is_key(mm, "bar"), "bar is a key!");
    tassert(mm_is_key(mm, "tenth"), "tenth isn't a key!");
    tassert(mm_set(&mm, "sixteenth", f15) == NO_ERROR, "assignment failed");
    tassert(mm_set(&mm, "seventeenth", f16) == NO_ERROR, "assignment failed");
    tassert(mm_set(&mm, "eighteenth", f17) == NO_ERROR, "assignment failed");
    tassert(mm_set(&mm, "nineteenth", f18) == NO_ERROR, "assignment failed");
    tassert(mm_set(&mm, "twentieth", f19) == NO_ERROR, "assignment failed");
    tassert(mm_is_value(mm, f01), "f01 isn't a value!");
    tassert(mm_is_value(mm, f13), "f13 isn't a value!");
    mm_free(&mm);
    
    return (struct TestResult) { .successes = successes, .total = total };
}
