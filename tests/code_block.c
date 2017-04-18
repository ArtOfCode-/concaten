#include "../test.h"
#include "../code_block.h"
#include <stdlib.h>
#include <string.h>

char *cb_gen_s() { char *r = malloc(1); *r = 0; return r; }
#define TOKEN ((struct Token) { \
    .raw = cb_gen_s(), \
    .raw_len = 1, \
    .origin = cb_gen_s(), \
    .origin_len = 1, \
    .line = __LINE__, \
    .index = ++idx, \
    .type = TKN_WORD \
})

#define a_get(w, idx, l, i) \
    tassert(cb_get(test_##w, idx, &res) == NO_ERROR, "failed to get"); \
    tassert(res.line == l && res.index == i, "got wrong value (%zu %zu)", \
            res.line, res.index)

struct TestResult test_code_block() {
    size_t successes = 0, total = 0, idx = 0;
    struct CodeBlock test_o;
    struct Token res;
    // make sure init works
    tassert(cb_new(4, &test_o) == NO_ERROR, "failed to initialize codeblock");
    tassert(test_o.count == 0, "bad initial count");
    // test_o basic appending
    tassert(cb_append(&test_o, TOKEN) == NO_ERROR, "failed to append");
    // test_o basic get
    a_get(o, 0, 30, 1);
    // get to the point where we resize
    tassert(cb_append(&test_o, TOKEN) == NO_ERROR, "failed to append");
    tassert(cb_append(&test_o, TOKEN) == NO_ERROR, "failed to append");
    tassert(cb_append(&test_o, TOKEN) == NO_ERROR, "failed to append");
    // make sure things are set up right
    tassert(test_o.count == 4, "wrong count (%zu)", test_o.count);
    tassert(test_o.cap == 4, "resized early");
    // resize!
    tassert(cb_append(&test_o, TOKEN) == NO_ERROR, "failed to append");
    tassert(test_o.cap == 8, "failed to resize");
    tassert(test_o.count == 5, "wrong count (%zu)", test_o.count);
    // remove from middle
    tassert(cb_remove(&test_o, 1) == NO_ERROR, "failed to remove");
    tassert(test_o.count == 4, "wrong count (%zu)", test_o.count);
    // remove from end
    tassert(cb_remove(&test_o, 3) == NO_ERROR, "failed to remove");
    tassert(test_o.count == 3, "wrong count (%zu)", test_o.count);
    // bad remove
    tassert(cb_remove(&test_o, 9) == CB_RMV_BAD_IDX_FAIL,
            "removed from bad position");
    // get again
    a_get(o, 0, 30, 1);
    // testing copy
    struct CodeBlock test_c;
    tassert(cb_copy(test_o, &test_c) == NO_ERROR, "failed to copy");
    a_get(c, 0, 30, 1);
    tassert(cb_prepend(&test_c, TOKEN) == NO_ERROR, "failed to prepend");
    a_get(c, 0, 59, 6);
    // make sure the copy doesn't affect the original
    a_get(o, 0, 30, 1);
    cb_free(&test_c);
    // bad get
    tassert(cb_get(test_o, 7, &res) == CB_GET_BAD_IDX_FAIL,
            "got value that shouldn't be there");
    // insert in middle
    tassert(cb_insert(&test_o, 2, TOKEN) == NO_ERROR, "failed to insert");
    a_get(o, 2, 68, 7);
    // bad insert
    tassert(cb_insert(&test_o, 9, TOKEN) == CB_INST_BAD_IDX_FAIL,
            "successfully inserted in bad pos");
    // basic prepend
    tassert(cb_prepend(&test_o, TOKEN) == NO_ERROR, "failed to insert");
    a_get(o, 0, 74, 9);
    // set
    tassert(cb_set(&test_o, 1, TOKEN) == NO_ERROR, "failed to set");
    // suddenly, insert! (intentionally in the middle of the `set` test_o)
    tassert(cb_insert(&test_o, 3, TOKEN) == NO_ERROR, "failed to insert");
    a_get(o, 3, 79, 11);
    // finish the set test_o
    a_get(o, 1, 77, 10);
    cb_free(&test_o);
    
    return (struct TestResult) { .successes = successes, .total = total };
}
