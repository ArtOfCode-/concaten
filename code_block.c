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
    
}
