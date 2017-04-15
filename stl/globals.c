#include "globals.h"
#include <stdlib.h>
#include <string.h>
#include "../tokenizer.h"
#include "../method_map.h"
#include "../object.h"

ERROR curly(struct DataStack *d, struct ScopeStack *s, struct TokenStack *t) {
    (void) s;
    ERROR err = NO_ERROR;
    size_t curly_count = 1;
    struct CodeBlock ret;
    if (cb_new(16, &ret) != NO_ERROR) {
        return STL_GLB_CURLY_CB_NEW_FAIL;
    }
    while (curly_count != 0) {
        struct Token next;
        if ((err = tst_pop(t, &next)) == NO_ERROR) {
            cb_append(&ret, next);
            if (next.type == TKN_WORD && next.raw_len == 1) {
                if (next.raw[0] == '{') {
                    ++curly_count;
                } else if (next.raw[0] == '}') {
                    --curly_count;
                }
            }
        } else {
            break;
        }
    }
    if (err != NO_ERROR) {
        return err;
    }
    --ret.count;
    struct Object *pushing = malloc(sizeof(*pushing));
    if (!pushing) {
        return STL_GLB_CURLY_MALLOC_FAIL;
    }
    printf("%zu tokens, ending in %s\n", ret.count,
           ret.tokens[ret.count - 1].raw);
    if (ctno_literal(&ret, sizeof(ret), LTL_codeBlock, NULL, pushing) !=
        NO_ERROR) {
        return STL_GLB_CURLY_CTNO_NEW_FAIL;
    }
    cb_free(&ret);
    if (dst_push(d, pushing) != NO_ERROR) {
        return STL_GLB_CURLY_DSPUSH_FAIL;
    }
    return NO_ERROR;
}

struct MethodMap global_funcs;

#define try(expr, code) do {\
    err = (expr); \
    if (err != NO_ERROR) { err = code; goto error_handler; } \
} while (0)
#define try_add_c(name, v) do {\
    struct Runnable rn_##v; \
    try(rn_from_c(v, &rn_##v), STL_INT_INIT_RN_FROM_C_FAIL); \
    try(mm_set(&global_funcs, name, rn_##v), STL_INT_INIT_MM_SET_FAIL); \
} while (0)
// normally, string literals are bad, but the way these specific tokens are
// used means it's fine to put literals, as long as I don't `tkn_free`.
#define tkn(str_form, _type) (struct Token) { \
    .raw = str_form, .raw_len = strlen(str_form), \
    .origin = "<native code>", .origin_len = 13, \
    .type = _type, .line = pseudo_line, \
    .index = pseudo_index += strlen(str_form) + 1, \
}
#define try_add_ctn(name, id, ...) do { \
    ++pseudo_line; \
    struct Token id##_tkns[] = { __VA_ARGS__ }; \
    size_t id##_count = sizeof(id##_tkns) / sizeof(id##_tkns[0]); \
    struct CodeBlock cb_##id; \
    try(cb_from_tkns(id##_count, id##_tkns, &cb_##id), \
        STL_GLB_INIT_CB_NEW_FAIL); \
    struct Runnable rn_##id; \
    try(rn_from_ctn(cb_##id, &rn_##id), STL_GLB_INIT_RN_NEW_FAIL); \
    try(mm_set(&global_funcs, name, rn_##id), STL_GLB_INIT_MM_SET_FAIL); \
} while(0)

ERROR init_globals() {
    if (mm_new(16, &global_funcs) != NO_ERROR) {
        return STL_GLB_INIT_MM_NEW_FAIL;
    }
    size_t pseudo_line = 0, pseudo_index = 0;
    ERROR err;
    try_add_ctn("*puts", any_puts,
                tkn(">string", TKN_WORD), tkn("puts", TKN_WORD));
    try_add_c("{", curly);
    
    return NO_ERROR;
  error_handler:;
    mm_free(&global_funcs);
    return err;
}
