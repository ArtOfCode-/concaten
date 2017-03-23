#include <string.h>
#include "../test.h"

#include "../object.h"

static size_t successes = 0, total = 0;
void assert_eq(const char *restrict a, const char *restrict b, const char *restrict from) {
    tassert(a, "LHS is null");
    tassert(b, "RHS is null");
    if (!a || !b) return;
    tassert(strcmp(a, b) == 0, "%s wrong value", from);
}

struct TestResult test_object() {
    long val = 12;
    struct Object t1;
    tassert(ctno_literal(&val, sizeof(long), NULL, &t1) == NO_ERROR,
            "failed to initialize t1");
    const char *name = "Foobar";
    struct Object t2;
    tassert(ctno_literal(name, 7, NULL, &t2) == NO_ERROR,
            "failed to initialize t2");
    struct Object t3;
    struct PropMap t1pm;
    tassert(pm_new(8, &t1pm) == NO_ERROR, "failed to create propmap");
    tassert(ctno_dynamic(t1pm, NULL, &t3) == NO_ERROR,
            "failed to initialize t3");
    tassert(ctno_set_prop(&t3, "1", &t1) == NO_ERROR, "failed to set val");
    tassert(ctno_set_prop(&t3, "2", &t2) == NO_ERROR, "failed to set val");
    tassert(*ctno_to(t1, long) == val, "t1->long bad value");
    assert_eq(ctno_to(t2, char), name, "t2->char*");
    struct Object t1g1;
    tassert(ctno_get_prop(t3, "1", &t1g1) == NO_ERROR, "failed to get val");
    struct Object t2g1;
    tassert(ctno_get_prop(t3, "2", &t2g1) == NO_ERROR, "failed to get val");
    tassert(*ctno_to(t1g1, long) == val, "t1g1->long bad value");
    assert_eq(ctno_to(t2g1, char), name, "t2g1->char*");
    ctno_free(&t1);
    ctno_free(&t2);
    // they should still exist, because t3 has a reference to them!
    tassert(*ctno_to(t1, long) == val, "t1->long 2 bad value");
    assert_eq(ctno_to(t2, char), name, "t2->char* 2");
    struct Object t1g2;
    tassert(ctno_get_prop(t3, "1", &t1g2) == NO_ERROR, "failed to get val");
    struct Object t2g2;
    tassert(ctno_get_prop(t3, "2", &t2g2) == NO_ERROR, "failed to get val");
    tassert(*ctno_to(t1g2, long) == val, "t1g2->long bad value");
    assert_eq(ctno_to(t2g2, char), name, "t2g2->char*");
    tassert(ctno_set_prop(&t3, "self", &t3) == CTNO_SET_CYCLE_FAIL,
            "direct cycles allowed");
    struct Object t4;
    struct PropMap t4pm;
    tassert(pm_new(8, &t4pm) == NO_ERROR, "failed to initialize propmap");
    tassert(ctno_dynamic(t4pm, NULL, &t4) == NO_ERROR,
            "failed to initialize t4");
    tassert(ctno_set_prop(&t4, "self2", &t3) == NO_ERROR,
            "failed intermediary add");
    tassert(ctno_set_prop(&t3, "self", &t4) == CTNO_SET_CYCLE_FAIL,
            "indirect cycles allowed");
    ctno_free(&t4);
    ctno_free(&t3);
    // and _now_ they should be gone.
    
    return (struct TestResult) { .successes = successes, .total = total };
}
