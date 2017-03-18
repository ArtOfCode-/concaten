#include "../tests.h"

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

#define a_append(what) \
    tassert(cb_append(&test, what) == NO_ERROR, "failed to append")

#define a_get(idx, l, i) \
    tassert(cb_get(test, idx, &res) == NO_ERROR, "failed to get"); \
    tassert(res.line == l && res.index == i, "got wrong value (%zu %zu)", \
            res.line, res.index)

struct TestResult test_code_block() {
    size_t successes = 0, total = 0, idx = 0;
    struct CodeBlock test;
    struct Token res;
    // make sure init works
    tassert(cb_new(4, &test) == NO_ERROR, "failed to initialize codeblock");
    tassert(test.count == 0, "bad initial count");
    // test basic appending
    a_append(TOKEN);
    // test basic get
    a_get(0, 31, 1);
    // get to the point where we resize
    a_append(TOKEN);
    a_append(TOKEN);
    a_append(TOKEN);
    // make sure things are set up right
    tassert(test.count == 4, "wrong count (%zu)", test.count);
    tassert(test.cap == 4, "resized early");
    // resize!
    a_append(TOKEN);
    tassert(test.cap == 8, "failed to resize");
    tassert(test.count == 5, "wrong count (%zu)", test.count);
    // remove from middle
    tassert(cb_remove(&test, 1) == NO_ERROR, "failed to remove");
    tassert(test.count == 4, "wrong count (%zu)", test.count);
    // remove from end
    tassert(cb_remove(&test, 3) == NO_ERROR, "failed to remove");
    tassert(test.count == 3, "wrong count (%zu)", test.count);
    // bad remove
    tassert(cb_remove(&test, 9) == CB_RMV_BAD_IDX_FAIL,
            "removed from bad position");
    // get again
    a_get(0, 31, 1);
    // bad get
    tassert(cb_get(test, 7, &res) == CB_GET_BAD_IDX_FAIL,
            "got value that shouldn't be there");
    // insert in middle
    tassert(cb_insert(&test, 2, TOKEN) == NO_ERROR, "failed to insert");
    a_get(2, 60, 6);
    // bad insert
    tassert(cb_insert(&test, 9, TOKEN) == CB_INST_BAD_IDX_FAIL,
            "successfully inserted in bad pos");
    // basic prepend
    tassert(cb_prepend(&test, TOKEN) == NO_ERROR, "failed to insert");
    a_get(0, 66, 8);
    // set
    tassert(cb_set(&test, 1, TOKEN) == NO_ERROR, "failed to set");
    // suddenly, insert! (intentionally in the middle of the `set` test)
    tassert(cb_insert(&test, 3, TOKEN) == NO_ERROR, "failed to insert");
    a_get(3, 71, 10);
    // finish the set test
    a_get(1, 69, 9);
    cb_free(&test);
    
    return (struct TestResult) { .successes = successes, .total = total };
}
