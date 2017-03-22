#ifndef CONCATEN_CODE_BLOCK_H
#define CONCATEN_CODE_BLOCK_H

#include "tokenizer.h"

struct CodeBlock {
    struct Token *tokens;
    size_t count;
    size_t cap;
};

ERROR cb_new(size_t, struct CodeBlock *);
ERROR cb_copy(const struct CodeBlock, struct CodeBlock *);
ERROR cb_append(struct CodeBlock *, const struct Token);
// no cb_push_to(CodeBlock, TokenStack) b/c that's in Runnable
ERROR cb_insert(struct CodeBlock *, size_t, struct Token);
ERROR cb_remove(struct CodeBlock *, size_t);
ERROR cb_prepend(struct CodeBlock *, struct Token);
ERROR cb_get(const struct CodeBlock, size_t, struct Token *);
ERROR cb_set(struct CodeBlock *, size_t, struct Token);
void cb_free(struct CodeBlock *);

#endif //CONCATEN_CODE_BLOCK_H