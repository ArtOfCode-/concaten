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
    static size_t num = 0;
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

void test_token_stack() {
    struct Tokenizer tknr = tknr_from_string("1 2 3 4 5", "mem");
    struct TokenStack tst = tst_new(tknr);
    struct Token popped;
    
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

#define do_pop \
    res = tst_pop(&tst, &popped); \
    printf("pop: %d - %zu-%zu %s %s\n", res, popped.index, popped.line, popped.origin, popped.raw);
    
    int res;
    do_pop
    do_pop
    do_pop
    do_pop
    do_pop
    
    printf("restoring state...");
    tst_restore_state(&tst);
    puts("done");
    
    do_pop
    do_pop
    do_pop
    do_pop
    do_pop
    do_pop
    do_pop
    do_pop
    do_pop
    do_pop
    do_pop
    do_pop
    do_pop
    do_pop
    do_pop
    do_pop
    do_pop
    do_pop
    
    puts("freeing");
    tst_free(&tst);
    tknr_free(&tknr);
    puts("done");
}
