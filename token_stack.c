#include "token_stack.h"

struct TokenStack tst_new(const struct Tokenizer underlying) {
    return (struct TokenStack) {
            .head = NULL,
            .tknr = underlying
    };
}

struct TokenStack tst_copy(const struct TokenStack copying) {
    return (struct TokenStack) {
            .head = copying.head,
            .tknr = tknr_
    }
}

bool tst_push(struct TokenStack *to, struct Token pushing) {
    
}
