#include "tests.h"

#include <sys/time.h>
#include <stdbool.h>

bool __test(struct TestResult (*testf)(), const char *test_name) {
    struct timeval beg, end;
    printf("Testing %-19s", test_name);
    gettimeofday(&beg, NULL);
    struct TestResult tr = testf();
    gettimeofday(&end, NULL);
    if (tr.successes != tr.total) {
        size_t fails = tr.total - tr.successes;
        printf("\n%zu failed in %zu us.\n", fails, end.tv_usec - beg.tv_usec);
        return false;
    } else {
        printf("%3zu succeeded in %3zu us.\n", tr.successes,
               end.tv_usec - beg.tv_usec);
        return true;
    }
}

#define do_test(module_name) \
    if (__test(test_##module_name, #module_name "...")) ++idx; \
    else return idx;

int main() {
    size_t idx = 0;
    do_test(code_block);
    do_test(data_stack);
    do_test(method_map);
    do_test(object);
    do_test(prop_map);
    do_test(stringbuilder);
    do_test(token_stack);
    do_test(tokenizer);
    puts("Done with all tests");
}
