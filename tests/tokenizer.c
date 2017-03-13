#include "../tests.h"

#include <string.h>
#include <stdlib.h>
#include <sys/time.h>

#include "../tokenizer.h"

enum FailType {
    FT_SUCCESS = 0,
    FT_WRONG_ERR = 1,
    FT_LESS_TOKENS = FT_WRONG_ERR << 1,
    FT_MORE_TOKENS = FT_LESS_TOKENS << 1,
    FT_WRONG_TYPE = FT_MORE_TOKENS << 1
};
struct TestSpec {
    int code;
    size_t token_count;
    const char *source;
    bool is_from_file;
    enum TokenType *types;
};
struct TestResult {
    enum FailType result;
    enum TokenType last_token_type;
    int code;
    long usec;
    size_t count;
};
struct TestResult test(const struct TestSpec ts) {
    int err = 0;
    struct timeval start, stop;
    struct Token next = tkn_empty(0, 0);
    size_t count = 0;
    enum FailType res = FT_SUCCESS;
    struct Tokenizer t;
    gettimeofday(&start, NULL);
    if (ts.is_from_file) {
        t = tknr_from_filepath(ts.source);
    } else {
        t = tknr_from_string(ts.source, "<test>");
    }
    err = t.error;
    if (err) {
        goto end;
    }
    while (tknr_next(&t, &next)) {
        ++count;
        if (count > ts.token_count) {
            break;
        }
        if (ts.types && (next.type != ts.types[count - 1])) {
            break;
        }
    }
    err = t.error;
    end:;
    gettimeofday(&stop, NULL);
    if (count > ts.token_count) res |= FT_LESS_TOKENS;
    if (count < ts.token_count) res |= FT_MORE_TOKENS;
    if (ts.types && !tknr_end(&t)) {
        if (next.type != ts.types[count - 1]) res |= FT_WRONG_TYPE;
        tknr_free(&t);
    }
    if (ts.code != err) res |= FT_WRONG_ERR;
    struct TestResult ret = (struct TestResult) {
            .result = res,
            .usec = stop.tv_usec - start.tv_usec,
            .code = t.error,
            .last_token_type = next.type,
            .count = count
    };
    tkn_free(&next);
    return ret;
}

#define stest_e(_source, _code) (struct TestSpec) { \
    .source = _source, \
    .code = _code, \
    .types = (enum TokenType[]){ }, \
    .token_count = 0, \
    .is_from_file = false \
}
#define stest_bd(_source) (struct TestSpec) { \
    .source = _source, \
    .code = 1521, \
    .types = NULL, \
    .token_count = 0, \
    .is_from_file = false \
}
void test_tokenizer() {
    struct TestSpec tests[] = {
            stest_e("", 1112),
            stest_e("\"ends early", 1502), stest_e("r/ends early", 1502),
            stest_bd("0b012"), stest_bd("0x1fg"), stest_bd("0o178"), stest_bd("1f"),
            (struct TestSpec) {
                    .source = "success: \"string\" 42 0x1Fe94\n"
                            "0b11001 0o127635 1.2e3 # Hello!\n"
                            ":foobar foobar2\n"
                            "r/asdf boofar/xgi",
                    .is_from_file = false,
                    .code = 0,
                    .types = (enum TokenType[]) {
                            TKN_WORD, TKN_STRING, TKN_INTEGER, TKN_INTEGER,
                            TKN_INTEGER, TKN_INTEGER, TKN_REAL,
                            TKN_IDENTIFIER, TKN_WORD,
                            TKN_REGEX
                    },
                    .token_count = 10
            },
            (struct TestSpec) {
                    .source = "test.ctn",
                    .is_from_file = true,
                    .code = 0,
                    .types = NULL,
                    .token_count = 72
            }
    };
    size_t total = sizeof(tests) / sizeof(struct TestSpec);
    size_t fails = 0;
    for (size_t i = 0; i < total; ++i) {
        struct TestSpec current = tests[i];
        printf("Test case %zu from %s:\n", i + 1, current.is_from_file ? "file" : "string");
        puts(tests[i].source);
        puts("---");
        struct TestResult res = test(tests[i]);
        if (res.result != FT_SUCCESS) {
            ++fails;
            printf("Failure in %lu usec. Details:\n", res.usec);
            if (res.result & FT_MORE_TOKENS) printf(" Too few tokens parsed. (%zu, not %zu)\n",
                                                    res.count, current.token_count);
            if (res.result & FT_LESS_TOKENS) printf(" Too many tokens parsed. (%zu, not %zu)\n",
                                                    res.count, current.token_count);
            if (res.result & FT_WRONG_ERR) printf(" Unexpected error code received. (%d, not %d)\n",
                                                  res.code, current.code);
            if (res.result & FT_WRONG_TYPE) printf(" Unexpected token type received. (%s, not %s)\n",
                                                   tkn_type_name(res.last_token_type),
                                                   tkn_type_name(current.types[res.count - 1]));
        } else {
            printf("Success in %lu usec\n", res.usec);
        }
        puts("---");
    }
    printf("%zu/%zu failed.\n", fails, total);
}