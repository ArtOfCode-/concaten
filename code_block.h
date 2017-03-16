#ifndef CONCATEN_CODE_BLOCK_H
#define CONCATEN_CODE_BLOCK_H

#include "tokenizer.h"

struct CodeBlock {
    struct Token *tokens;
    size_t count;
    size_t cap;
};

bool cb_new(size_t, struct CodeBlock *);
bool cb_copy(const struct CodeBlock, struct CodeBlock *);
bool cb_append(struct CodeBlock *, const struct Token);
// no cb_push_to(CodeBlock, TokenStack) b/c that's in Runnable
// TODO all the array functions, too
bool cb_insert(struct CodeBlock *, size_t, struct Token);
bool cb_remove(struct CodeBlock *, size_t);
bool cb_prepend(struct CodeBlock *, struct Token);
bool cb_get(const struct CodeBlock, size_t, struct Token *);
bool cb_set(struct CodeBlock *, size_t, struct Token);
void cb_free(struct CodeBlock *);

#endif //CONCATEN_CODE_BLOCK_H