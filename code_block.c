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

bool cb_expand(struct CodeBlock *cb) {
    size_t new_cap = cb->cap * 2 * sizeof(struct Token);
    struct Token *new_mem = realloc(cb->tokens, new_cap);
    if (!new_mem) return false;
    cb->tokens = new_mem;
    cb->cap *= 2;
    return true;
}

bool cb_append(struct CodeBlock *to, const struct Token adding) {
    if (to->count == to->cap && !cb_expand(to)) {
        return false;
    }
    to->tokens[to->count++] = adding;
    return true;
}

bool cb_insert(struct CodeBlock *into, size_t pos, struct Token what) {
    if (into->count == into->cap && !cb_expand(into)) {
        return false;
    }
    for (size_t i = into->count; i > pos; --i) {
        into->tokens[i] = into->tokens[i - 1];
    }
    into->tokens[pos] = what;
    ++into->count;
    return true;
}

bool cb_remove(struct CodeBlock *from, size_t pos) {
    for (size_t i = pos; i < from->count; ++i) {
        from->tokens[i] = from->tokens[i + 1];
    }
    --from->count;
    return true;
}

bool cb_prepend(struct CodeBlock *to, struct Token what) {
    return cb_insert(to, 0, what);
}

bool cb_get(const struct CodeBlock from, size_t idx, struct Token *into) {
    if (idx >= from.count) return false;
    if (into) *into = from.tokens[idx];
    return true;
}

bool cb_set(struct CodeBlock *in, size_t idx, struct Token new_val) {
    if (idx >= in->count) return false;
    in->tokens[idx] = new_val;
    return true;
}

void cb_free(struct CodeBlock *freeing) {
    free(freeing->tokens);
    freeing->tokens = NULL;
}
