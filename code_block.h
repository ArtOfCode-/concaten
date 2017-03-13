#ifndef CONCATEN_CODE_BLOCK_H
#define CONCATEN_CODE_BLOCK_H

#include "tokenizer.h"

struct CodeBlock {
    struct Token *tokens;
    size_t count;
    size_t cap;
};

struct CodeBlock cb_new(size_t init_cap);
struct CodeBlock cb_append(struct CodeBlock *, struct Token);
//TODO etc.

#endif //CONCATEN_CODE_BLOCK_H