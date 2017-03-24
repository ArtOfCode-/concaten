#include "../test.h"

#include "../token_stack.h"

#include <string.h>

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

void a_token_push(struct TokenStack *tst, struct Token pushing, ERROR res) {
    tassert(tst_push(tst, pushing) == res,
            "unexpected result when pushing %zu %zu (%d)",
            pushing.index, pushing.line, !res);
}

void a_token_pop(struct TokenStack *tst, ERROR expected, size_t index, size_t line) {
    struct Token popped;
    ERROR err = tst_pop(tst, &popped);
    tassert(err == expected, "got unexpected return result");
    if (err == NO_ERROR) {
        tassert(popped.index == index && popped.line == line,
                "wrong element popped (%zu %zu, not %zu %zu)",
                popped.index, popped.line, index, line);
        tkn_free(&popped);
    }
}

void a_level_push(struct TokenStack *tst, ERROR expected) {
    tassert(tst_push_level(tst) == expected,
            "unexpected result when pushing level (%d)", !expected);
}

void a_level_pop(struct TokenStack *tst, ERROR expected) {
    tassert(tst_pop_level(tst) == expected,
            "unexpected result when popping level (%d)\n", !expected);
}

void a_restore(struct TokenStack *tst, ERROR expected) {
    tassert(tst_restore_state(tst) == expected,
            "unexpected result when restoring (%d)\n", !expected);
}


struct TestResult test_token_stack() {
    struct Tokenizer tknr;
    tassert(tknr_from_string("1 2 3 4 5", "mem", &tknr) == NO_ERROR,
            "Failed to create tokenizer");
    struct TokenStack tst;
    tassert(tst_new(tknr, &tst) == NO_ERROR, "failed to create token stack");
    
    a_token_push(&tst, gen_token(1, 1), NO_ERROR);
    a_token_push(&tst, gen_token(2, 1), NO_ERROR);
    a_token_push(&tst, gen_token(3, 1), NO_ERROR);
    a_level_push(&tst, NO_ERROR);
    a_token_push(&tst, gen_token(4, 2), NO_ERROR);
    a_level_push(&tst, NO_ERROR);
    a_level_push(&tst, NO_ERROR);
    a_token_push(&tst, gen_token(5, 4), NO_ERROR);
    a_token_push(&tst, gen_token(6, 4), NO_ERROR);
    a_token_push(&tst, gen_token(7, 4), NO_ERROR);
    a_token_push(&tst, gen_token(8, 4), NO_ERROR);
    a_level_push(&tst, NO_ERROR);
    a_token_push(&tst, gen_token(9, 5), NO_ERROR);
    a_token_push(&tst, gen_token(10, 5), NO_ERROR);
    tst_save_state(&tst);
    a_token_push(&tst, gen_token(11, 5), NO_ERROR);
    a_level_push(&tst, NO_ERROR);
    a_token_push(&tst, gen_token(12, 6), NO_ERROR);
    a_token_push(&tst, gen_token(13, 6), NO_ERROR);
    a_token_pop(&tst, NO_ERROR, 13, 6);
    a_token_pop(&tst, NO_ERROR, 12, 6);
    a_token_pop(&tst, NO_ERROR, 11, 5);
    a_token_push(&tst, gen_token(14, 7), NO_ERROR);
    a_token_push(&tst, gen_token(15, 7), NO_ERROR);
    a_token_push(&tst, gen_token(16, 7), NO_ERROR);
    a_token_pop(&tst, NO_ERROR, 16, 7);
    a_token_pop(&tst, NO_ERROR, 15, 7);
    a_token_pop(&tst, NO_ERROR, 14, 7);
    a_token_pop(&tst, NO_ERROR, 10, 5);
    a_token_pop(&tst, NO_ERROR, 9, 5);
    a_restore(&tst, NO_ERROR);
    a_token_pop(&tst, NO_ERROR, 10, 5);
    a_token_pop(&tst, NO_ERROR, 9, 5);
    a_token_pop(&tst, NO_ERROR, 8, 4);
    a_token_pop(&tst, NO_ERROR, 7, 4);
    a_level_push(&tst, NO_ERROR);
    a_token_push(&tst, gen_token(20, 9), NO_ERROR);
    a_token_push(&tst, gen_token(21, 9), NO_ERROR);
    a_token_push(&tst, gen_token(22, 9), NO_ERROR);
    a_level_pop(&tst, NO_ERROR);
    a_token_push(&tst, gen_token(14, 8), NO_ERROR);
    a_token_push(&tst, gen_token(15, 8), NO_ERROR);
    a_token_push(&tst, gen_token(16, 8), NO_ERROR);
    a_token_pop(&tst, NO_ERROR, 16, 8);
    a_token_pop(&tst, NO_ERROR, 15, 8);
    a_token_pop(&tst, NO_ERROR, 14, 8);
    a_token_pop(&tst, NO_ERROR, 6, 4);
    a_token_pop(&tst, NO_ERROR, 5, 4);
    a_token_pop(&tst, NO_ERROR, 4, 2);
    tst_save_state(&tst);
    a_token_pop(&tst, NO_ERROR, 3, 1);
    a_token_pop(&tst, NO_ERROR, 2, 1);
    a_token_pop(&tst, NO_ERROR, 1, 1);
    a_token_pop(&tst, NO_ERROR, 0, 1);
    a_token_pop(&tst, NO_ERROR, 2, 1);
    a_token_pop(&tst, NO_ERROR, 4, 1);
    a_restore(&tst, NO_ERROR);
    a_token_pop(&tst, NO_ERROR, 3, 1);
    a_token_pop(&tst, NO_ERROR, 2, 1);
    a_token_pop(&tst, NO_ERROR, 1, 1);
    a_token_pop(&tst, NO_ERROR, 0, 1);
    a_token_pop(&tst, NO_ERROR, 2, 1);
    tst_save_state(&tst);
    a_token_pop(&tst, NO_ERROR, 4, 1);
    a_token_pop(&tst, NO_ERROR, 6, 1);
    a_token_pop(&tst, NO_ERROR, 8, 1);
    a_token_pop(&tst, TST_POP_EMPTY_FAIL, 0, 0);
    a_token_pop(&tst, TST_POP_EMPTY_FAIL, 0, 0);
    a_restore(&tst, NO_ERROR);
    a_token_pop(&tst, NO_ERROR, 4, 1);
    a_token_pop(&tst, NO_ERROR, 6, 1);
    a_token_pop(&tst, NO_ERROR, 8, 1);
    
    tst_free(&tst);
    tknr_free(&tknr);
    
    return (struct TestResult) { .successes = successes, .total = total };
}
