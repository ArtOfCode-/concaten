#include "../test.h"

#include <string.h>
#include <stdlib.h>

#include "../tokenizer.h"
#include "../object.h"

struct Token tkn(char *raw, enum TokenType type) {
    size_t raw_len = strlen(raw) + 1;
    char *raw_c = malloc(raw_len);
    strncpy(raw_c, raw, raw_len);
    return (struct Token) {
            .raw_len = raw_len,
            .raw = raw_c,
            .origin = NULL,
            .type = type
    };
}

struct TestResult test_tkn_value() {
    ERROR prev_err;
    size_t successes = 0, total = 0;
    struct Object ret;
    struct Token t[] = {
            tkn("123", TKN_INTEGER), tkn("2.3", TKN_REAL),
            tkn("\"hi\"", TKN_STRING), tkn(":foo", TKN_IDENTIFIER)
    };
    struct Object o[4];
    integral o0_val = 123;
    tassert(!NOS_FAILED(
            ctno_literal(&o0_val, sizeof(o0_val), LTL_integral, NULL, &o[0])),
            "failed to initialize o0");
    real o1_val = 2.3;
    tassert(!NOS_FAILED(
            ctno_literal(&o1_val, sizeof(o1_val), LTL_real, NULL, &o[1])),
            "failed to initialize o1");
    char *o2_val = "hi";
    tassert(!NOS_FAILED(ctno_literal(o2_val, 3, LTL_string, NULL, &o[2])),
            "failed to initialize o2");
    char *o3_val = "foo";
    tassert(!NOS_FAILED(ctno_literal(o3_val, 4, LTL_identifier, NULL, &o[3])),
            "failed to initialize o3");
    
    for (size_t i = 0; i < 4; ++i) {
        tassert(!NOS_FAILED(tkn_value(&t[i], &ret)), "get fail at %zu", i);
        tassert(ctno_eq(o[i], ret), "wrong val at %zu", i);
    }

    struct Token word = tkn("asdf", TKN_WORD);
    tassert(FAILED(tkn_value(&word, &ret)),
            "got value for word");
    tassert(prev_err.errcode == TTO_WORDS_VALUELESS_FAIL, "got wrong error");
    struct Token unknown = tkn("adsf", TKN_UNKNOWN);
    tassert(FAILED(tkn_value(&unknown, &ret)),
            "got value for unknown");
    tassert(prev_err.errcode == TTO_UNKNOWN_TYPE_FAIL, "got wrong error");
    
    return (struct TestResult) { .successes = successes, .total = total };
}
