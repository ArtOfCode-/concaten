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

void test_tokenizer();
void test_prop_map();
void test_method_map();
void test_object();
void test_data_stack();
void test_token_stack();
void test_code_block();

#endif //CONCATEN_TESTS_H
