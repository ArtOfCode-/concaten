#include <stdlib.h>
#include "globals.h"
#include "../method_map.h"

struct MethodMap *global_funcs = NULL;

#define try(expr, code) do {\
    err = (expr); \
    if (err != NO_ERROR) { err = code; goto error_handler; } \
} while (0)
#define try_add_c(name, v) do {\
    struct Runnable rn_##v; \
    try(rn_from_c(v, &rn_##v), STL_INT_INIT_RN_FROM_C_FAIL); \
    try(mm_set(integral_methods, name, rn_##v), STL_INT_INIT_MM_SET_FAIL); \
} while (0)
#define tkn(str_form, type) (struct Token)
#define try_add_ctn(name, id, tkn_count, text) do { \
    struct CodeBlock cb_##id; \
    try(cb_new(tkn_count))

ERROR init_globals() {
    global_funcs = malloc(sizeof(*global_funcs));
    if (!global_funcs) {
        return STL_GLB_INIT_MALLOC_FAIL;
    }
    size_t pseudo_line = 0, psuedo_index = 0;
    
    return NO_ERROR;
error_handler:;
    mm_free(global_funcs);
    return err;
}
