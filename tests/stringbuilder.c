#include "../test.h"

#include <string.h>

#include "../stringbuilder.h"

struct TestResult test_stringbuilder() {
    size_t total = 0, successes = 0;
    struct StringBuilder sb;
    tassert(!NOS_FAILED(sb_new(4, &sb)), "failed to init");
    tassert(sb.count == 0, "bad initial count");
    tassert(sb.count == strlen(sb.mem), "bad mem strlen");
    tassert(sb.cap == 4, "bad initial cap");
    tassert(strcmp("", sb.mem) == 0, "bad initial content");
    tassert(!NOS_FAILED(sb_append(&sb, 'a')), "failed to append");
    tassert(!NOS_FAILED(sb_append(&sb, 'b')), "failed to append");
    tassert(!NOS_FAILED(sb_append(&sb, 'c')), "failed to append");
    tassert(sb.cap == 4, "expanded early");
    tassert(sb.count == 3, "bad count");
    tassert(strlen(sb.mem) == 3, "bad mem strlen");
    tassert(!NOS_FAILED(sb_append(&sb, 'd')), "failed to append");
    tassert(sb.cap > 4, "failed to expand when expected");
    tassert(sb.count == 4, "bad count");
    tassert(strlen(sb.mem) == 4, "bad mem strlen");
    tassert(strcmp("abcd", sb.mem) == 0, "bad content");
    tassert(!NOS_FAILED(sb_append(&sb, 'e')), "failed to append");
    tassert(!NOS_FAILED(sb_append(&sb, 'f')), "failed to append");
    tassert(!NOS_FAILED(sb_append(&sb, 'g')), "failed to append");
    tassert(!NOS_FAILED(sb_append(&sb, 'h')), "failed to append");
    tassert(sb.cap > 8, "failed to expand when expected");
    tassert(sb.count == 8, "bad count");
    tassert(strlen(sb.mem) == 8, "bad mem strlen");
    tassert(strcmp("abcdefgh", sb.mem) == 0, "bad content");
    sb_free(&sb);
    
    return (struct TestResult) { .successes = successes, .total = total };
}
