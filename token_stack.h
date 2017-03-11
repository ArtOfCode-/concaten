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

enum TSCN_Type { TSCN_TOKEN_POP, TSCN_TOKEN_PUSH, TSCN_LEVEL_PUSH, TSCN_LEVEL_POP };

struct TS_ChangeNode {
    struct TS_ChangeNode *prev;
//    size_t prev_count;
    enum TSCN_Type type;
    union {
        struct Token popped;//type == TSCN_TOKEN_POP
        struct TS_TokenNode *popped_head;//type == TSCN_LEVEL_POP
    } data;
};

struct TokenStack {
    struct TS_LevelNode *level_head;
    struct Tokenizer tknr;
    bool tknr_ours;
    struct TS_ChangeNode *latest_change;
    bool tracking_changes;
};

struct TokenStack tst_new(const struct Tokenizer);
bool tst_push(struct TokenStack *, const struct Token);
bool tst_pop(struct TokenStack *, struct Token *);
bool tst_peek(const struct TokenStack, struct Token *);
bool tst_push_level(struct TokenStack *);
bool tst_pop_level(struct TokenStack *);
void tst_save_state(struct TokenStack *);
void tst_restore_state(struct TokenStack *);
void tst_free(struct TokenStack *);

#endif //CONCATEN_TOKEN_STACK_H
