#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

#include "tokenizer.h"

enum FailType {
    FT_SUCCESS = 0,
    FT_WRONG_ERR = 1,
    FT_TOO_MANY_TOKENS = FT_WRONG_ERR << 1,
    FT_TOO_FEW_TOKENS = FT_TOO_MANY_TOKENS << 1,
    FT_WRONG_TYPE = FT_TOO_FEW_TOKENS << 1
};
struct TestSpec {
    int code;
    enum TokenType *types;
    size_t types_count;
    const char *source;
    bool is_from_file;
};
struct TestResult {
    enum FailType result;
    int code;
    long usec;
};
struct TestResult test(const struct TestSpec ts) {
    int err = 0;
    struct timeval start, stop;
    struct Token next;
    size_t count = 0;
    enum FailType res = FT_SUCCESS;
    Tokenizer t;
    gettimeofday(&start, NULL);
    t = ts.is_from_file
        ? tknr_from_filepath(ts.source)
        : tknr_from_string(ts.source, "<test>");
    err = tknr_err(t);
    if (err) {
        goto end;
    }
    while (tknr_next(t, &next)) {
        ++count;
        if (count > ts.types_count) {
            break;
        }
        if (next.type != ts.types[count - 1]) {
            break;
        }
    }
    err = tknr_err(t);
end:;
    gettimeofday(&stop, NULL);
    if (count > ts.types_count) res |= FT_TOO_MANY_TOKENS;
    if (count < ts.types_count) res |= FT_TOO_FEW_TOKENS;
    if (next.type != ts.types[count - 1]) res |= FT_WRONG_TYPE;
    if (ts.code != err) res |= FT_WRONG_ERR;
    struct TestResult ret = (struct TestResult) {
            .result = res,
            .usec = stop.tv_usec - start.tv_usec,
            .code = tknr_err(t)
    };
    tknr_free(t);
    tkn_free(&next);
    return ret;
}

size_t string_tests() {
    // TODO test edge cases
    const char *tests[] = {
            "", "\"ends early", "r/ends early",
            "bad-digit 0x123gi", "bad-digit 0b123",
            "bad-digit 0o678", "r/bad flag/gli",
            "success: \"string\" 42 0x1Fe94\n"
            "0b11001 0o127635 1.2e3\n"
            ":foobar foobar2\n"
            "r/asdf boofar/xgi"
    };
    size_t test_count = sizeof(tests) / sizeof(char *);
    for (size_t i = 0; i < test_count; ++i) {
        printf("String test case #%zu:\n", i + 1);
        puts(tests[i]);
        puts("---");
        const char *test_string = tests[i];
        Tokenizer tknr = tknr_from_string(test_string, "test");
        int err = tknr_err(tknr);
        if (err != 0) {
            printf(" Error %d occured when initializing.\n", err);
            continue;
        }
        struct Token next;
        while (tknr_next(tknr, &next)) {
            printf(" `%s` (%s - %d) at %s@%zu:%zu\n", tkn_raw(next), tkn_type_name(next),
                   tkn_type(next), tkn_origin(next), tkn_line(next), tkn_index(next));
            tkn_free(&next);
        }
        err = tknr_err(tknr);
        if (err != 0) {
            printf(" Error %d occured while tokenizing.\n", err);
        }
        tknr_free(tknr);
        puts("---");
    }
    return test_count;
}

size_t file_tests() {
    // TODO test edge cases
    char *test_paths[] = {
            "test.ctn"
    };
    size_t test_count = sizeof(test_paths) / sizeof(char *);
    for (size_t i = 0; i < test_count; ++i) {
        printf("File test case #%zu: %s\n", i + 1, test_paths[i]);
        Tokenizer tknr = tknr_from_filepath(test_paths[i]);
        int err = tknr_err(tknr);
        if (err != 0) {
            printf(" Error %d occured when initializing.\n", err);
            continue;
        }
        struct Token next;
        while (tknr_next(tknr, &next)) {
            printf(" `%s` (%s - %d) at %s@%zu:%zu\n", tkn_raw(next), tkn_type_name(next),
                   tkn_type(next), tkn_origin(next), tkn_line(next), tkn_index(next));
            tkn_free(&next);
        }
        err = tknr_err(tknr);
        if (err != 0) {
            printf(" Error %d occured while tokenizing.\n", err);
        }
        tknr_free(tknr);
        puts("---");
    }
    return test_count;
}

int main() {
    // TODO thoroughly test tokenizer (file > memory)
    // TODO better test framework (pass in just-created tokenizer, expected error, etc., get success/fail)
    // this will reduce code duplication and make it easy to track how many unit tests fail
    
    struct TestSpec tests[] = {
            (struct TestSpec) {
                    .source = "",
                    .is_from_file = false,
                    .code = 1112,
                    .types = NULL,
                    .types_count = 0
            },
            (struct TestSpec) {
                    .source = "\"ends early",
                    .is_from_file = false,
                    .types = NULL,
                    .types_count = 0,
                    .code = 1502
            },
            (struct TestSpec) {
                    .source = "r/ends early",
                    .is_from_file = false,
                    .types = NULL,
                    .types_count = 0,
                    .code = 1502
            }
    };
    size_t total = sizeof(tests) / sizeof(struct TestSpec);
    size_t fails = 0;
    for (size_t i; i < total; ++i) {
        struct TestSpec current = tests[i];
        printf("Test case %zu from %s:", i, current.is_from_file ? "file" : "string");
        puts(tests[i].source);
        struct TestResult res = test(tests[i]);
        if (res.result != FT_SUCCESS) {
            ++fails;
            printf("Tokenizer error %d (expected %d). Details:\n", res.code, current.code);
            // todo details for each of these
            if (res.result & FT_TOO_FEW_TOKENS) printf(" Too few tokens parsed.\n");
            if (res.result & FT_TOO_MANY_TOKENS) printf(" Too many tokens parsed.\n");
            if (res.result & FT_WRONG_ERR) printf(" Unexpected error code received. (%d, not %d)\n",
                                                  res.code, current.code);
            if (res.result & FT_WRONG_TYPE) printf(" Unexpected token type received. (%d, not %d)\n",
                                                   foo, current.code); // todo add those data
        }
    }
}
