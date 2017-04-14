#include "integral.h"
#include "../method_map.h"
#include "../object.h"
#include "string.h"

ERROR to_s_d(struct DataStack *d, struct ScopeStack *s, struct TokenStack *t) {
    (void)s;(void)t;
    struct Object *top;
    if (dst_pop(d, &top) != NO_ERROR) {
        return STL_INT_STR_DSPOP_FAIL;
    }
    if (!top->is_literal || top->data.literal.type != LTL_integral) {
        return ARGUMENT_TYPE_MISMATCH_FAIL;
    }
    integral *val = ctno_to(top, integral);
    if (!val) {
        return STL_INT_STR_TO_LIT_FAIL;
    }
    ctno_free(top);
    int len = snprintf(NULL, 0, "%lld", *val);
    if (len < 0) return STL_INT_STR_GET_SIZE_FAIL;
    size_t len_st = ((size_t) len) + 1;
    char *str_ver = malloc(len_st);
    if (!str_ver) return STL_INT_STR_MALLOC_FAIL;
    if (snprintf(str_ver, len_st, "%lld", *val) < 0) {
        free(str_ver);
        return STL_INT_STR_SNPRINTF_FAIL;
    }
    struct Object *res = malloc(sizeof(*res));
    if (!res) {
        free(str_ver);
        return STL_INT_STR_MALLOC_FAIL;
    }
    if (mm_claim(string_methods) != NO_ERROR) {
        return STL_MM_CLAIM_FAIL;
    }
    if (ctno_literal(str_ver, len_st, LTL_string, string_methods, res)
            != NO_ERROR) {
        free(str_ver);
        return STL_INT_STR_CTNO_NEW_FAIL;
    }
    free(str_ver);
    if (dst_push(d, res) != NO_ERROR) {
        ctno_free(res);
        return STL_INT_STR_DSPUSH_FAIL;
    }
    return NO_ERROR;
}

struct MethodMap *integral_methods = NULL;

#define try(expr, code) do {\
    err = (expr); \
    if (err != NO_ERROR) { err = code; goto error_handler; } \
} while (0)
#define try_add_c(name, v) do {\
    struct Runnable rn_##v; \
    try(rn_from_c(v, &rn_##v), STL_INT_INIT_RN_FROM_C_FAIL); \
    try(mm_set(integral_methods, name, rn_##v), STL_INT_INIT_MM_SET_FAIL); \
} while (0)

ERROR init_integral_methods() {
    ERROR err;
    integral_methods = malloc(sizeof(*integral_methods));
    if (!integral_methods) {
        return STL_INT_INIT_MALLOC_FAIL;
    }
    if (mm_new(16, integral_methods) != NO_ERROR) {
        return STL_INT_INIT_MM_NEW_FAIL;
    }
    try_add_c(">string", to_s_d);
    try_add_c(">string-d", to_s_d);
    return NO_ERROR;
error_handler:;
    mm_free(integral_methods);
    return err;
}

