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

void test_code_block() {
    size_t successes = 0, total = 0, idx = 0;
    struct CodeBlock test;
    struct Token res;
    tassert(cb_new(4, &test), "failed to initialize codeblock");
    tassert(test.count == 0, "bad initial count");
    tassert(cb_append(&test, TOKEN), "failed to append");
    tassert(cb_get(test, 0, &res), "failed to get");
    tassert(res.line == 21 && res.index == 1, "got incorrect value");
    tassert(test.count == 1, "wrong count (%zu)", test.count);
    tassert(cb_append(&test, TOKEN), "failed to append");
    tassert(test.count == 2, "wrong count (%zu)", test.count);
    tassert(cb_append(&test, TOKEN), "failed to append");
    tassert(cb_get(test, 0, &res), "failed to get");
    tassert(res.line == 21 && res.index == 1, "got incorrect value");
    tassert(test.cap == 4, "resized early");
    tassert(test.count == 3, "wrong count (%zu)", test.count);
    tassert(cb_append(&test, TOKEN), "failed to append");
    tassert(test.count == 4, "wrong count (%zu)", test.count);
    tassert(test.cap == 4, "resized early");
    tassert(cb_append(&test, TOKEN), "failed to append");
    tassert(test.cap == 8, "failed to resize");
    tassert(test.count == 5, "wrong count (%zu)", test.count);
    tassert(cb_remove(&test, 1), "failed to remove");
    tassert(test.count == 4, "wrong count (%zu)", test.count);
    tassert(test.cap == 8, "resized out of nowhere");
    tassert(cb_get(test, 0, &res), "failed to get");
    tassert(res.line == 21 && res.index == 1, "got incorrect value");
    tassert(!cb_get(test, 7, &res), "got value that shouldn't be there");
    tassert(cb_insert(&test, 0, TOKEN), "failed to insert");
    tassert(cb_get(test, 0, &res), "failed to get");
    tassert(res.line == 44 && res.index == 6, "got incorrect value");
    tassert(cb_prepend(&test, TOKEN), "failed to insert");
    tassert(cb_get(test, 0, &res), "failed to get");
    tassert(res.line == 47 && res.index == 7, "got incorrect value");
    tassert(cb_set(&test, 1, TOKEN), "failed to set");
    tassert(cb_get(test, 0, &res), "failed to get");
    tassert(res.line == 47 && res.index == 7, "got incorrect value");
    tassert(cb_get(test, 1, &res), "failed to get");
    tassert(res.line == 50 && res.index == 8, "got incorrect value");
    cb_free(&test);
    
    printf("%zu/%zu successes\n", successes, total);
}
