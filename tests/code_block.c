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

struct ModuleTestResult test_code_block() {
    size_t successes = 0, total = 0, idx = 0;
    struct CodeBlock test;
    struct Token res;
    // make sure init works
    tassert(cb_new(4, &test), "failed to initialize codeblock");
    tassert(test.count == 0, "bad initial count");
    // test basic appending
    tassert(cb_append(&test, TOKEN), "failed to append");
    // test basic get
    tassert(cb_get(test, 0, &res), "failed to get");
    tassert(res.line == 23 && res.index == 1, "got incorrect value");
    // get to the point where we resize
    tassert(cb_append(&test, TOKEN), "failed to append");
    tassert(cb_append(&test, TOKEN), "failed to append");
    tassert(cb_append(&test, TOKEN), "failed to append");
    // make sure things are set up right
    tassert(test.count == 4, "wrong count (%zu)", test.count);
    tassert(test.cap == 4, "resized early");
    // resize!
    tassert(cb_append(&test, TOKEN), "failed to append");
    tassert(test.cap == 8, "failed to resize");
    tassert(test.count == 5, "wrong count (%zu)", test.count);
    // remove from middle
    tassert(cb_remove(&test, 1), "failed to remove");
    tassert(test.count == 4, "wrong count (%zu)", test.count);
    // remove from end
    tassert(cb_remove(&test, 3), "failed to remove");
    tassert(test.count == 3, "wrong count (%zu)", test.count);
    // bad remove
    tassert(!cb_remove(&test, 9), "removed from bad position");
    // get again
    tassert(cb_get(test, 0, &res), "failed to get");
    tassert(res.line == 23 && res.index == 1, "got incorrect value");
    // bad get
    tassert(!cb_get(test, 7, &res), "got value that shouldn't be there");
    // insert in middle
    tassert(cb_insert(&test, 2, TOKEN), "failed to insert");
    tassert(cb_get(test, 2, &res), "failed to get");
    tassert(res.line == 52 && res.index == 6, "got incorrect value");
    // bad insert
    tassert(!cb_insert(&test, 9, TOKEN), "successfully inserted in bad pos");
    // basic prepend
    tassert(cb_prepend(&test, TOKEN), "failed to insert");
    tassert(cb_get(test, 0, &res), "failed to get");
    tassert(res.line == 58 && res.index == 8, "got incorrect value");
    // set
    tassert(cb_set(&test, 1, TOKEN), "failed to set");
    // suddenly, insert! (intentionally in the middle of the `set` test)
    tassert(cb_insert(&test, 3, TOKEN), "failed to insert");
    tassert(cb_get(test, 3, &res), "failed to get");
    tassert(res.line == 64 && res.index == 10, "got incorrect value");
    // finish the set test
    tassert(cb_get(test, 1, &res), "failed to get");
    tassert(res.line == 62 && res.index == 9, "got incorrect value");
    cb_free(&test);
    
    return (struct ModuleTestResult) { .successes = successes, .total = total };
}
