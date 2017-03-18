#include <stdlib.h>
#include "code_block.h"

//const ERROR CODE_BLOCK_FAIL = 1000
  const ERROR CB_CTOR_MALLOC_FAIL = 1001;
  const ERROR CB_COPY_MALLOC_FAIL = 1002;
  const ERROR CB_APND_EXPAND_FAIL = 1003;
//const ERROR CB_INSERT_FAIL = 1010;
  const ERROR CB_INST_BAD_IDX_FAIL = 1011;
  const ERROR CB_INST_EXPAND_FAIL = 1012;
//const ERROR CB_PREPEND_FAIL = 1020;
  const ERROR CB_PREP_EXPAND_FAIL = 1022;
//const ERROR CB_REMOVE_FAIL =31020;
  const ERROR CB_RMV_BAD_IDX_FAIL = 1031;
//const ERROR CB_GET_FAIL = 1040;
  const ERROR CB_GET_BAD_IDX_FAIL = 1041;
//const ERROR CB_SET_FAIL = 1050;
  const ERROR CB_SET_BAD_IDX_FAIL = 1051;

ERROR cb_new(size_t init_cap, struct CodeBlock *ret) {
    struct Token *mem = malloc(init_cap * sizeof(struct Token));
    if (!mem) return CB_CTOR_MALLOC_FAIL;
    *ret = (struct CodeBlock) {
            .cap = init_cap,
            .count = 0,
            .tokens = mem
    };
    return NO_ERROR;
}

ERROR cb_copy(const struct CodeBlock from, struct CodeBlock *into) {
    struct Token *mem = malloc(from.cap * sizeof(struct Token));
    if (!mem) return CB_COPY_MALLOC_FAIL;
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

// non-public method; shouldn't get a public ERROR for itself
bool cb_expand(struct CodeBlock *cb) {
    size_t new_cap = cb->cap * 2 * sizeof(struct Token);
    struct Token *new_mem = realloc(cb->tokens, new_cap);
    if (!new_mem) return false;
    cb->tokens = new_mem;
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
    if (pos >= into->count) return CB_INST_BAD_IDX_FAIL;
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
    if (pos >= from->count) return CB_RMV_BAD_IDX_FAIL;
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
    if (idx >= from.count) return CB_GET_BAD_IDX_FAIL;
    if (into) *into = from.tokens[idx];
    return NO_ERROR;
}

ERROR cb_set(struct CodeBlock *in, size_t idx, struct Token new_val) {
    if (idx >= in->count) return CB_SET_BAD_IDX_FAIL;
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
