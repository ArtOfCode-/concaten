#include "../tests.h"

#include "../data_stack.h"

#define LINE str(__LINE__) ": "
#define assert_pop_eq(num, what) \
    dst##num##_got = dst_pop(&dst##num); \
    tassert(dst##num##_got == &what, LINE str(num) " got bad result")
#define assert_no_pop(num) \
    dst##num##_got = dst_pop(&dst##num); \
    tassert(!dst##num##_got, LINE str(num) " got bad result")

struct TestResult test_data_stack() {
    struct DataStack dst1 = dst_new();
    long val = 12;
    size_t successes = 0, total = 0;
    struct Object foo = ctno_literal(&val, sizeof(long), TID_long, NULL); ++val;
    struct Object bar = ctno_literal(&val, sizeof(long), TID_long, NULL); ++val;
    struct Object baz = ctno_literal(&val, sizeof(long), TID_long, NULL); ++val;
    struct Object wuf = ctno_literal(&val, sizeof(long), TID_long, NULL); ++val;
    dst_push(&dst1, &foo);
    dst_push(&dst1, &bar);
    dst_push(&dst1, &baz);
    dst_push(&dst1, &wuf);
    struct DataStack dst2 = dst_copy(dst1);
    struct Object *dst2_got;
    assert_pop_eq(2, wuf);
    assert_pop_eq(2, baz);
    assert_pop_eq(2, bar);
    dst_push(&dst2, &wuf);
    dst_push(&dst2, &baz);
    assert_pop_eq(2, baz);
    dst_push(&dst1, &foo);
    assert_pop_eq(2, wuf);
    dst_push(&dst1, &bar);
    assert_pop_eq(2, foo);
    assert_no_pop(2);
    struct Object *dst1_got;
    assert_pop_eq(1, bar);
    assert_pop_eq(1, foo);
    assert_pop_eq(1, wuf);
    assert_pop_eq(1, baz);
    assert_pop_eq(1, bar);
    assert_pop_eq(1, foo);
    assert_no_pop(1);
    
    return (struct TestResult) { .successes = successes, .total = total };
}