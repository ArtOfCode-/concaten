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
    size_t successes = 0, total = 0;
    struct Object ret;
    struct Token t[] = {
            tkn("123", TKN_INTEGER), tkn("2.3", TKN_REAL),
            tkn("\"hi\"", TKN_STRING), tkn(":foo", TKN_IDENTIFIER),
            tkn("\"\\\"\\e[31mhell\\e[0mo!\\\"\"", TKN_STRING)
    };
    struct Object o[5];
    integral o0_v = 123;
    tassert(ctno_literal(&o0_v, sizeof(o0_v), LTL_integral, NULL, &o[0])
            == NO_ERROR, "failed to initialize o0");
    real o1_v = 2.3;
    tassert(ctno_literal(&o1_v, sizeof(o1_v), LTL_real, NULL, &o[1])
            == NO_ERROR, "failed to initialize o1");
    char *o2_v = "hi";
    tassert(ctno_literal(o2_v, strlen(o2_v) + 1, LTL_string, NULL, &o[2])
            == NO_ERROR, "failed to initialize o2");
    char *o3_v = "foo";
    tassert(ctno_literal(o3_v, strlen(o3_v) + 1, LTL_identifier, NULL, &o[3])
            == NO_ERROR, "failed to initialize o3");
    char *o4_v = "\"\x07[31mhell\x07[0mo!\"";
    tassert(ctno_literal(o4_v, strlen(o4_v) + 1, LTL_string, NULL, &o[4])
            == NO_ERROR, "failed to initialize o4");
    
    for (size_t i = 0; i < sizeof(t) / sizeof(t[0]); ++i) {
        tassert(tkn_value(&t[i], &ret) == NO_ERROR, "get fail at %zu", i);
        tassert(ctno_eq(o[i], ret), "wrong val at %zu", i);
        ctno_free(&o[i]);
        ctno_free(&ret);
    }

    struct Token word = tkn("asdf", TKN_WORD);
    tassert(tkn_value(&word, &ret) == TTO_WORDS_VALUELESS_FAIL,
            "got value for word");
    tkn_free(&word);
    struct Token unknown = tkn("adsf", TKN_UNKNOWN);
    tassert(tkn_value(&unknown, &ret) == TTO_UNKNOWN_TYPE_FAIL,
            "got value for unknown");
    tkn_free(&unknown);
    
    return (struct TestResult) { .successes = successes, .total = total };
}
