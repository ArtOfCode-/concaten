#ifndef CONCATEN_TESTS_H
#define CONCATEN_TESTS_H

#include <stdio.h>

#define _str_back(x) #x
#define str(x) _str_back(x)
#define tassert(cond, ...) do {\
    ++total; \
    if (cond) { ++successes; } \
    else { \
        printf("\n" __FILE__ ":" str(__LINE__) ": "); \
        printf(__VA_ARGS__); \
    } \
} while(0)

struct TestResult {
    size_t successes, total;
};

struct TestResult test_stringbuilder();
struct TestResult test_tokenizer();
struct TestResult test_prop_map();
struct TestResult test_method_map();
struct TestResult test_object();
struct TestResult test_data_stack();
struct TestResult test_token_stack();
struct TestResult test_code_block();
struct TestResult test_tkn_value();

#endif //CONCATEN_TESTS_H
