#include "../test.h"

#include "../data_stack.h"
#include "../object.h"

struct DST_Node {
    // we want the contents of _this object_, not necessarily the ones it
    // points to, to be immutable, with the notable exception of the refcount.
    struct DST_Node *next;
    struct Object *value;
    size_t refcount;
};

#define assert_pop_eq(num, what) \
    tassert(!NOS_FAILED(dst_pop(&dst##num, &dst##num##_got)), \
            "failed to pop"); \
    tassert(dst##num##_got == &what, \
            str(__LINE__) ": " str(num) " got bad result")

#define assert_no_pop(num) \
    tassert(FAILED(dst_pop(&dst##num, NULL)), "got impossibly"); \
    tassert(prev_err.errcode == DST_POP_EMPTY_FAIL, "wrong error code");

struct TestResult test_data_stack() {
    ERROR prev_err;
    size_t successes = 0, total = 0;
    struct DataStack dst1;
    struct Object *dst2_got;
    struct Object *dst1_got;
    tassert(!NOS_FAILED(dst_new(&dst1)), "failed to init datastack");
    integral val = 12;
    struct Object foo, bar, baz, wuf;
    tassert(!NOS_FAILED(
            ctno_literal(&val, sizeof(val), LTL_integral, NULL, &foo)),
            "failed to initialize foo");
    ++val;
    tassert(!NOS_FAILED(
            ctno_literal(&val, sizeof(val), LTL_integral, NULL, &bar)),
            "failed to initialize bar");
    ++val;
    tassert(!NOS_FAILED(
            ctno_literal(&val, sizeof(val), LTL_integral, NULL, &baz)),
            "failed to initialize baz");
    ++val;
    tassert(!NOS_FAILED(
            ctno_literal(&val, sizeof(val), LTL_integral, NULL, &wuf)),
            "failed to initialize wuf");
    ++val;
    tassert(!NOS_FAILED(dst_push(&dst1, &foo)), "failed to push");
    tassert(!NOS_FAILED(dst_push(&dst1, &bar)), "failed to push");
    tassert(!NOS_FAILED(dst_push(&dst1, &baz)), "failed to push");
    tassert(!NOS_FAILED(dst_push(&dst1, &wuf)), "failed to push");
    tassert(!NOS_FAILED(dst_push(&dst1, &baz)), "failed to push");
    assert_pop_eq(1, baz);
    struct DataStack dst2;
    tassert(!NOS_FAILED(dst_copy(dst1, &dst2)), "failed to copy");
    assert_pop_eq(2, wuf);
    assert_pop_eq(2, baz);
    assert_pop_eq(2, bar);
    tassert(!NOS_FAILED(dst_push(&dst2, &wuf)), "failed to push");
    tassert(!NOS_FAILED(dst_push(&dst2, &baz)), "failed to push");
    assert_pop_eq(2, baz);
    tassert(!NOS_FAILED(dst_push(&dst1, &foo)), "failed to push");
    assert_pop_eq(2, wuf);
    tassert(!NOS_FAILED(dst_push(&dst1, &bar)), "failed to push");
    assert_pop_eq(2, foo);
    assert_no_pop(2);
    assert_pop_eq(1, bar);
    assert_pop_eq(1, foo);
    assert_pop_eq(1, wuf);
    assert_pop_eq(1, baz);
    assert_pop_eq(1, bar);
    assert_pop_eq(1, foo);
    assert_no_pop(1);
    dst_free(&dst1);
    dst_free(&dst2);
    ctno_free(&foo);
    ctno_free(&bar);
    ctno_free(&baz);
    ctno_free(&wuf);
    
    return (struct TestResult) { .successes = successes, .total = total };
}
