#include "code_block.h"

#include <stdlib.h>
#include <stdint.h>

ERROR cb_new(size_t init_cap, struct CodeBlock *ret) {
    struct Token *mem = malloc(sizeof(*mem) * init_cap);
    if (!mem) { return CB_CTOR_MALLOC_FAIL; }
    *ret = (struct CodeBlock) {
            .cap = init_cap,
            .count = 0,
            .tokens = mem
    };
    return NO_ERROR;
}

ERROR cb_copy(const struct CodeBlock from, struct CodeBlock *into) {
    struct Token *mem = malloc(sizeof(*mem) * from.cap);
    if (!mem) { return CB_COPY_MALLOC_FAIL; }
    for (size_t i = 0; i < from.cap; ++i) {
        mem[i] = from.tokens[i];
    }
    *into = (struct CodeBlock) {
            .cap = from.cap,
            .count = from.cap,
            .tokens = mem
    };
    return NO_ERROR;
}

ERROR cb_from_tkns(size_t amnt, struct Token *tkns, struct CodeBlock *into) {
    struct Token *tkns_c = malloc(sizeof(*tkns_c) * amnt);
    if (!tkns_c) return CB_FROM_TKNS_MALLOC_FAIL;
    for (size_t i = 0; i < amnt; ++i) {
        if (tkn_copy(tkns[i], &tkns_c[i]) != NO_ERROR) {
            for (; i > 0; --i) {
                tkn_free(&tkns_c[i]);
            }
            tkn_free(&tkns_c[0]);
            free(tkns_c);
            return CB_FROM_TKNS_TKN_COPY_FAIL;
        }
    }
    *into = (struct CodeBlock) {
            .cap = amnt,
            .count = amnt,
            .tokens = tkns_c
    };
    return NO_ERROR;
}

// non-public method; shouldn't get a public ERROR for itself
bool cb_expand(struct CodeBlock *cb) {
    if (cb->cap > SIZE_MAX / 2) { return false; }
    struct Token *mem = realloc(cb->tokens, sizeof(*mem) * cb->cap * 2);
    if (!mem) { return false; }
    cb->tokens = mem;
    cb->cap *= 2;
    return true;
}

ERROR cb_append(struct CodeBlock *to, const struct Token adding) {
    if (to->count == to->cap && !cb_expand(to)) {
        return CB_APND_EXPAND_FAIL;
    }
    to->tokens[to->count++] = adding;
    return NO_ERROR;
}

ERROR cb_insert(struct CodeBlock *into, size_t pos, struct Token what) {
    if (pos >= into->count) { return CB_INST_BAD_IDX_FAIL; }
    if (into->count == into->cap && !cb_expand(into)) {
        return CB_INST_EXPAND_FAIL;
    }
    for (size_t i = into->count; i > pos; --i) {
        into->tokens[i] = into->tokens[i - 1];
    }
    into->tokens[pos] = what;
    ++into->count;
    return NO_ERROR;
}

ERROR cb_remove(struct CodeBlock *from, size_t pos) {
    if (pos >= from->count) { return CB_RMV_BAD_IDX_FAIL; }
    for (size_t i = pos; i < from->count; ++i) {
        from->tokens[i] = from->tokens[i + 1];
    }
    --from->count;
    return NO_ERROR;
}

ERROR cb_prepend(struct CodeBlock *into, struct Token what) {
    if (into->count == into->cap && !cb_expand(into)) {
        return CB_PREP_EXPAND_FAIL;
    }
    for (size_t i = into->count; i > 0; --i) {
        into->tokens[i] = into->tokens[i - 1];
    }
    into->tokens[0] = what;
    ++into->count;
    return NO_ERROR;
}

ERROR cb_get(const struct CodeBlock from, size_t idx, struct Token *into) {
    if (idx >= from.count) { return CB_GET_BAD_IDX_FAIL; }
    if (into) { *into = from.tokens[idx]; }
    return NO_ERROR;
}

ERROR cb_set(struct CodeBlock *in, size_t idx, struct Token new_val) {
    if (idx >= in->count) { return CB_SET_BAD_IDX_FAIL; }
    in->tokens[idx] = new_val;
    return NO_ERROR;
}

void cb_free(struct CodeBlock *freeing) {
    free(freeing->tokens);
    *freeing = (struct CodeBlock) {
            .cap = 0,
            .count = 0,
            .tokens = NULL
    };
}
