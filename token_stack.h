#ifndef CONCATEN_TOKEN_STACK_H
#define CONCATEN_TOKEN_STACK_H

#include <stdlib.h>

#include "tokenizer.h"

struct TS_TokenNode {
    struct TS_TokenNode *next;
    struct Token value;
};

struct TS_LevelNode {
    struct TS_LevelNode *next;
    struct TS_TokenNode *token_head;
};

enum TS_ChangeType {
    TSCN_TOKEN_PUSH, // We pushed a token. `change.data` is unset.
    TSCN_TOKEN_POP, // We popped a token. `change.data.popped` is set.
    TSCN_TOKEN_NEXT, // We read in a token. `change.data.popped` is set.
    TSCN_LEVEL_PUSH, // We pushed a level. `change.data` is unset.
    TSCN_LEVEL_POP // We popped a level. `change.data.popped_head` is set.
};

struct TS_ChangeNode {
    struct TS_ChangeNode *prev;
    enum TS_ChangeType type;
    union {
        struct Token popped;//type == TSCN_TOKEN_POP
        struct TS_TokenNode *popped_head;//type == TSCN_LEVEL_POP
    } data;
};

struct PushableTokenizer {
    struct Tokenizer tknr;
    // ...plus a stack we can put things on
    // strictly speaking, a queue is what we need, but because in this specific
    // case we only need to push items to the back one at a time, and when it's
    // empty, a stack will work.
    struct TS_TokenNode *head;
};

struct PushableTokenizer;
struct TokenStack {
    struct TS_LevelNode *level_head;
    struct PushableTokenizer ptknr;
    struct TS_ChangeNode *latest_change;
    bool tracking_changes;
};

ERROR tst_new(const struct Tokenizer, struct TokenStack *);
ERROR tst_push(struct TokenStack *, const struct Token);
ERROR tst_pop(struct TokenStack *, struct Token *);
ERROR tst_peek(struct TokenStack *, struct Token *);
ERROR tst_push_level(struct TokenStack *);
ERROR tst_pop_level(struct TokenStack *);
ERROR tst_save_state(struct TokenStack *);
ERROR tst_discard_save(struct TokenStack *);
ERROR tst_restore_state(struct TokenStack *);
void tst_free(struct TokenStack *);

#endif //CONCATEN_TOKEN_STACK_H
