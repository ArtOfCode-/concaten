#include "../test.h"

#include "../type.h"

#define create(n) \
    struct Type t##n; \
    size_t t##n##_c = sizeof(t##n##_s) / sizeof(*t##n##_s); \
    tassert(tp_new(t##n##_s, t##n##_c, &t##n) == NO_ERROR, "tp_new"); \
    struct MethodMap m##n; \
    tassert(mm_new(8, &m##n) == NO_ERROR, "mm_new"); \
    for (size_t i = 0; i < t##n##_c; ++i) \
        tassert(mm_set(&m##n, t##n##_s[i], testrn) == NO_ERROR, "mm_set"); \
    struct Object *o##n = malloc(sizeof(*o##n)); \
    tassert(o##n, "failed to allocate space for %zu", n); \
    tassert(ctno_literal(&r, sizeof(integral), LTL_integral, &m##n, &o##n) \
            == NO_ERROR, \
        "ctno_new");

struct TestResult test_type() {
    size_t total = 0, successes = 0;
    integral r = 1;
    struct Runnable testrn;
    tassert(rn_from_c(NULL, &testrn) == NO_ERROR, "failed rn_new");
    
    char *t1_s[] = {
            "add", "sub", "mult", "div"
    };
    char *t2_s[] = {
            "+"
    };
    create(1);
    create(2);
    struct Type t2c;
    tassert(tp_copy(t2, &t2c) == NO_ERROR, "failed to copy");
    tassert(tp_eq(t2, t2c), "expected equal values");
    struct Type t3 = MATCH_ALL;
    tassert(!tp_eq(t2, t3), "expected unequal values");
    tassert(!tp_eq(t2, t1), "expected unequal values");
    
    tassert(tp_matches(t1, o1), "should match but didn't");
    tassert(!tp_matches(t1, o2), "shouldn't match but did");
    tassert(!tp_matches(t2, o1), "shouldn't match but did");
    tassert(tp_matches(t2, o2), "should match but didn't");
    tassert(!tp_matches(t2c, o1), "shouldn't match but did");
    tassert(tp_matches(t2c, o2), "should match but didn't");
    tassert(tp_matches(t3, o1), "should match but didn't");
    tassert(tp_matches(t3, o2), "should match but didn't");
    
    tp_free(&t1);
    tp_free(&t2);
    tp_free(&t3);
    
    return (struct TestResult) { .total = total, .successes = successes };
}
