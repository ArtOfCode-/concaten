#include "tests.h"

#define do_test(module_name) do { \
    ++idx; \
    printf("Testing %-15s", str(module_name)"..."); \
    struct TestResult tr = test_##module_name(); \
    if (tr.successes != tr.total) { \
        size_t fails = tr.total - tr.successes; \
        printf("\n%zu failure%s.\n", fails, fails == 1 ? "" : "s"); \
        return idx * 100 + fails; \
    } else { \
        printf(" %2zu successes.\n", tr.total); \
    } \
} while(0)

int main() {
    size_t idx = 0;
    do_test(tokenizer);
    do_test(prop_map);
    do_test(method_map);
    do_test(object);
    do_test(data_stack);
    do_test(token_stack);
    do_test(code_block);
    puts("Done with all tests");
}
