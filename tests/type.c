#include "../test.h"

#include "../type.h"

struct TestResult test_type() {
    size_t total = 0, successes = 0;
    
    struct Type t1, t2, t3, t4;

    
    tp_free(&t1);
    tp_free(&t2);
    tp_free(&t3);
    tp_free(&t4);
    
    return (struct TestResult) { .total = total, .successes = successes };
}
