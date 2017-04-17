#include "../test.h"
#include "../runnable.h"
#include "../scope_stack.h"

#define rn_x \
    X(0) X(1) X(2) X(3) X(4) X(5) X(6) X(7) X(8) X(9)

#define X(n) \
    ERROR rn_f_##n(struct DataStack *ds, struct ScopeStack *ss, \
                   struct TokenStack *ts) { \
        (void)ds;(void)ts;(void)ss;return n; \
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
    struct MethodMap globals;
    tassert(mm_new(1, &globals) == NO_ERROR, "failed to init fake mm");
    tassert(sst_new(2, globals, &ss) == NO_ERROR, "failed to init ss");
    tassert(sst_push_scope(&ss) == NO_ERROR, "failed to push scope");
    tassert(sst_pop_scope(&ss) == NO_ERROR, "failed to pop scope");
    tassert(sst_set(&ss, "r0", r0) == NO_ERROR, "failed to set r0");
    tassert(sst_set(&ss, "r1", r1) == NO_ERROR, "failed to set r1");
    tassert(sst_push_scope(&ss) == NO_ERROR, "failed to push scope");
    tassert(sst_set(&ss, "r2", r2) == NO_ERROR, "failed to set r2");
    tassert(sst_set(&ss, "r3", r3) == NO_ERROR, "failed to set r3");
    tassert(sst_get(ss, "r3", &out) == NO_ERROR, "failed to get r3");
    tassert(sst_push_scope(&ss) == NO_ERROR, "failed to push scope");
    tassert(sst_set(&ss, "r4", r4) == NO_ERROR, "failed to set r4");
    tassert(sst_get(ss, "r4", &out) == NO_ERROR, "failed to get r4");
    tassert(sst_get(ss, "r3", &out) == NO_ERROR, "failed to get r3");
    tassert(sst_set(&ss, "r5", r5) == NO_ERROR, "failed to set r5");
    tassert(sst_get(ss, "r4", &out) == NO_ERROR, "failed to get r4");
    tassert(rn_eq(out, r4), "failed to get correct val");
    tassert(sst_get(ss, "r5", &out) == NO_ERROR, "failed to get r5");
    tassert(sst_save_state(&ss) == NO_ERROR, "failed to save state");
    tassert(sst_pop_scope(&ss) == NO_ERROR, "failed to pop scope");
    tassert(sst_get(ss, "r3", &out) == NO_ERROR, "failed to get r3");
    tassert(sst_get(ss, "r4", &out) == SST_GET_NO_KEY_FAIL, "got r4");
    tassert(sst_get(ss, "r5", &out) == SST_GET_NO_KEY_FAIL, "got r5");
    tassert(sst_set(&ss, "r4", r6) == NO_ERROR, "failed to set r4 to r6");
    tassert(sst_get(ss, "r4", &out) == NO_ERROR, "failed to get r4");
    tassert(rn_eq(out, r6), "failed to get correct value");
    tassert(sst_restore_state(&ss) == NO_ERROR, "failed to restore state");
    tassert(sst_get(ss, "r3", &out) == NO_ERROR, "failed to get r4");
    tassert(rn_eq(r3, out), "failed to get correct val");
    tassert(sst_get(ss, "r4", &out) == NO_ERROR, "failed to get r4");
    tassert(rn_eq(out, r4), "failed to get correct val");
    tassert(sst_get(ss, "r5", &out) == NO_ERROR, "failed to get r5");
    tassert(rn_eq(out, r5), "failed to get correct val");
    tassert(sst_pop_scope(&ss) == NO_ERROR, "failed to pop scope");
    tassert(sst_get(ss, "r3", &out) == NO_ERROR, "failed to get r3");
    tassert(sst_get(ss, "r4", &out) == SST_GET_NO_KEY_FAIL, "got r4");
    tassert(sst_get(ss, "r5", &out) == SST_GET_NO_KEY_FAIL, "got r5");
    // current setup: r0 r1 / r2 r3
    tassert(sst_push_scope(&ss) == NO_ERROR, "failed to push scope");
    tassert(sst_set(&ss, "r1", r4) == NO_ERROR, "failed to set overridden r1");
    tassert(sst_set(&ss, "r2", r5) == NO_ERROR, "failed to set overridden r2");
    tassert(sst_get(ss, "r1", &out) == NO_ERROR, "failed to get r1");
    tassert(rn_eq(out, r4), "got incorrect value for overridden r1");
    tassert(sst_get(ss, "r2", &out) == NO_ERROR, "failed to get r2");
    tassert(rn_eq(out, r5), "got incorrect value for overridden r2");
    size_t out_count;
    struct Runnable *out_ary;
    tassert(sst_get_all(ss, "r1", &out_ary, &out_count) == NO_ERROR,
            "failed to get_all for key r1");
    tassert(out_count == 2, "wrong count for out_ary");
    tassert(rn_eq(out_ary[0], r4), "got wrong value for first element");
    tassert(rn_eq(out_ary[1], r1), "got wrong value for second element");
    free(out_ary);
    tassert(sst_redefine(&ss, "r0", r6), "failed to redefine r0");
    tassert(sst_get_all(ss, "r0", &out_ary, &out_count) == NO_ERROR,
            "failed to get_all for key r0");
    tassert(out_count == 1, "wrong count for out_ary");
    tassert(rn_eq(out_ary[0], r6), "got wrong value for first element");
    free(out_ary);
    tassert(sst_get(ss, "r0", &out) == NO_ERROR, "failed to get r0");
    tassert(rn_eq(out, r6), "got wrong single value for r0");
    
#define X(n) rn_free(&r##n);
    rn_x
#undef X
    mm_free(&globals);
    sst_free(&ss);
    
    return (struct TestResult) { .total = total, .successes = successes };
}
