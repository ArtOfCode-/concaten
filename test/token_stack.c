#include "../tests.h"

#include "../token_stack.h"

size_t strlen(const char *f) {
    size_t ret = 0;
    while (*f) {
        ++ret;
        ++f;
    }
    return ret;
}

struct Token gen_token(size_t layer_num) {
    static size_t num = 1;
    char *mem = malloc(12);
    sprintf(mem, "layer_%zu", layer_num);
    return (struct Token) {
            .line = layer_num,
            .index = num++,
            .origin = mem,
            .origin_len = strlen(mem) + 1,
            .raw = "gen_token-res",
            .raw_len = 14,
            .type = TKN_WORD
    };
}

static size_t total = 0;
static size_t successes = 0;

void push_assert(struct TokenStack *tst, struct Token pushing, bool expected) {
    ++total;
    if (tst_push(tst, pushing) == expected) ++successes;
}

void pop_assert(struct TokenStack *tst, size_t index, size_t line) {
    ++total;
    struct Token popped;
    tst_pop(tst, &popped);
    if (popped.index == index && popped.line == line) ++successes;
}

void push_level_assert(struct TokenStack *tst, bool expected) {
    ++total;
    if (tst_push_level(tst) == expected) ++successes;
}

void pop_level_assert(struct TokenStack *tst, bool expected) {
    ++total;
    if (tst_pop_level(tst) == expected) ++successes;
}

void restore_assert(struct TokenStack *tst, bool expected) {
    ++total;
    if (tst_restore_state(tst) == expected) ++successes;
}

void test_token_stack() {
    struct Tokenizer tknr = tknr_from_string("1 2 3 4 5", "mem");
    struct TokenStack tst = tst_new(tknr);
    struct Token popped;
    
    // TODO Rewrite tests to use asserts, print values at end
    printf("push: %d\n", tst_push(&tst, gen_token(1)));
    printf("push: %d\n", tst_push(&tst, gen_token(1)));
    printf("push: %d\n", tst_push(&tst, gen_token(1)));
    tst_push_level(&tst);
    printf("push: %d\n", tst_push(&tst, gen_token(2)));
    tst_push_level(&tst);
    // layer 3 intentionally left blank
    tst_push_level(&tst);
    printf("push: %d\n", tst_push(&tst, gen_token(4)));
    printf("push: %d\n", tst_push(&tst, gen_token(4)));
    printf("push: %d\n", tst_push(&tst, gen_token(4)));
    printf("push: %d\n", tst_push(&tst, gen_token(4)));
    tst_push_level(&tst);
    printf("push: %d\n", tst_push(&tst, gen_token(5)));
    printf("push: %d\n", tst_push(&tst, gen_token(5)));
    
    tst_save_state(&tst);
    printf("state saved: %d\n", tst.tracking_changes);
    
    printf("push: %d\n", tst_push(&tst, gen_token(5)));
    tst_push_level(&tst);
    printf("push: %d\n", tst_push(&tst, gen_token(6)));

    int res;
    res = tst_pop(&tst, &popped);
    printf("pop: %d - %zu-%zu %s %s\n", res, popped.index, popped.line, popped.origin, popped.raw);
    res = tst_pop(&tst, &popped);
    printf("pop: %d - %zu-%zu %s %s\n", res, popped.index, popped.line, popped.origin, popped.raw);
    res = tst_pop(&tst, &popped);
    printf("pop: %d - %zu-%zu %s %s\n", res, popped.index, popped.line, popped.origin, popped.raw);
    res = tst_pop(&tst, &popped);
    printf("pop: %d - %zu-%zu %s %s\n", res, popped.index, popped.line, popped.origin, popped.raw);
    res = tst_pop(&tst, &popped);
    printf("pop: %d - %zu-%zu %s %s\n", res, popped.index, popped.line, popped.origin, popped.raw);
    
    printf("restoring state...");
    tst_restore_state(&tst);
    puts("done");
    
    res = tst_pop(&tst, &popped);
    printf("pop: %d - %zu-%zu %s %s\n", res, popped.index, popped.line, popped.origin, popped.raw);
    res = tst_pop(&tst, &popped);
    printf("pop: %d - %zu-%zu %s %s\n", res, popped.index, popped.line, popped.origin, popped.raw);
    res = tst_pop(&tst, &popped);
    printf("pop: %d - %zu-%zu %s %s\n", res, popped.index, popped.line, popped.origin, popped.raw);
    res = tst_pop(&tst, &popped);
    printf("pop: %d - %zu-%zu %s %s\n", res, popped.index, popped.line, popped.origin, popped.raw);
    res = tst_pop(&tst, &popped);
    printf("pop: %d - %zu-%zu %s %s\n", res, popped.index, popped.line, popped.origin, popped.raw);
    res = tst_pop(&tst, &popped);
    printf("pop: %d - %zu-%zu %s %s\n", res, popped.index, popped.line, popped.origin, popped.raw);
    res = tst_pop(&tst, &popped);
    printf("pop: %d - %zu-%zu %s %s\n", res, popped.index, popped.line, popped.origin, popped.raw);
    res = tst_pop(&tst, &popped);
    printf("pop: %d - %zu-%zu %s %s\n", res, popped.index, popped.line, popped.origin, popped.raw);
    res = tst_pop(&tst, &popped);
    printf("pop: %d - %zu-%zu %s %s\n", res, popped.index, popped.line, popped.origin, popped.raw);
    res = tst_pop(&tst, &popped);
    printf("pop: %d - %zu-%zu %s %s\n", res, popped.index, popped.line, popped.origin, popped.raw);
    res = tst_pop(&tst, &popped);
    printf("pop: %d - %zu-%zu %s %s\n", res, popped.index, popped.line, popped.origin, popped.raw);
    res = tst_pop(&tst, &popped);
    printf("pop: %d - %zu-%zu %s %s\n", res, popped.index, popped.line, popped.origin, popped.raw);
    res = tst_pop(&tst, &popped);
    printf("pop: %d - %zu-%zu %s %s\n", res, popped.index, popped.line, popped.origin, popped.raw);
    res = tst_pop(&tst, &popped);
    printf("pop: %d - %zu-%zu %s %s\n", res, popped.index, popped.line, popped.origin, popped.raw);
    res = tst_pop(&tst, &popped);
    printf("pop: %d - %zu-%zu %s %s\n", res, popped.index, popped.line, popped.origin, popped.raw);
    res = tst_pop(&tst, &popped);
    printf("pop: %d - %zu-%zu %s %s\n", res, popped.index, popped.line, popped.origin, popped.raw);
    res = tst_pop(&tst, &popped);
    printf("pop: %d - %zu-%zu %s %s\n", res, popped.index, popped.line, popped.origin, popped.raw);
    res = tst_pop(&tst, &popped);
    printf("pop: %d - %zu-%zu %s %s\n", res, popped.index, popped.line, popped.origin, popped.raw);
    
    puts("freeing");
    tst_free(&tst);
    tknr_free(&tknr);
    puts("done");
}
