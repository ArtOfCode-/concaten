#ifndef CONCATEN_CODE_BLOCK_H
#define CONCATEN_CODE_BLOCK_H

#include "tokenizer.h"

struct CodeBlock {
    struct Token *tokens;
    size_t count;
    size_t cap;
};

bool cb_new(size_t, struct CodeBlock *);
struct CodeBlock cb_copy(const struct CodeBlock);
void cb_append(struct CodeBlock *, struct Token);
// no cb_push_to(CodeBlock, TokenStack) b/c that's in Runnable
// TODO all the array functions, too
void cb_free(struct CodeBlock *);

#endif //CONCATEN_CODE_BLOCK_H