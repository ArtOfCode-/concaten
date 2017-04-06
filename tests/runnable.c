#include "../test.h"
#include "../runnable.h"

struct TestResult test_runnable() {
    size_t successes = 0, total = 0;
    tassert(false, "no Runnable tests yet");
    return (struct TestResult) { .successes = successes, .total = total };
}