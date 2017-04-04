#include "../test.h"

#include "../code_block.h"

#define TOKEN ((struct Token) { \
    .raw = "hi!", \
    .raw_len = 4, \
    .origin = "magic", \
    .origin_len = 6, \
    .line = __LINE__, \
    .index = ++idx, \
    .type = TKN_WORD \
})

#define a_get(w, idx, l, i) \
    tassert(!NOS_FAILED(cb_get(test_##w, idx, &res)), "failed to get"); \
    tassert(res.line == l && res.index == i, "got wrong value (%zu %zu)", \
            res.line, res.index)

struct TestResult test_code_block() {
    ERROR prev_err;
    size_t successes = 0, total = 0, idx = 0;
    struct CodeBlock test_o;
    struct Token res;
    // make sure init works
    tassert(!NOS_FAILED(cb_new(4, &test_o)), "failed to initialize codeblock");
    tassert(test_o.count == 0, "bad initial count");
    // test_o basic appending
    tassert(!NOS_FAILED(cb_append(&test_o, TOKEN)), "failed to append");
    // test_o basic get
    a_get(o, 0, 29, 1);
    // get to the point where we resize
    tassert(!NOS_FAILED(cb_append(&test_o, TOKEN)), "failed to append");
    tassert(!NOS_FAILED(cb_append(&test_o, TOKEN)), "failed to append");
    tassert(!NOS_FAILED(cb_append(&test_o, TOKEN)), "failed to append");
    // make sure things are set up right
    tassert(test_o.count == 4, "wrong count (%zu)", test_o.count);
    tassert(test_o.cap == 4, "resized early");
    // resize!
    tassert(!NOS_FAILED(cb_append(&test_o, TOKEN)), "failed to append");
    tassert(test_o.cap == 8, "failed to resize");
    tassert(test_o.count == 5, "wrong count (%zu)", test_o.count);
    // remove from middle
    tassert(!NOS_FAILED(cb_remove(&test_o, 1)), "failed to remove");
    tassert(test_o.count == 4, "wrong count (%zu)", test_o.count);
    // remove from end
    tassert(!NOS_FAILED(cb_remove(&test_o, 3)), "failed to remove");
    tassert(test_o.count == 3, "wrong count (%zu)", test_o.count);
    // bad remove
    tassert(FAILED(cb_remove(&test_o, 9)),
            "removed from bad position");
    tassert(prev_err.errcode == CB_RMV_BAD_IDX_FAIL, "wrong error code");
    // get again
    a_get(o, 0, 29, 1);
    // testing copy
    struct CodeBlock test_c;
    tassert(!NOS_FAILED(cb_copy(test_o, &test_c)), "failed to copy");
    a_get(c, 0, 29, 1);
    tassert(!NOS_FAILED(cb_prepend(&test_c, TOKEN)), "failed to prepend");
    a_get(c, 0, 59, 6);
    // make sure the copy doesn't affect the original
    a_get(o, 0, 29, 1);
    cb_free(&test_c);
    // bad get
    tassert(FAILED(cb_get(test_o, 7, &res)),
            "got value that shouldn't be there");
    tassert(prev_err.errcode == CB_GET_BAD_IDX_FAIL, "wrong error code");
    // insert in middle
    tassert(!NOS_FAILED(cb_insert(&test_o, 2, TOKEN)), "failed to insert");
    a_get(o, 2, 69, 7);
    // bad insert
    tassert(FAILED(cb_insert(&test_o, 9, TOKEN)),
            "successfully inserted in bad pos");
    tassert(prev_err.errcode == CB_INST_BAD_IDX_FAIL, "wrong error code");
    // basic prepend
    tassert(!NOS_FAILED(cb_prepend(&test_o, TOKEN)), "failed to insert");
    a_get(o, 0, 76, 9);
    // set
    tassert(!NOS_FAILED(cb_set(&test_o, 1, TOKEN)), "failed to set");
    // suddenly, insert! (intentionally in the middle of the `set` test_o)
    tassert(!NOS_FAILED(cb_insert(&test_o, 3, TOKEN)), "failed to insert");
    a_get(o, 3, 81, 11);
    // finish the set test_o
    a_get(o, 1, 79, 10);
    cb_free(&test_o);
    
    return (struct TestResult) { .successes = successes, .total = total };
}
