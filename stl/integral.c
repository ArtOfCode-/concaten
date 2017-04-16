#include "integral.h"
#include "../method_map.h"
#include "../object.h"
#include "string.h"

ERROR integral_to_s_d(struct DataStack *d, struct ScopeStack *s,
                      struct TokenStack *t) {
    (void) s;
    (void) t;
    struct Object *top;
    if (dst_pop(d, &top) != NO_ERROR) {
        return ARGUMENT_MISMATCH_FAIL;
    }
    integral *val = ctno_to(top, integral);
    if (!val) {
        return ARGUMENT_MISMATCH_FAIL;
    }
    int len = snprintf(NULL, 0, "%lld", *val);
    if (len < 0) { return STL_INT_STR_GET_SIZE_FAIL; }
    size_t len_st = ((size_t) len) + 1;
    char *str_ver = malloc(len_st);
    if (!str_ver) { return STL_INT_STR_MALLOC_FAIL; }
    if (snprintf(str_ver, len_st, "%lld", *val) < 0) {
        free(str_ver);
        ctno_free(top);
        return STL_INT_STR_SNPRINTF_FAIL;
    }
    ctno_free(top);
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
        return STL_CTNO_NEW_FAIL;
    }
    free(str_ver);
    if (dst_push(d, res) != NO_ERROR) {
        ctno_free(res);
        return STL_INT_STR_DSPUSH_FAIL;
    }
    return NO_ERROR;
}

ERROR integral_plus(struct DataStack *d, struct ScopeStack *s,
                    struct TokenStack *t) {
    (void) s;
    (void) t;
    struct Object *top_o, *second_o;
    if (dst_pop(d, &top_o) != NO_ERROR) {
        return ARGUMENT_MISMATCH_FAIL;
    }
    integral *top_v = ctno_to(top_o, integral);
    if (!top_v) {
        return ARGUMENT_MISMATCH_FAIL;
    }
    if (dst_pop(d, &second_o) != NO_ERROR) {
        return ARGUMENT_MISMATCH_FAIL;
    }
    integral *second_v = ctno_to(second_o, integral);
    if (!second_v) {
        return ARGUMENT_MISMATCH_FAIL;
    }
    integral res = *top_v + *second_v;
    ctno_free(top_o);
    ctno_free(second_o);
    struct Object *res_o = malloc(sizeof(*res_o));
    if (!res_o) {
        return STL_CTNO_NEW_FAIL;
    }
    if (mm_claim(integral_methods) != NO_ERROR) {
        free(res_o);
        return STL_MM_CLAIM_FAIL;
    }
    if (ctno_literal(&res, sizeof(res), LTL_integral, integral_methods,
                     res_o) != NO_ERROR) {
        free(res_o);
        mm_free(integral_methods);
        return STL_CTNO_NEW_FAIL;
    }
    if (dst_push(d, res_o) != NO_ERROR) {
        ctno_free(res_o);
        mm_free(integral_methods);
        return STL_INT_ADD_PUSH_FAIL;
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
    try(rn_from_c(v, &rn_##v), STL_INIT_RN_FROM_C_FAIL); \
    try(mm_set(integral_methods, name, rn_##v), STL_INIT_MM_SET_FAIL); \
} while (0)

ERROR init_integral_methods() {
    ERROR err;
    integral_methods = malloc(sizeof(*integral_methods));
    if (!integral_methods) {
        return STL_INIT_MALLOC_FAIL;
    }
    if (mm_new(16, integral_methods) != NO_ERROR) {
        return STL_INIT_MM_NEW_FAIL;
    }
    try_add_c(">string", integral_to_s_d);
    try_add_c(">string-d", integral_to_s_d);
    try_add_c("+", integral_plus);
    
    return NO_ERROR;
  error_handler:;
    mm_free(integral_methods);
    return err;
}

