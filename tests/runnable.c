#include "../test.h"
#include "../runnable.h"
#include "../object.h"

static size_t successes, total;

static bool top_is_lit;
static enum TokenType top_type;

ERROR test1(struct DataStack *ds, struct ScopeStack *ss, struct TokenStack *ts) {
    (void) ss;
    struct Object *tods;
    tassert(dst_peek(ds, &tods) == NO_ERROR, "failed to peek ds");
    tassert(tods->is_literal == top_is_lit, "got incorrect value for ds");
    
    struct Token tots;
    tassert(tst_peek(ts, &tots) == NO_ERROR, "failed to peek ts");
    tassert(tots.type == top_type, "got incorrect value for ts");
    return NO_ERROR;
}

ERROR test2(struct DataStack *ds, struct ScopeStack *ss, struct TokenStack *ts) {
    (void) ss;
    struct Object *tods;
    tassert(dst_pop(ds, &tods) == NO_ERROR, "failed to pop ds");
    tassert(tods->is_literal == top_is_lit, "got wrong value for ds");
    
    struct Token tots;
    tassert(tst_pop(ts, &tots) == NO_ERROR, "failed to pop ts");
    tkn_free(&tots);
    tassert(tots.type == top_type, "got wrong value for ts");
    return USER_DEFINED_ERROR;
}

struct TestResult test_runnable() {
    successes = 0;
    total = 0;
    struct DataStack ds;
    tassert(dst_new(&ds) == NO_ERROR, "failed to init data stack");
    struct Object *lit = malloc(sizeof(*lit));
    tassert(lit, "failed to allocate space for lit");
    integral v = 12;
    tassert(ctno_literal(&v, sizeof(v), LTL_integral, NULL, lit) == NO_ERROR,
            "failed to init literal object");
    struct Object *dyn = malloc(sizeof(*dyn));
    tassert(dyn, "failed to allocate space for dyn");
    struct PropMap pm;
    tassert(pm_new(8, &pm) == NO_ERROR, "failed to init propmap for dyn");
    tassert(ctno_dynamic(pm, NULL, dyn) == NO_ERROR,
            "failed to init dynamic object");
    tassert(dst_push(&ds, dyn) == NO_ERROR, "failed to push to ds");
    tassert(dst_push(&ds, lit) == NO_ERROR, "failed to push to ds");
    struct Tokenizer test_ts_tknr;
    tassert(tknr_from_string("foo :bar 0xbad", "hi", &test_ts_tknr) == NO_ERROR,
            "failed to init ts's tknr");
    struct TokenStack ts;
    tassert(tst_new(test_ts_tknr, &ts) == NO_ERROR,
            "failed to init token stack");
    struct CodeBlock cb1, cb2;
    tassert(cb_new(1, &cb1) == NO_ERROR, "failed to init cb1");
    tassert(cb_append(&cb1, (struct Token) {
            .type = TKN_INTEGRAL
    }) == NO_ERROR, "failed to push token into cb");
    tassert(cb_new(1, &cb2) == NO_ERROR, "failed to init cb2");
    tassert(cb_append(&cb2, (struct Token) {
            .type = TKN_REGEX
    }) == NO_ERROR, "failed to push token into cb");
    
    struct Runnable c1, c2, c1c;
    tassert(rn_from_c(test1, &c1) == NO_ERROR, "failed to init c test 1");
    tassert(rn_copy(c1, &c1c) == NO_ERROR, "failed to copy c test 1");
    tassert(rn_eq(c1, c1c), "copy isn't equal to the original");
    rn_free(&c1c);
    tassert(rn_from_c(test2, &c2) == NO_ERROR, "failed to init c test 2");
    tassert(!rn_eq(c1, c2), "different c runnables compare equal");
    top_is_lit = true;
    top_type = TKN_WORD;
    tassert(rn_run(c1, &ds, NULL, &ts) == NO_ERROR,
            "failed to run correctly");
    tassert(rn_run(c2, &ds, NULL, &ts) == USER_DEFINED_ERROR,
            "got wrong error running c2");
    top_is_lit = false;
    top_type = TKN_IDENTIFIER;
    
    struct Runnable ctn1, ctn2, ctn1c;
    tassert(rn_from_ctn(cb1, &ctn1) == NO_ERROR, "failed to init ctn test 1");
    tassert(rn_copy(ctn1, &ctn1c) == NO_ERROR, "failed to copy ctn test 1");
    tassert(rn_eq(ctn1, ctn1c), "copy isn't equal to the original");
    rn_free(&ctn1c);
    tassert(rn_from_ctn(cb2, &ctn2) == NO_ERROR, "failed to init ctn test 2");
    tassert(!rn_eq(ctn1, ctn2), "different ctn runnables compare equal");
    tassert(rn_run(ctn1, &ds, NULL, &ts) == NO_ERROR, "failed to run");
    top_type = TKN_INTEGRAL;
    tassert(rn_run(c1, &ds, NULL, &ts) == NO_ERROR, "failed to run c");
    tassert(rn_run(ctn2, &ds, NULL, &ts) == NO_ERROR, "failed to run");
    top_type = TKN_REGEX;
    tassert(rn_run(c1, &ds, NULL, &ts) == NO_ERROR, "failed to run c`");
    tassert(rn_run(c2, &ds, NULL, &ts) == USER_DEFINED_ERROR,
            "got wrong error running c2");
    tassert(dst_push(&ds, dyn) == NO_ERROR, "failed to push again");
    top_type = TKN_INTEGRAL;
    tassert(rn_run(c1, &ds, NULL, &ts) == NO_ERROR, "failed to run c");
    tassert(rn_run(c1, &ds, NULL, &ts) == NO_ERROR, "failed to run c");
    tassert(rn_run(c1, &ds, NULL, &ts) == NO_ERROR, "failed to run c");
    tassert(rn_run(c1, &ds, NULL, &ts) == NO_ERROR, "failed to run c");
    tassert(rn_run(ctn2, &ds, NULL, &ts) == NO_ERROR, "failed to run");
    tassert(rn_run(ctn2, &ds, NULL, &ts) == NO_ERROR, "failed to run");
    top_type = TKN_REGEX;
    tassert(rn_run(c1, &ds, NULL, &ts) == NO_ERROR, "failed to run c`");
    
    dst_free(&ds);
    ctno_free(dyn);
    ctno_free(lit);
    pm_free(&pm);
    tknr_free(&test_ts_tknr);
    tst_free(&ts);
    cb_free(&cb1);
    cb_free(&cb2);
    rn_free(&c1);
    rn_free(&c2);
    rn_free(&ctn1);
    rn_free(&ctn2);
    
//    tassert(false, "Runnable tests incomplete");
    return (struct TestResult) { .successes = successes, .total = total };
}
