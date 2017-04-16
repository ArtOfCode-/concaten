#include <math.h>
#include "../stl.h"
#include "../ctno_ctors.h"

ERROR real_to_s_d(struct DataStack *d, struct ScopeStack *s,
                      struct TokenStack *t) {
    (void) s;
    (void) t;
    struct Object *top;
    if (dst_pop(d, &top) != NO_ERROR) {
        return ARGUMENT_MISMATCH_FAIL;
    }
    real *val = ctno_to(top, real);
    if (!val) {
        return ARGUMENT_MISMATCH_FAIL;
    }
    int len = snprintf(NULL, 0, "%f", *val);
    if (len < 0) { return STL_INT_STR_GET_SIZE_FAIL; }
    size_t len_st = ((size_t) len) + 1;
    char *str_ver = malloc(len_st);
    if (!str_ver) { return STL_INT_STR_MALLOC_FAIL; }
    if (snprintf(str_ver, len_st, "%f", *val) < 0) {
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

ERROR real_add(struct DataStack *d, struct ScopeStack *s,
               struct TokenStack *t) {
    (void) s;
    (void) t;
    struct Object *top_o, *second_o;
    if (dst_pop(d, &top_o) != NO_ERROR) {
        return ARGUMENT_MISMATCH_FAIL;
    }
    real *top_v = ctno_to(top_o, real);
    if (!top_v) {
        return ARGUMENT_MISMATCH_FAIL;
    }
    if (dst_pop(d, &second_o) != NO_ERROR) {
        return ARGUMENT_MISMATCH_FAIL;
    }
    struct Object *res_o = malloc(sizeof(*res_o));
    if (second_o->is_literal) {
        if (second_o->data.literal.type == LTL_integral) {
            integral *second_v = ctno_to(second_o, integral);
            real res = *second_v+ *top_v;
            ctno_free(top_o);
            ctno_free(second_o);
            if (!res_o) {
                return STL_CTNO_NEW_FAIL;
            }
            if (mm_claim(real_methods) != NO_ERROR) {
                free(res_o);
                return STL_MM_CLAIM_FAIL;
            }
            if (ctno_literal(&res, sizeof(res), LTL_real, real_methods,
                             res_o) != NO_ERROR) {
                free(res_o);
                mm_free(real_methods);
                return STL_CTNO_NEW_FAIL;
            }
        } else if (second_o->data.literal.type == LTL_real) {
            real *second_v = ctno_to(second_o, real);
            real res = *second_v+ *top_v;
            ctno_free(top_o);
            ctno_free(second_o);
            if (!res_o) {
                return STL_CTNO_NEW_FAIL;
            }
            if (mm_claim(real_methods) != NO_ERROR) {
                free(res_o);
                return STL_MM_CLAIM_FAIL;
            }
            if (ctno_literal(&res, sizeof(res), LTL_real, real_methods,
                             res_o) != NO_ERROR) {
                free(res_o);
                mm_free(real_methods);
                return STL_CTNO_NEW_FAIL;
            }
        }
    } else {
        return ARGUMENT_MISMATCH_FAIL;
    }
    if (dst_push(d, res_o) != NO_ERROR) {
        ctno_free(res_o);
        mm_free(real_methods);
        return STL_INT_ADD_PUSH_FAIL;
    }
    return NO_ERROR;
}

ERROR real_sub(struct DataStack *d, struct ScopeStack *s,
                   struct TokenStack *t) {
    (void) s;
    (void) t;
    struct Object *top_o, *second_o;
    if (dst_pop(d, &top_o) != NO_ERROR) {
        return ARGUMENT_MISMATCH_FAIL;
    }
    real *top_v = ctno_to(top_o, real);
    if (!top_v) {
        return ARGUMENT_MISMATCH_FAIL;
    }
    if (dst_pop(d, &second_o) != NO_ERROR) {
        return ARGUMENT_MISMATCH_FAIL;
    }
    struct Object *res_o = malloc(sizeof(*res_o));
    if (second_o->is_literal) {
        if (second_o->data.literal.type == LTL_integral) {
            integral *second_v = ctno_to(second_o, integral);
            real res = *second_v- *top_v;
            ctno_free(top_o);
            ctno_free(second_o);
            if (!res_o) {
                return STL_CTNO_NEW_FAIL;
            }
            if (ctno_from_real(res, res_o) != NO_ERROR) {
                free(res_o);
                mm_free(real_methods);
                return STL_CTNO_NEW_FAIL;
            }
        } else if (second_o->data.literal.type == LTL_real) {
            real *second_v = ctno_to(second_o, real);
            real res = *second_v- *top_v;
            ctno_free(top_o);
            ctno_free(second_o);
            if (!res_o) {
                return STL_CTNO_NEW_FAIL;
            }
            if (ctno_from_real(res, res_o) != NO_ERROR) {
                free(res_o);
                mm_free(real_methods);
                return STL_CTNO_NEW_FAIL;
            }
        } else {
            return ARGUMENT_MISMATCH_FAIL;
        }
    } else {
        return ARGUMENT_MISMATCH_FAIL;
    }
    if (dst_push(d, res_o) != NO_ERROR) {
        ctno_free(res_o);
        mm_free(real_methods);
        return STL_INT_ADD_PUSH_FAIL;
    }
    return NO_ERROR;
}

ERROR real_mul(struct DataStack *d, struct ScopeStack *s,
                   struct TokenStack *t) {
    (void) s;
    (void) t;
    struct Object *top_o, *second_o;
    if (dst_pop(d, &top_o) != NO_ERROR) {
        return ARGUMENT_MISMATCH_FAIL;
    }
    real *top_v = ctno_to(top_o, real);
    if (!top_v) {
        return ARGUMENT_MISMATCH_FAIL;
    }
    if (dst_pop(d, &second_o) != NO_ERROR) {
        return ARGUMENT_MISMATCH_FAIL;
    }
    struct Object *res_o = malloc(sizeof(*res_o));
    if (second_o->is_literal) {
        if (second_o->data.literal.type == LTL_integral) {
            integral *second_v = ctno_to(second_o, integral);
            real res = *second_v* (*top_v);
            ctno_free(top_o);
            ctno_free(second_o);
            if (!res_o) {
                return STL_CTNO_NEW_FAIL;
            }
            if (ctno_from_real(res, res_o) != NO_ERROR) {
                free(res_o);
                mm_free(real_methods);
                return STL_CTNO_NEW_FAIL;
            }
        } else if (second_o->data.literal.type == LTL_real) {
            real *second_v = ctno_to(second_o, real);
            real res = *second_v* (*top_v);
            ctno_free(top_o);
            ctno_free(second_o);
            if (!res_o) {
                return STL_CTNO_NEW_FAIL;
            }
            if (ctno_from_real(res, res_o) != NO_ERROR) {
                free(res_o);
                mm_free(real_methods);
                return STL_CTNO_NEW_FAIL;
            }
        } else {
            return ARGUMENT_MISMATCH_FAIL;
        }
    } else {
        return ARGUMENT_MISMATCH_FAIL;
    }
    if (dst_push(d, res_o) != NO_ERROR) {
        ctno_free(res_o);
        mm_free(real_methods);
        return STL_INT_ADD_PUSH_FAIL;
    }
    return NO_ERROR;
}

ERROR real_div(struct DataStack *d, struct ScopeStack *s,
                    struct TokenStack *t) {
    (void) s;
    (void) t;
    struct Object *top_o, *second_o;
    if (dst_pop(d, &top_o) != NO_ERROR) {
        return ARGUMENT_MISMATCH_FAIL;
    }
    real *top_v = ctno_to(top_o, real);
    if (!top_v) {
        return ARGUMENT_MISMATCH_FAIL;
    }
    if (dst_pop(d, &second_o) != NO_ERROR) {
        return ARGUMENT_MISMATCH_FAIL;
    }
    struct Object *res_o = malloc(sizeof(*res_o));
    real res;
    if (second_o->is_literal) {
        if (second_o->data.literal.type == LTL_integral) {
            integral *second_v = ctno_to(second_o, integral);
            if (*second_v == 0) {
                return STL_DIV_BY_ZERO_FAIL;
            }
            res = *second_v/ (real) *top_v;
        } else if (second_o->data.literal.type == LTL_real) {
            real *second_v = ctno_to(second_o, real);
            if (*second_v == 0) {
                return STL_DIV_BY_ZERO_FAIL;
            }
            res = *second_v/ *top_v;
        } else {
            return ARGUMENT_MISMATCH_FAIL;
        }
    } else {
        return ARGUMENT_MISMATCH_FAIL;
    }
    ctno_free(top_o);
    ctno_free(second_o);
    if (!res_o) {
        return STL_CTNO_NEW_FAIL;
    }
    if (ctno_from_real(res, res_o) != NO_ERROR) {
        free(res_o);
        mm_free(real_methods);
        return STL_CTNO_NEW_FAIL;
    }
    if (dst_push(d, res_o) != NO_ERROR) {
        ctno_free(res_o);
        mm_free(real_methods);
        return STL_INT_ADD_PUSH_FAIL;
    }
    return NO_ERROR;
}

real real_euclid_mod(real a, real b)
{
  real r = fmod(a, b);
  return r >= 0 ? r : r + fabs(b);
}

ERROR real_mod(struct DataStack *d, struct ScopeStack *s,
                    struct TokenStack *t) {
    (void) s;
    (void) t;
    struct Object *top_o, *second_o;
    if (dst_pop(d, &top_o) != NO_ERROR) {
        return ARGUMENT_MISMATCH_FAIL;
    }
    real *top_v = ctno_to(top_o, real);
    if (!top_v) {
        return ARGUMENT_MISMATCH_FAIL;
    }
    if (dst_pop(d, &second_o) != NO_ERROR) {
        return ARGUMENT_MISMATCH_FAIL;
    }
    struct Object *res_o = malloc(sizeof(*res_o));
    real res;
    if (second_o->is_literal) {
        if (second_o->data.literal.type == LTL_integral) {
            integral *second_v = ctno_to(second_o, integral);
            if (*second_v == 0) {
                return STL_DIV_BY_ZERO_FAIL;
            }
            res = real_euclid_mod(*second_v, (real) *top_v);
        } else if (second_o->data.literal.type == LTL_real) {
            real *second_v = ctno_to(second_o, real);
            if (*second_v == 0) {
                return STL_DIV_BY_ZERO_FAIL;
            }
            res = real_euclid_mod(*second_v, *top_v);
        } else {
            return ARGUMENT_MISMATCH_FAIL;
        }
    } else {
        return ARGUMENT_MISMATCH_FAIL;
    }
    ctno_free(top_o);
    ctno_free(second_o);
    if (!res_o) {
        return STL_CTNO_NEW_FAIL;
    }
    if (ctno_from_real(res, res_o) != NO_ERROR) {
        free(res_o);
        mm_free(real_methods);
        return STL_CTNO_NEW_FAIL;
    }
    if (dst_push(d, res_o) != NO_ERROR) {
        ctno_free(res_o);
        mm_free(real_methods);
        return STL_INT_ADD_PUSH_FAIL;
    }
    return NO_ERROR;
}

struct MethodMap *real_methods = NULL;

#define try(expr, code) do {\
    err = (expr); \
    if (err != NO_ERROR) { err = code; goto error_handler; } \
} while (0)
#define try_add_c(name, v) do {\
    struct Runnable rn_##v; \
    try(rn_from_c(v, &rn_##v), STL_INIT_RN_FROM_C_FAIL); \
    try(mm_set(real_methods, name, rn_##v), STL_INIT_MM_SET_FAIL); \
} while (0)

ERROR init_real_methods() {
    ERROR err;
    real_methods = malloc(sizeof(*real_methods));
    if (!real_methods) {
        return STL_INIT_MALLOC_FAIL;
    }
    if (mm_new(16, real_methods) != NO_ERROR) {
        return STL_INIT_MM_NEW_FAIL;
    }
    try_add_c(">string", real_to_s_d);
    try_add_c(">string-d", real_to_s_d);
    try_add_c("+", real_add);
    try_add_c("-", real_sub);
    try_add_c("*", real_mul);
    try_add_c("/", real_div);
    try_add_c("mod", real_mod);
    
    return NO_ERROR;
  error_handler:;
    mm_free(real_methods);
    return err;
}

