#include "../stl.h"

ERROR ctn_puts(struct DataStack *d, struct ScopeStack *s, struct TokenStack *t) {
    (void)s;(void)t;
    struct Object *top;
    if (dst_pop(d, &top) != NO_ERROR) {
        // shouldn't ever happen
        return ARGUMENT_MISMATCH_FAIL;
    }
    char *val = ctno_to(top, char);
    if (!val) {
        return ARGUMENT_MISMATCH_FAIL;
    }
    puts(val);
    ctno_free(top);
    return NO_ERROR;
}

ERROR string_to_s(struct DataStack *d, struct ScopeStack *s,
                  struct TokenStack *t) {
    (void)d;(void)s;(void)t; // nop; strings are already strings
    return NO_ERROR;
}

struct MethodMap *string_methods = NULL;

#define try(expr, code) do {\
    err = (expr); \
    if (err != NO_ERROR) { err = code; goto error_handler; } \
} while (0)
#define try_add_c(name, v) do {\
    struct Runnable rn_##v; \
    try(rn_from_c(v, &rn_##v), STL_INIT_RN_FROM_C_FAIL); \
    try(mm_set(string_methods, name, rn_##v), STL_INIT_MM_SET_FAIL); \
} while (0)

ERROR init_string_methods() {
    ERROR err;
    string_methods = malloc(sizeof(*string_methods));
    if (!string_methods) {
        return STL_INIT_MALLOC_FAIL;
    }
    if (mm_new(16, string_methods) != NO_ERROR) {
        return STL_INIT_MM_NEW_FAIL;
    }
    try_add_c("puts", ctn_puts);
    try_add_c(">string", string_to_s);
    
    return NO_ERROR;
error_handler:;
    mm_free(string_methods);
    return err;
}
