#include "globals.h"
#include <stdlib.h>
#include <string.h>
#include "../tokenizer.h"
#include "../method_map.h"

struct MethodMap global_funcs;

#define try(expr, code) do {\
    err = (expr); \
    if (err != NO_ERROR) { err = code; goto error_handler; } \
} while (0)
#define try_add_c(name, v) do {\
    struct Runnable rn_##v; \
    try(rn_from_c(v, &rn_##v), STL_INT_INIT_RN_FROM_C_FAIL); \
    try(mm_set(integral_methods, name, rn_##v), STL_INT_INIT_MM_SET_FAIL); \
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
    
    return NO_ERROR;
error_handler:;
    mm_free(&global_funcs);
    return err;
}
