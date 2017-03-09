#ifndef CONCATEN_TOKEN_STACK_H
#define CONCATEN_TOKEN_STACK_H

#include <stdlib.h>

#include "tokenizer.h"

struct TS_TokenNode {
    struct TS_TokenNode *next;
    struct Token value;
    size_t refcount;
};

struct TS_LevelNode {
    struct TS_LevelNode *next;
    struct TS_TokenNode *token_head;
    size_t refcount;
};

enum TSCN_Type { TSCN_TOKEN_POP, TSCN_TOKEN_PUSH, TSCN_LAYER_PUSH, TSCN_LAYER_POP };

struct TS_ChangeNode {
    struct TS_ChangeNode *prev;
    size_t prev_count;
    enum TSCN_Type type;
    struct Token popped; // only set if type == TSNC_TOKEN_POP, otherwise empty
};

struct TokenStack {
    struct TS_LevelNode *level_head;
    struct Tokenizer tknr;
    struct TS_ChangeNode *latest_change;
    bool tracking_changes;
};

struct TokenStack tst_new(const struct Tokenizer);
void tst_save_state(struct TokenStack *);
void tst_restore_state(struct TokenStack *);
bool tst_push(struct TokenStack *, const struct Token);
struct Token *tst_pop(struct TokenStack *);
bool tst_push_layer(struct TokenStack *);
bool tst_pop_layer(struct TokenStack *);
void tst_free(struct TokenStack *);

#endif //CONCATEN_TOKEN_STACK_H
