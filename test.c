#include "test.h"

#include <sys/time.h>
#include <stdbool.h>

bool do_test(struct TestResult (*testf)(), const char *test_name) {
    struct timeval beg, end;
    printf("Testing %-19s", test_name);
    gettimeofday(&beg, NULL);
    struct TestResult tr = testf();
    gettimeofday(&end, NULL);
    if (tr.successes != tr.total) {
        size_t fails = tr.total - tr.successes;
        printf("\n%zu failed in %zu us.\n", fails,
               (size_t) (end.tv_usec - beg.tv_usec));
        return false;
    } else {
        printf("%3zu succeeded in %4zu us.\n", tr.successes,
               (size_t) (end.tv_usec - beg.tv_usec));
        return true;
    }
}

#define test(module_name) \
    if (do_test(test_##module_name, #module_name "...")) ++idx; \
    else return idx;

int main() {
    size_t idx = 0;
    test(code_block);
    test(data_stack);
    test(method_map);
    test(object);
    test(prop_map);
    test(stringbuilder);
    test(token_stack);
    test(tokenizer);
    puts("Done with all tests. Completed with no errors.");
}
