#ifndef CONCATEN_TOKEN_STACK_H
#define CONCATEN_TOKEN_STACK_H

#include <stdlib.h>

#include "tokenizer.h"

struct TS_LevelNode;
struct TS_ChangeNode;
struct TokenStack {
    struct TS_LevelNode *level_head;
    struct Tokenizer tknr;
    struct TS_ChangeNode *latest_change;
    bool tracking_changes;
};

ERROR tst_new(const struct Tokenizer, struct TokenStack *);
ERROR tst_push(struct TokenStack *, const struct Token);
ERROR tst_pop(struct TokenStack *, struct Token *);
ERROR tst_peek(const struct TokenStack, struct Token *);
ERROR tst_push_level(struct TokenStack *);
ERROR tst_pop_level(struct TokenStack *);
ERROR tst_save_state(struct TokenStack *);
ERROR tst_restore_state(struct TokenStack *);
void tst_free(struct TokenStack *);

#endif //CONCATEN_TOKEN_STACK_H
