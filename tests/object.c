#include <string.h>
#include "../tests.h"

#include "../object.h"

static size_t successes = 0, total = 0;
void assert_eq(const char *restrict a, const char *restrict b, const char *restrict from) {
    tassert(a, "LHS is null");
    tassert(b, "RHS is null");
    if (!a || !b) return;
    tassert(strcmp(a, b) == 0, "%s wrong value", from);
}

struct ModuleTestResult test_object() {
    long val = 12;
    struct Object t1 = ctno_literal(&val, sizeof(long), TID_long, NULL);
    const char *name = "Foobar";
    struct Object t2 = ctno_literal(name, 7, TID_char, NULL);
    struct Object t3 = ctno_dynamic(pm_new(8), NULL);
    ctno_set_prop(&t3, "1", &t1);
    ctno_set_prop(&t3, "2", &t2);
    tassert(*ctno_to(t1, long) == val, "t1->long bad value");
    assert_eq(ctno_to(t2, char), name, "t2->char*");
    struct Object t1g1 = *ctno_get_prop(t3, "1");
    struct Object t2g1 = *ctno_get_prop(t3, "2");
    tassert(*ctno_to(t1g1, long) == val, "t1g1->long bad value");
    assert_eq(ctno_to(t2g1, char), name, "t2g1->char*");
    ctno_free(&t1);
    ctno_free(&t2);
    // they should still exist, because t3 has a reference to them!
    tassert(*ctno_to(t1, long) == val, "t1->long 2 bad value");
    assert_eq(ctno_to(t2, char), name, "t2->char* 2");
    struct Object t1g2 = *ctno_get_prop(t3, "1");
    struct Object t2g2 = *ctno_get_prop(t3, "2");
    tassert(*ctno_to(t1g2, long) == val, "t1g2->long bad value");
    assert_eq(ctno_to(t2g2, char), name, "t2g2->char*");
    tassert(!ctno_set_prop(&t3, "self", &t3), "direct cycles allowed");
    struct Object t4 = ctno_dynamic(pm_new(8), NULL);
    ctno_set_prop(&t4, "self2", &t3);
    tassert(!ctno_set_prop(&t3, "self", &t4), "indirect cycles allowed");
    ctno_free(&t4);
    ctno_free(&t3);
    // and _now_ they should be gone.
    
    return (struct ModuleTestResult) { .successes = successes, .total = total };
}
