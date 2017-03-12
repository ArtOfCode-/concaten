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

struct Token gen_token(size_t num, size_t layer_num) {
    char *mem = malloc(12);
    sprintf(mem, "layer_%zu", layer_num);
    return (struct Token) {
            .line = layer_num,
            .index = num,
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
    else printf("unexpected result when pushing (%d)\n", !expected);
}

void pop_assert(struct TokenStack *tst, bool expected, size_t index, size_t line) {
    ++total;
    struct Token popped;
    if (!tst_pop(tst, &popped)) {
        if (!expected) ++successes;
        else printf("unexpected failure when popping (%d)\n", !expected);
        return;
    }
    if (popped.index == index && popped.line == line) ++successes;
    else printf("unexpected result when popping (%zu %zu, not %zu %zu)\n",
        popped.index, popped.line, index, line);
}

void push_level_assert(struct TokenStack *tst, bool expected) {
    ++total;
    if (tst_push_level(tst) == expected) ++successes;
    else printf("unexpected result when pushing level (%d)", !expected);
}

void pop_level_assert(struct TokenStack *tst, bool expected) {
    ++total;
    if (tst_pop_level(tst) == expected) ++successes;
    else printf("unexpected result when popping level (%d)\n", !expected);
}

void restore_assert(struct TokenStack *tst, bool expected) {
    ++total;
    if (tst_restore_state(tst) == expected) ++successes;
    else printf("unexpected result when restoring (%d)\n", !expected);
}

void save_assert(struct TokenStack *tst, bool expected) {
    
}

void test_token_stack() {
    struct Tokenizer tknr = tknr_from_string("1 2 3 4 5", "mem");
    struct TokenStack tst = tst_new(tknr);
    
    // TODO Test pop_level
    push_assert(&tst, gen_token(1, 1), true);
    push_assert(&tst, gen_token(2, 1), true);
    push_assert(&tst, gen_token(3, 1), true);
    push_level_assert(&tst, true);
    push_assert(&tst, gen_token(4, 2), true);
    tst_push_level(&tst);
    push_level_assert(&tst, true);
    push_assert(&tst, gen_token(5, 4), true);
    push_assert(&tst, gen_token(6, 4), true);
    push_assert(&tst, gen_token(7, 4), true);
    push_assert(&tst, gen_token(8, 4), true);
    push_level_assert(&tst, true);
    push_assert(&tst, gen_token(9, 5), true);
    push_assert(&tst, gen_token(10, 5), true);
    tst_save_state(&tst);
    push_assert(&tst, gen_token(11, 5), true);
    push_level_assert(&tst, true);
    push_assert(&tst, gen_token(12, 6), true);
    push_assert(&tst, gen_token(13, 6), true);
    pop_assert(&tst, true, 13, 6);
    pop_assert(&tst, true, 12, 6);
    pop_assert(&tst, true, 11, 5);
    push_assert(&tst, gen_token(14, 7), true);
    push_assert(&tst, gen_token(15, 7), true);
    push_assert(&tst, gen_token(16, 7), true);
    pop_assert(&tst, true, 16, 7);
    pop_assert(&tst, true, 15, 7);
    pop_assert(&tst, true, 14, 7);
    pop_assert(&tst, true, 10, 5);
    pop_assert(&tst, true, 9, 5);
    restore_assert(&tst, true);
    pop_assert(&tst, true, 10, 5);
    pop_assert(&tst, true, 9, 5);
    pop_assert(&tst, true, 8, 4);
    pop_assert(&tst, true, 7, 4);
    push_assert(&tst, gen_token(14, 8), true);
    push_assert(&tst, gen_token(15, 8), true);
    push_assert(&tst, gen_token(16, 8), true);
    pop_assert(&tst, true, 16, 8);
    pop_assert(&tst, true, 15, 8);
    pop_assert(&tst, true, 14, 8);
    pop_assert(&tst, true, 6, 4);
    pop_assert(&tst, true, 5, 4);
    pop_assert(&tst, true, 4, 2);
    tst_save_state(&tst);
    pop_assert(&tst, true, 3, 1);
    pop_assert(&tst, true, 2, 1);
    pop_assert(&tst, true, 1, 1);
    pop_assert(&tst, true, 0, 1);
    pop_assert(&tst, true, 2, 1);
    pop_assert(&tst, true, 4, 1);
    restore_assert(&tst, true);
    pop_assert(&tst, true, 3, 1);
    pop_assert(&tst, true, 2, 1);
    pop_assert(&tst, true, 1, 1);
    pop_assert(&tst, true, 0, 1);
    pop_assert(&tst, true, 2, 1);
    pop_assert(&tst, true, 4, 1);
    pop_assert(&tst, true, 6, 1);
    pop_assert(&tst, true, 8, 1);
    pop_assert(&tst, false, 0, 0);
    pop_assert(&tst, false, 0, 0);
    
    tst_free(&tst);
    tknr_free(&tknr);
    
    printf("%zu/%zu successes", successes, total);
}
