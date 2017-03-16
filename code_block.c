#include <stdlib.h>
#include "code_block.h"

bool cb_new(size_t init_cap, struct CodeBlock *ret) {
    struct Token *mem = malloc(init_cap * sizeof(struct Token));
    if (!mem) return false;
    *ret = (struct CodeBlock) {
            .cap = init_cap,
            .count = 0,
            .tokens = mem
    };
    return true;
}

bool cb_copy(const struct CodeBlock from, struct CodeBlock *into) {
    struct Token *mem = malloc(from.cap * sizeof(struct Token));
    if (!mem) return false;
    for (size_t i = 0; i < from.cap; ++i) {
        mem[i] = from.tokens[i];
    }
    *into = (struct CodeBlock) {
            .cap = from.cap,
            .count = from.cap,
            .tokens = mem
    };
    return true;
}

bool cb_append(struct CodeBlock *to, const struct Token adding) {
    return false;
}
