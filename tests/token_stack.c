#include "../test.h"

#include "../token_stack.h"

#include <string.h>

struct Token gen_token(size_t num, size_t layer_num) {
    char *raw_mem = malloc(14);
    memcpy(raw_mem, "gen_token-res", 14);
    char *origin_mem = malloc(12);
    sprintf(origin_mem, "layer_%zu", layer_num);
    return (struct Token) {
            .line = layer_num,
            .index = num,
            .origin = origin_mem,
            .origin_len = strlen(origin_mem) + 1,
            .raw = raw_mem,
            .raw_len = strlen(raw_mem) + 1,
            .type = TKN_WORD
    };
}

static size_t total = 0;
static size_t successes = 0;

void a_token_push(struct TokenStack *tst, struct Token pushing, ERROR_VAL res) {
    ERROR prev_err;
    if (FAILED(tst_push(tst, pushing))) {
        tassert(prev_err.errcode == res, "got wrong error code when pushing");
    } else if (res) {
        tassert(false, "successfully pushed when failure expected");
    }
}

void a_token_pop(struct TokenStack *tst, ERROR_VAL res, size_t i, size_t l) {
    ERROR prev_err;
    struct Token popped;
    if (FAILED(tst_pop(tst, &popped))) { 
        tassert(prev_err.errcode == res, "got wrong error code");
        return;
    }
    tassert(popped.index == i && popped.line == l,
            "wrong element popped (%zu %zu, not %zu %zu)",
            popped.index, popped.line, i, l);
    tkn_free(&popped);
}

void a_level_push(struct TokenStack *tst, ERROR_VAL expected) {
    ERROR prev_err;
    if (FAILED(tst_push_level(tst))) {
        tassert(prev_err.errcode == expected, "got wrong error code");
        return;
    }
    tassert(expected == 0, "success occurred when not expected");
}

void a_level_pop(struct TokenStack *tst, ERROR_VAL expected) {
    ERROR prev_err;
    if (FAILED(tst_pop_level(tst))) {
        tassert(prev_err.errcode == expected, "got wrong error code");
        return;
    }
    tassert(expected == 0, "unexpectedly succeeded. yay?");
}

void a_restore(struct TokenStack *tst, ERROR_VAL expected) {
    ERROR prev_err;
    if (FAILED(tst_restore_state(tst))) {
        tassert(prev_err.errcode == expected, "got wrong error code");
        return;
    }
    tassert(expected == 0, "unexpectedly succeeded. yay?");
}


struct TestResult test_token_stack() {
    struct Tokenizer tknr;
    tassert(!NOS_FAILED(tknr_from_string("1 2 3 4 5", "mem", &tknr)),
            "Failed to create tokenizer");
    struct TokenStack tst;
    tassert(!NOS_FAILED(tst_new(tknr, &tst)), "failed to create token stack");
    
    a_token_push(&tst, gen_token(1, 1), 0);
    a_token_push(&tst, gen_token(2, 1), 0);
    a_token_push(&tst, gen_token(3, 1), 0);
    a_level_push(&tst, 0);
    a_token_push(&tst, gen_token(4, 2), 0);
    a_level_push(&tst, 0);
    a_level_push(&tst, 0);
    a_token_push(&tst, gen_token(5, 4), 0);
    a_token_push(&tst, gen_token(6, 4), 0);
    a_token_push(&tst, gen_token(7, 4), 0);
    a_token_push(&tst, gen_token(8, 4), 0);
    a_level_push(&tst, 0);
    a_token_push(&tst, gen_token(9, 5), 0);
    a_token_push(&tst, gen_token(10, 5), 0);
    tst_save_state(&tst);
    a_token_push(&tst, gen_token(11, 5), 0);
    a_level_push(&tst, 0);
    a_token_push(&tst, gen_token(12, 6), 0);
    a_token_push(&tst, gen_token(13, 6), 0);
    a_token_pop(&tst, 0, 13, 6);
    a_token_pop(&tst, 0, 12, 6);
    a_token_pop(&tst, 0, 11, 5);
    a_token_push(&tst, gen_token(14, 7), 0);
    a_token_push(&tst, gen_token(15, 7), 0);
    a_token_push(&tst, gen_token(16, 7), 0);
    a_token_pop(&tst, 0, 16, 7);
    a_token_pop(&tst, 0, 15, 7);
    a_token_pop(&tst, 0, 14, 7);
    a_token_pop(&tst, 0, 10, 5);
    a_token_pop(&tst, 0, 9, 5);
    a_restore(&tst, 0);
    a_token_pop(&tst, 0, 10, 5);
    a_token_pop(&tst, 0, 9, 5);
    a_token_pop(&tst, 0, 8, 4);
    a_token_pop(&tst, 0, 7, 4);
    a_level_push(&tst, 0);
    a_token_push(&tst, gen_token(20, 9), 0);
    a_token_push(&tst, gen_token(21, 9), 0);
    a_token_push(&tst, gen_token(22, 9), 0);
    a_level_pop(&tst, 0);
    a_token_push(&tst, gen_token(14, 8), 0);
    a_token_push(&tst, gen_token(15, 8), 0);
    a_token_push(&tst, gen_token(16, 8), 0);
    a_token_pop(&tst, 0, 16, 8);
    a_token_pop(&tst, 0, 15, 8);
    a_token_pop(&tst, 0, 14, 8);
    a_token_pop(&tst, 0, 6, 4);
    a_token_pop(&tst, 0, 5, 4);
    a_token_pop(&tst, 0, 4, 2);
    tst_save_state(&tst);
    a_token_pop(&tst, 0, 3, 1);
    a_token_pop(&tst, 0, 2, 1);
    a_token_pop(&tst, 0, 1, 1);
    a_token_pop(&tst, 0, 0, 1);
    a_token_pop(&tst, 0, 2, 1);
    a_token_pop(&tst, 0, 4, 1);
    a_restore(&tst, 0);
    a_token_pop(&tst, 0, 3, 1);
    a_token_pop(&tst, 0, 2, 1);
    a_token_pop(&tst, 0, 1, 1);
    a_token_pop(&tst, 0, 0, 1);
    a_token_pop(&tst, 0, 2, 1);
    tst_save_state(&tst);
    a_token_pop(&tst, 0, 4, 1);
    a_token_pop(&tst, 0, 6, 1);
    a_token_pop(&tst, 0, 8, 1);
    a_token_pop(&tst, TST_POP_EMPTY_FAIL, 0, 0);
    a_token_pop(&tst, TST_POP_EMPTY_FAIL, 0, 0);
    a_restore(&tst, 0);
    a_token_pop(&tst, 0, 4, 1);
    a_token_pop(&tst, 0, 6, 1);
    a_token_pop(&tst, 0, 8, 1);
    
    tst_free(&tst);
    tknr_free(&tknr);
    
    return (struct TestResult) { .successes = successes, .total = total };
}
