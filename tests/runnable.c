#include "../test.h"
#include "../runnable.h"

ERROR test1(struct DataStack * ds, struct TokenStack *ts) {
    printf("top of ds is literal)
}

struct TestResult test_runnable() {
    size_t successes = 0, total = 0;
    struct DataStack test_ds;
    tassert(dst_new(&test_ds) == NO_ERROR, "failed to init data stack");
    struct Tokenizer test_ts_tknr;
    tassert(tknr_from_string("foo bar baz", "hi", &test_ts_tknr) == NO_ERROR,
            "failed to init ts's tknr");
    struct TokenStack test_ts;
    tassert(tst_new(test_ts_tknr, &test_ts), "failed to init token stack");
    
    struct Runnable c_testing;
    tassert(rn_from_c())
    
    tassert(false, "no Runnable tests yet");
    return (struct TestResult) { .successes = successes, .total = total };
}