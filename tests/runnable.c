#include "../test.h"
#include "../runnable.h"
#include "../object.h"

static size_t successes, total;

static bool top_is_lit;
static enum TokenType top_type;

ERROR test1(struct DataStack *ds, struct TokenStack *ts) {
    struct Object *tods;
    tassert(dst_peek(ds, &tods) == NO_ERROR, "failed to peek ds");
    tassert(tods->is_literal == top_is_lit, "got incorrect value for ds");
    
    struct Token tots;
    tassert(tst_peek(ts, &tots) == NO_ERROR, "failed to peek ts");
    tassert(tots.type == top_type, "got incorrect value for ts");
    return NO_ERROR;
}

ERROR test2(struct DataStack *ds, struct TokenStack *ts) {
    struct Object *tods;
    tassert(dst_pop(ds, &tods) == NO_ERROR, "failed to pop ds");
    tassert(tods->is_literal == top_is_lit, "got wrong value for ds");
    
    struct Token tots;
    tassert(tst_pop(ts, &tots) == NO_ERROR, "failed to pop ts");
    tassert(tots.type == top_type, "got wrong value for ts");
    return USER_DEFINED_ERROR;
}

struct TestResult test_runnable() {
    successes = 0;
    total = 0;
    struct DataStack test_ds;
    tassert(dst_new(&test_ds) == NO_ERROR, "failed to init data stack");
    struct Object lit;
    integral v = 12;
    tassert(ctno_literal(&v, sizeof(v), LTL_integral, NULL, &lit) == NO_ERROR,
            "failed to init literal object");
    struct Object dyn;
    struct PropMap pm = (struct PropMap) { 0 };
    tassert(ctno_dynamic(pm, NULL, &dyn) == NO_ERROR,
            "failed to init dynamic object");
    tassert(dst_push(&test_ds, &dyn) == NO_ERROR, "failed to push to ds");
    tassert(dst_push(&test_ds, &lit) == NO_ERROR, "failed to push to ds");
    struct Tokenizer test_ts_tknr;
    tassert(tknr_from_string("foo :bar 0xbad", "hi", &test_ts_tknr) == NO_ERROR,
            "failed to init ts's tknr");
    struct TokenStack test_ts;
    tassert(tst_new(test_ts_tknr, &test_ts) == NO_ERROR,
            "failed to init token stack");
    
    struct Runnable c_testing;
    tassert(rn_from_c(test1, &c_testing) == NO_ERROR, "failed to init c test");
    top_is_lit = true;
    top_type = TKN_WORD;
    tassert(rn_run(c_testing, &test_ds, &test_ts) == NO_ERROR,
            "failed to run correctly");
    
    tassert(false, "no Runnable tests yet");
    return (struct TestResult) { .successes = successes, .total = total };
}