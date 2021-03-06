#include "../test.h"

#include <stdlib.h>

#include "../tokenizer.h"

static size_t successes = 0, total = 0;

struct Spec {
    ERROR code;
    size_t token_count;
    const char *source;
    bool is_from_file;
    enum TokenType *types;
};
void test(const struct Spec ts, size_t i) {
    ERROR err = 0;
    // we have to initialize it in case the first get fails
    struct Token next = (struct Token) {
            .type = TKN_UNKNOWN,
            .index = 0,
            .line = 0,
            .origin = NULL,
            .raw = NULL,
            .origin_len = 0,
            .raw_len = 0,
    };
    size_t cnt = 0;
    struct Tokenizer t;
    if (ts.is_from_file) {
        err = tknr_from_filepath(ts.source, &t);
    } else {
        err = tknr_from_string(ts.source, "<test>", &t);
    }
    if (err != NO_ERROR) {
        tassert(err == ts.code, "%zu: init error (" EFMT " not " EFMT ")%s",
                i, err, ts.code, err == TKNR_CTOR_FILE_FOPEN_FAIL 
                                 ? " (Make sure tft.ctn is in tests/)" : "");
    } else {
        while ((err = tknr_next(&t, &next)) == NO_ERROR) {
            ++cnt;
            if (cnt > ts.token_count) {
                tkn_free(&next);
                break;
            }
            if (ts.types && (next.type != ts.types[cnt - 1])) {
                tkn_free(&next);
                break;
            }
            tkn_free(&next);
        }
        if (err == TKNR_NT_INPUT_END_FAIL && tknr_end(t)) err = NO_ERROR;
        tassert(cnt >= ts.token_count, "%zu: Too few tokens (got %zu)",
                i, cnt);
        tassert(cnt <= ts.token_count, "%zu: Too many tokens (got %zu)",
                i, cnt);
        if (ts.types && !tknr_end(t)) {
            tassert(next.type == ts.types[cnt - 1],
                    "%zu: Wrong type (%s not %s)", i,
                    tkn_type_name(next.type), tkn_type_name(ts.types[cnt-1]));
        }
        tassert(ts.code == err, "%zu: Incorrect code (" EFMT ", not " EFMT ")",
                i, err, ts.code);
        tknr_free(&t);
    }
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
    .code = TKNR_SYN_NUM_ILLEGAL_DIGIT_FAIL, \
    .types = NULL, \
    .token_count = 0, \
    .is_from_file = false \
}
struct TestResult test_tokenizer() {
    struct Spec tests[] = {
            stest_e("", TKNR_CTOR_STR_BAD_STRLEN_FAIL),
            stest_e("\"ends early", TKNR_SYN_UNEXPECTED_END_FAIL),
            stest_e("r/ends early", TKNR_SYN_UNEXPECTED_END_FAIL),
            stest_bd("0b012"),
            stest_bd("-0x1fg"),
            stest_bd("0o178"),
            (struct Spec) {
                    .source = "r/ege/xkcd",
                    .code = TKNR_SYN_RGX_BAD_FLAG_FAIL,
                    .is_from_file = false,
                    .token_count = 0,
                    .types = NULL
            },
            stest_bd("1f"),
            (struct Spec) {
                    .source = "success: \"string\" 42 0x1Fe94\n"
                            "0b11001 -0o127635 1.2e3 # Hello!\n"
                            ":foobar -> - foobar2\n"
                            "r/asdf boofar/xgi",
                    .is_from_file = false,
                    .code = NO_ERROR,
                    .types = (enum TokenType[]) {
                            TKN_WORD, TKN_STRING, TKN_INTEGRAL, TKN_INTEGRAL,
                            TKN_INTEGRAL, TKN_INTEGRAL, TKN_REAL,
                            TKN_IDENTIFIER, TKN_WORD, TKN_WORD, TKN_WORD,
                            TKN_REGEX
                    },
                    .token_count = 12
            },
            (struct Spec) {
                    .source = "./tests/tft.ctn",
                    .is_from_file = true,
                    .code = NO_ERROR,
                    .types = NULL,
                    .token_count = 82
            }
    };
    size_t test_num = sizeof(tests) / sizeof(tests[0]);
    for (size_t i = 0; i < test_num; ++i) {
        test(tests[i], i);
    }
    
    return (struct TestResult) { .successes = successes, .total = total };
}
