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

void a_token_push(struct TokenStack *tst, struct Token pushing, bool res) {
    ++total;
    if (tst_push(tst, pushing) == res) ++successes;
    else printf("unexpected result when pushing (%d)\n", !res);
}

void a_pop(struct TokenStack *tst, bool expected, size_t index, size_t line) {
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

void a_level_push(struct TokenStack *tst, bool expected) {
    ++total;
    if (tst_push_level(tst) == expected) ++successes;
    else printf("unexpected result when pushing level (%d)", !expected);
}

void a_level_pop(struct TokenStack *tst, bool expected) {
    ++total;
    if (tst_pop_level(tst) == expected) ++successes;
    else printf("unexpected result when popping level (%d)\n", !expected);
}

void a_restore(struct TokenStack *tst, bool expected) {
    ++total;
    if (tst_restore_state(tst) == expected) ++successes;
    else printf("unexpected result when restoring (%d)\n", !expected);
}

void a_save(struct TokenStack *tst) {
    ++total;
    tst_save_state(tst);
    if (tst->tracking_changes) ++successes;
    else puts("unexpected result when saving state (0)");
}

void test_token_stack() {
    struct Tokenizer tknr = tknr_from_string("1 2 3 4 5", "mem");
    struct TokenStack tst = tst_new(tknr);
    
    // TODO Test pop_level
    a_token_push(&tst, gen_token(1, 1), true);
    a_token_push(&tst, gen_token(2, 1), true);
    a_token_push(&tst, gen_token(3, 1), true);
    a_level_push(&tst, true);
    a_token_push(&tst, gen_token(4, 2), true);
    a_level_push(&tst, true);
    a_level_push(&tst, true);
    a_token_push(&tst, gen_token(5, 4), true);
    a_token_push(&tst, gen_token(6, 4), true);
    a_token_push(&tst, gen_token(7, 4), true);
    a_token_push(&tst, gen_token(8, 4), true);
    a_level_push(&tst, true);
    a_token_push(&tst, gen_token(9, 5), true);
    a_token_push(&tst, gen_token(10, 5), true);
    a_save(&tst);
    a_token_push(&tst, gen_token(11, 5), true);
    a_level_push(&tst, true);
    a_token_push(&tst, gen_token(12, 6), true);
    a_token_push(&tst, gen_token(13, 6), true);
    a_pop(&tst, true, 13, 6);
    a_pop(&tst, true, 12, 6);
    a_pop(&tst, true, 11, 5);
    a_token_push(&tst, gen_token(14, 7), true);
    a_token_push(&tst, gen_token(15, 7), true);
    a_token_push(&tst, gen_token(16, 7), true);
    a_pop(&tst, true, 16, 7);
    a_pop(&tst, true, 15, 7);
    a_pop(&tst, true, 14, 7);
    a_pop(&tst, true, 10, 5);
    a_pop(&tst, true, 9, 5);
    a_restore(&tst, true);
    a_pop(&tst, true, 10, 5);
    a_pop(&tst, true, 9, 5);
    a_pop(&tst, true, 8, 4);
    a_pop(&tst, true, 7, 4);
    a_level_push(&tst, true);
    a_token_push(&tst, gen_token(20, 9), true);
    a_token_push(&tst, gen_token(21, 9), true);
    a_token_push(&tst, gen_token(22, 9), true);
    a_level_pop(&tst, true);
    a_token_push(&tst, gen_token(14, 8), true);
    a_token_push(&tst, gen_token(15, 8), true);
    a_token_push(&tst, gen_token(16, 8), true);
    a_pop(&tst, true, 16, 8);
    a_pop(&tst, true, 15, 8);
    a_pop(&tst, true, 14, 8);
    a_pop(&tst, true, 6, 4);
    a_pop(&tst, true, 5, 4);
    a_pop(&tst, true, 4, 2);
    a_save(&tst);
    a_pop(&tst, true, 3, 1);
    a_pop(&tst, true, 2, 1);
    a_pop(&tst, true, 1, 1);
    a_pop(&tst, true, 0, 1);
    a_pop(&tst, true, 2, 1);
    a_pop(&tst, true, 4, 1);
    a_restore(&tst, true);
    a_pop(&tst, true, 3, 1);
    a_pop(&tst, true, 2, 1);
    a_pop(&tst, true, 1, 1);
    a_pop(&tst, true, 0, 1);
    a_pop(&tst, true, 2, 1);
    a_save(&tst);
    a_pop(&tst, true, 4, 1);
    a_pop(&tst, true, 6, 1);
    a_pop(&tst, true, 8, 1);
    a_pop(&tst, false, 0, 0);
    a_pop(&tst, false, 0, 0);
    a_restore(&tst, true);
    a_pop(&tst, true, 4, 1);
    a_pop(&tst, true, 6, 1);
    a_pop(&tst, true, 8, 1);
    
    tst_free(&tst);
    tknr_free(&tknr);
    
    printf("%zu/%zu successes\n", successes, total);
}
