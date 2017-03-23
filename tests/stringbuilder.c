#include "../test.h"

#include "../stringbuilder.h"

#include <string.h>

struct TestResult test_stringbuilder() {
    size_t total = 0, successes = 0;
    struct StringBuilder sb;
    tassert(sb_new(4, &sb) == NO_ERROR, "failed to init");
    tassert(sb.count == 0, "bad initial count");
    tassert(sb.count == strlen(sb.mem), "bad mem strlen");
    tassert(sb.cap == 4, "bad initial cap");
    tassert(strcmp("", sb.mem) == 0, "bad initial content");
    tassert(sb_append(&sb, 'a') == NO_ERROR, "failed to append");
    tassert(sb_append(&sb, 'b') == NO_ERROR, "failed to append");
    tassert(sb_append(&sb, 'c') == NO_ERROR, "failed to append");
    tassert(sb.cap == 4, "expanded early");
    tassert(sb.count == 3, "bad count");
    tassert(strlen(sb.mem) == 3, "bad mem strlen");
    tassert(sb_append(&sb, 'd') == NO_ERROR, "failed to append");
    tassert(sb.cap > 4, "failed to expand when expected");
    tassert(sb.count == 4, "bad count");
    tassert(strlen(sb.mem) == 4, "bad mem strlen");
    tassert(strcmp("abcd", sb.mem) == 0, "bad content");
    tassert(sb_append(&sb, 'e') == NO_ERROR, "failed to append");
    tassert(sb_append(&sb, 'f') == NO_ERROR, "failed to append");
    tassert(sb_append(&sb, 'g') == NO_ERROR, "failed to append");
    tassert(sb_append(&sb, 'h') == NO_ERROR, "failed to append");
    tassert(sb.cap > 8, "failed to expand when expected");
    tassert(sb.count == 8, "bad count");
    tassert(strlen(sb.mem) == 8, "bad mem strlen");
    tassert(strcmp("abcdefgh", sb.mem) == 0, "bad content");
    sb_free(&sb);
    
    return (struct TestResult) { .successes = successes, .total = total };
}