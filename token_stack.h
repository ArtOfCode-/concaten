#ifndef CONCATEN_TOKEN_STACK_H
#define CONCATEN_TOKEN_STACK_H

#include <stdlib.h>

#include "tokenizer.h"

struct TS_InnerNode {
    struct TS_InnerNode *next;
    struct Token value;
    size_t refcount;
};

struct TS_Node {
    struct TS_Node *next;
    struct TS_InnerNode *inner_head;
    size_t refcount;
};

struct TokenStack {
    struct TS_Node *head;
    struct Tokenizer tknr;
};

struct TokenStack tst_new(const struct Tokenizer);
struct TokenStack tst_copy(const struct TokenStack);
bool tst_push(struct TokenStack *, struct Token);
struct Token *tst_pop(struct TokenStack *);
bool tst_push_layer(struct TokenStack *);
bool tst_pop_layer(struct TokenStack *);
void tst_free(struct TokenStack *);

#endif //CONCATEN_TOKEN_STACK_H
