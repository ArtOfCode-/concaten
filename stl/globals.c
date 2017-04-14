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
#define try_add_ctn(name, id, ...) do { \
    struct CodeBlock cb_##id;
    try()

ERROR init_globals() {
    global_funcs = malloc(sizeof(*global_funcs));
    if (!global_funcs) {
        return STL_GLB_INIT_MALLOC_FAIL;
    }
    
    return NO_ERROR;
error_handler:;
    mm_free(global_funcs);
    return err;
}
