#include "../test.h"
#include "../runnable.h"
#include "../scope_stack.h"

#define rn_x \
    X(0) X(1) X(2) X(3) X(4) X(5) X(6) X(7) X(8) X(9)

#define X(n) \
    ERROR rn_f_##n(struct DataStack *ds, struct TokenStack *ts) { \
        (void)ds;(void)ts;return n; \
    }
rn_x
#undef X

struct TestResult test_scope_stack() {
    size_t successes = 0, total = 0;
#define X(n) \
    struct Runnable r##n; \
    tassert(rn_from_c(rn_f_##n, &r##n) == NO_ERROR, "failed to init r"str(n));
    rn_x
#undef X
    struct ScopeStack ss;
    struct Runnable out;
    tassert(ss_new(2, &ss) == NO_ERROR, "failed to init ss");
    tassert(ss_push_scope(&ss) == NO_ERROR, "failed to push scope");
    tassert(ss_pop_scope(&ss) == NO_ERROR, "failed to pop scope");
    tassert(ss_set(&ss, "r0", r0) == NO_ERROR, "failed to set r0");
    tassert(ss_set(&ss, "r1", r1) == NO_ERROR, "failed to set r1");
    tassert(ss_push_scope(&ss) == NO_ERROR, "failed to push scope");
    tassert(ss_set(&ss, "r2", r2) == NO_ERROR, "failed to set r2");
    tassert(ss_set(&ss, "r3", r3) == NO_ERROR, "failed to set r3");
    tassert(ss_get(ss, "r3", &out) == NO_ERROR, "failed to get r3");
    tassert(ss_push_scope(&ss) == NO_ERROR, "failed to push scope");
    tassert(ss_set(&ss, "r5", r5) == NO_ERROR, "failed to set r5");
    tassert(ss_get(ss, "r5", &out) == NO_ERROR, "failed to get r5");
    tassert(ss_get(ss, "r3", &out) == NO_ERROR, "failed to get r3");
    tassert(ss_set(&ss, "r6", r6) == NO_ERROR, "failed to set r6");
    tassert(ss_get(ss, "r5", &out) == NO_ERROR, "failed to get r5");
    tassert(rn_eq(out, r5), "failed to get correct val");
    tassert(ss_get(ss, "r6", &out) == NO_ERROR, "failed to get r6");
    tassert(ss_save_state(&ss) == NO_ERROR, "failed to save state");
    tassert(ss_pop_scope(&ss) == NO_ERROR, "failed to pop scope");
    tassert(ss_get(ss, "r3", &out) == NO_ERROR, "failed to get r3");
    tassert(ss_get(ss, "r5", &out) == SST_GET_NO_KEY_FAIL, "got r5");
    tassert(ss_get(ss, "r5", &out) == SST_GET_NO_KEY_FAIL, "got r6");
    tassert(ss_set(&ss, "r5", r7) == NO_ERROR, "failed to set r5 to r7");
    tassert(ss_get(ss, "r5", &out) == NO_ERROR, "failed to get r5");
    tassert(rn_eq(out, r7), "failed to get correct value");
    tassert(ss_restore_state(&ss) == NO_ERROR, "failed to restore state");
    tassert(ss_get(ss, "r4", &out) == NO_ERROR, "failed to get r4");
    tassert(rn_eq(r4, out), "failed to get correct val");
    tassert(ss_get(ss, "r5", &out) == NO_ERROR, "failed to get r5");
    tassert(rn_eq(out, r5), "failed to get correct val");
    tassert(ss_get(ss, "r6", &out) == NO_ERROR, "failed to get r6");
    tassert(rn_eq(out, r6), "failed to get correct val");
    

#define X(n) rn_free(&r##n);
    rn_x
#undef X
    
    return (struct TestResult) { .total = total, .successes = successes };
}
