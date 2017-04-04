#include <string.h>
#include "../test.h"

#include "../object.h"

static size_t successes = 0, total = 0;
void assert_eq(const char *restrict a, const char *restrict b,
               const char *restrict from) {
    tassert(a, "LHS is null");
    tassert(b, "RHS is null");
    if (!a || !b) return;
    tassert(strcmp(a, b) == 0, "%s wrong value", from);
}

struct TestResult test_object() {
    ERROR prev_err;
    integral val = 12;
//    struct Object t1, t2, t3, t4, t5, t6, t3g1, t3g2, t3g3, t3g4, t5g1;
//    struct PropMap t1pm, t4pm;
    struct Object t1;
    tassert(!NOS_FAILED(
            ctno_literal(&val, sizeof(val), LTL_integral, NULL, &t1)),
            "failed to initialize t1");
    const char *name = "Foobar";
    struct Object t2;
    tassert(!NOS_FAILED(ctno_literal(name, 7, LTL_string, NULL, &t2)),
            "failed to initialize t2");
    struct PropMap t1pm;
    tassert(!NOS_FAILED(pm_new(8, &t1pm)), "failed to create propmap");
    struct Object t3;
    tassert(!NOS_FAILED(ctno_dynamic(t1pm, NULL, &t3)),
            "failed to initialize t3");
    pm_free(&t1pm);
    tassert(!NOS_FAILED(ctno_set_prop(&t3, "1", &t1)), "failed to set val");
    tassert(!NOS_FAILED(ctno_set_prop(&t3, "2", &t2)), "failed to set val");
    tassert(*ctno_to(t1, integral) == val, "t1->long bad value");
    assert_eq(ctno_to(t2, char), name, "t2->char*");
    struct Object t3g1, t3g2;
    tassert(!NOS_FAILED(ctno_get_prop(t3, "1", &t3g1)), "failed to get val");
    tassert(!NOS_FAILED(ctno_get_prop(t3, "2", &t3g2)), "failed to get val");
    tassert(*ctno_to(t3g1, integral) == val, "t3g1->long bad value");
    tassert(ctno_eq(t3g1, t1), "got unequal values");
    assert_eq(ctno_to(t3g2, char), name, "t3g2->char*");
    tassert(ctno_eq(t3g2, t2), "got unequal values");
    tassert(!ctno_eq(t1, t3g2), "unequal values compared equal");
    tassert(!ctno_eq(t3, t1), "unequal values compared equal");
    ctno_free(&t1);
    ctno_free(&t2);
    // they should still exist, because t3 has a reference to them!
    tassert(*ctno_to(t1, integral) == val, "t1->long 2 bad value");
    assert_eq(ctno_to(t2, char), name, "t2->char* 2");
    struct Object t3g3, t3g4;
    tassert(!NOS_FAILED(ctno_get_prop(t3, "1", &t3g3)), "failed to get val");
    tassert(!NOS_FAILED(ctno_get_prop(t3, "2", &t3g4)), "failed to get val");
    tassert(*ctno_to(t3g3, integral) == val, "t3g3->long bad value");
    assert_eq(ctno_to(t3g4, char), name, "t3g4->char*");
    tassert(FAILED(ctno_set_prop(&t3, "self", &t3)),
            "direct cycles allowed");
    tassert(prev_err.errcode == CTNO_SET_CYCLE_FAIL, "wrong error code");
    struct PropMap t4pm;
    tassert(!NOS_FAILED(pm_new(8, &t4pm)), "failed to initialize propmap");
    struct Object t4;
    tassert(!NOS_FAILED(ctno_dynamic(t4pm, NULL, &t4)),
            "failed to initialize t4");
    pm_free(&t4pm);
    tassert(!NOS_FAILED(ctno_set_prop(&t4, "self2", &t3)),
            "failed intermediary add");
    tassert(FAILED(ctno_set_prop(&t3, "self", &t4)),
            "indirect cycles allowed");
    tassert(prev_err.errcode == CTNO_SET_CYCLE_FAIL, "wrong error code");
    ctno_free(&t4);
    struct Object t5;
    tassert(!NOS_FAILED(ctno_copy(t3, &t5)), "failed to copy");
    tassert(t3.data.properties.buckets != t5.data.properties.buckets,
            "reference copy, not real copy");
    tassert(ctno_eq(t3, t5), "copy produced unequal objects");
    struct Object t5g1;
    tassert(!NOS_FAILED(ctno_get_prop(t5, "1", &t5g1)), "failed to get");
    tassert(t5g1.data.literal.value == t1.data.literal.value,
            "failed to get identical object");
    ctno_free(&t5);
    struct Object t6;
    tassert(!NOS_FAILED(ctno_copy(t1, &t6)), "failed to copy");
    tassert(t1.data.literal.value != t6.data.literal.value,
            "reference copy");
    tassert(ctno_eq(t1, t6), "got inequal objects");
    tassert(*ctno_to(t1, integral) == *ctno_to(t6, integral), "unequal val");
    ctno_free(&t6);
    ctno_free(&t3);
    // and _now_ t1/t2 should be gone.
    
    return (struct TestResult) { .successes = successes, .total = total };
}
