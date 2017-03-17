#ifndef CONCATEN_TESTS_H
#define CONCATEN_TESTS_H

#include <stdio.h>

#define _str_back(x) #x
#define str(x) _str_back(x)
#define tassert(cond, ...) do {\
    ++total; \
    if (cond) { ++successes; } \
    else { \
        printf(__FILE__ ":" str(__LINE__) " test failed: "); \
        printf(__VA_ARGS__); \
        puts(""); \
    } \
} while(0)

struct ModuleTestResult {
    size_t successes, total;
};

struct ModuleTestResult test_tokenizer();
struct ModuleTestResult test_prop_map();
struct ModuleTestResult test_method_map();
struct ModuleTestResult test_object();
struct ModuleTestResult test_data_stack();
struct ModuleTestResult test_token_stack();
struct ModuleTestResult test_code_block();

#endif //CONCATEN_TESTS_H
