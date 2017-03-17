#include "../tests.h"

#include <sys/time.h>
#include <stdlib.h>

#include "../tokenizer.h"

enum FailType {
    FT_SUCCESS = 0,
    FT_WRONG_ERR = 1,
    FT_LESS_TOKENS = FT_WRONG_ERR << 1,
    FT_MORE_TOKENS = FT_LESS_TOKENS << 1,
    FT_WRONG_TYPE = FT_MORE_TOKENS << 1
};
struct Spec {
    ERROR code;
    size_t token_count;
    const char *source;
    bool is_from_file;
    enum TokenType *types;
};
struct Res {
    enum FailType result;
    enum TokenType last_token_type;
    ERROR code;
    long usec;
    size_t count;
};
struct Res test(const struct Spec ts) {
    ERROR err = 0;
    struct timeval start, stop;
    struct Token next = (struct Token) {
            .type = TKN_UNKNOWN,
            .index = 0,
            .line = 0,
            .origin = NULL,
            .raw = NULL,
            .origin_len = 0,
            .raw_len = 0,
    };
    size_t count = 0;
    enum FailType res = FT_SUCCESS;
    struct Tokenizer t;
    gettimeofday(&start, NULL);
    if (ts.is_from_file) {
        err = tknr_from_filepath(ts.source, &t);
    } else {
        err = tknr_from_string(ts.source, "<test>", &t);
    }
    if (err) {
        goto end;
    }
    while ((err = tknr_next(&t, &next)) == SUCCESS) {
        ++count;
        if (count > ts.token_count) {
            break;
        }
        if (ts.types && (next.type != ts.types[count - 1])) {
            break;
        }
    }
    if (tknr_end(&t) && err == SYN_STREAM_ENDED_FAIL) err = SUCCESS;
    end:;
    gettimeofday(&stop, NULL);
    if (count > ts.token_count) res |= FT_LESS_TOKENS;
    if (count < ts.token_count) res |= FT_MORE_TOKENS;
    if (ts.types && !tknr_end(&t)) {
        if (next.type != ts.types[count - 1]) res |= FT_WRONG_TYPE;
        tknr_free(&t);
    }
    if (ts.code != err) res |= FT_WRONG_ERR;
    struct Res ret = (struct Res) {
            .result = res,
            .usec = stop.tv_usec - start.tv_usec,
            .code = err,
            .last_token_type = next.type,
            .count = count
    };
    tkn_free(&next);
    return ret;
}

#define stest_e(_source, _code) (struct Spec) { \
    .source = _source, \
    .code = _code, \
    .types = NULL, \
    .token_count = 0, \
    .is_from_file = false \
}
#define stest_bd(_source) (struct Spec) { \
    .source = _source, \
    .code = SYN_NUM_ILLEGAL_DIGIT_FAIL, \
    .types = NULL, \
    .token_count = 0, \
    .is_from_file = false \
}
struct TestResult test_tokenizer() {
    // TODO Convert to use `tassert`
    struct Spec tests[] = {
            stest_e("", CTOR_STR_BAD_STRLEN_FAIL),
            stest_e("\"ends early", SYN_UNEXPECTED_END_FAIL),
            stest_e("r/ends early", SYN_UNEXPECTED_END_FAIL),
            stest_bd("0b012"),
            stest_bd("-0x1fg"),
            stest_bd("0o178"),
            // TODO test bad regex flag
            stest_bd("1f"),
            (struct Spec) {
                    .source = "success: \"string\" 42 0x1Fe94\n"
                            "0b11001 -0o127635 1.2e3 # Hello!\n"
                            ":foobar -> - foobar2\n"
                            "r/asdf boofar/xgi",
                    .is_from_file = false,
                    .code = SUCCESS,
                    .types = (enum TokenType[]) {
                            TKN_WORD, TKN_STRING, TKN_INTEGER, TKN_INTEGER,
                            TKN_INTEGER, TKN_INTEGER, TKN_REAL,
                            TKN_IDENTIFIER, TKN_WORD, TKN_WORD, TKN_WORD,
                            TKN_REGEX
                    },
                    .token_count = 12
            },
            (struct Spec) {
                    .source = "test.ctn",
                    .is_from_file = true,
                    .code = SUCCESS,
                    .types = NULL,
                    .token_count = 72
            }
    };
    size_t total = sizeof(tests) / sizeof(struct Spec);
    size_t successes = 0;
    for (size_t i = 0; i < total; ++i) {
        struct Spec current = tests[i];
        struct Res res = test(tests[i]);
        if (res.result == FT_SUCCESS) {
            ++successes;
        } else {
            printf("\nTest case %zu failed:", i);
            if (res.result & FT_MORE_TOKENS)
                printf("\n  Too few tokens parsed. (%zu, not %zu)",
                       res.count, current.token_count);
            if (res.result & FT_LESS_TOKENS)
                printf("\n  Too many tokens parsed. (%zu, not %zu)",
                       res.count, current.token_count);
            if (res.result & FT_WRONG_ERR)
                printf("\n  Unexpected error code received. ("EFMT", not "EFMT")",
                       res.code, current.code);
            if (res.result & FT_WRONG_TYPE)
                printf("\n  Unexpected token type received. (%s, not %s)",
                       tkn_type_name(res.last_token_type),
                       tkn_type_name(current.types[res.count - 1]));
        }
    }
    return (struct TestResult) { .successes = successes, .total = total };
}