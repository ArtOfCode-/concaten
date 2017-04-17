#include "../test.h"

#include "../data_stack.h"
#include "../object.h"

#define assert_pop_eq(num, what) \
    tassert(dst_pop(&dst##num, &dst##num##_got) == NO_ERROR, \
            "failed to pop"); \
    tassert(dst##num##_got == what, \
            str(__LINE__) ": " str(num) " got bad result");

#define assert_no_pop(num) \
    tassert(dst_pop(&dst##num, NULL) == DST_POP_EMPTY_FAIL, \
            str(__LINE__) ": " str(num) " got bad result")

struct TestResult test_data_stack() {
    size_t successes = 0, total = 0;
    struct DataStack dst1;
    struct Object *dst2_got;
    struct Object *dst1_got;
    tassert(dst_new(&dst1) == NO_ERROR, "failed to init datastack");
    tassert(dst_empty(dst1), "non-empty initially");
    integral val = 12;
    struct Object *foo = malloc(sizeof(*foo));
    tassert(foo, "failed to malloc space for object");
    tassert(ctno_literal(&val, sizeof(val), LTL_integral, NULL, foo) ==
                    NO_ERROR, "failed to initialize foo");
    ++val;
    struct Object *bar = malloc(sizeof(*bar));
    tassert(bar, "failed to malloc space for object");
    tassert(ctno_literal(&val, sizeof(val), LTL_integral, NULL, bar) ==
                    NO_ERROR, "failed to initialize bar");
    ++val;
    struct Object *baz = malloc(sizeof(*baz));
    tassert(baz, "failed to malloc space for object");
    tassert(ctno_literal(&val, sizeof(val), LTL_integral, NULL, baz) ==
                    NO_ERROR, "failed to initialize baz");
    ++val;
    struct Object *wuf = malloc(sizeof(*wuf));
    tassert(wuf, "failed to malloc space for object");
    tassert(ctno_literal(&val, sizeof(val), LTL_integral, NULL, wuf) ==
                    NO_ERROR, "failed to initialize wuf");
    ++val;
    tassert(dst_push(&dst1, foo) == NO_ERROR, "failed to push");
    tassert(dst_push(&dst1, bar) == NO_ERROR, "failed to push");
    tassert(!dst_empty(dst1), "empty after adding");
    tassert(dst_push(&dst1, baz) == NO_ERROR, "failed to push");
    tassert(dst_push(&dst1, wuf) == NO_ERROR, "failed to push");
    tassert(dst_push(&dst1, baz) == NO_ERROR, "failed to push");
    assert_pop_eq(1, baz);
    struct DataStack dst2;
    tassert(dst_copy(dst1, &dst2) == NO_ERROR, "failed to copy");
    tassert(!dst_empty(dst1), "empty after copying");
    assert_pop_eq(2, wuf);
    assert_pop_eq(2, baz);
    assert_pop_eq(2, bar);
    tassert(dst_push(&dst2, wuf) == NO_ERROR, "failed to push");
    tassert(dst_push(&dst2, baz) == NO_ERROR, "failed to push");
    assert_pop_eq(2, baz);
    tassert(dst_push(&dst1, foo) == NO_ERROR, "failed to push");
    assert_pop_eq(2, wuf);
    tassert(dst_push(&dst1, bar) == NO_ERROR, "failed to push");
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
    ctno_free(foo);
    ctno_free(bar);
    ctno_free(baz);
    ctno_free(wuf);
    ctno_free(dst2_got);
    ctno_free(dst1_got);
    
    return (struct TestResult) { .successes = successes, .total = total };
}
