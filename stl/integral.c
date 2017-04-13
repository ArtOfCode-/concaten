#include "integral.h"
#include "../method_map.h"
#include "../object.h"

ERROR to_s(struct DataStack *d, struct ScopeStack *s, struct TokenStack *t) {
    (void)s;(void)t;
    struct Object *top;
    ERROR pop_err = dst_pop(d, &top);
    if (pop_err != NO_ERROR) {
        return STL_INT_STR_DSPOP_FAIL;
    }
    integral *val = ctno_to(*top, integral);
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
    if (ctno_literal(str_ver, len_st, LTL_string,
            /*/string_methods/*/NULL/**/, res) != NO_ERROR) {
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
ERROR init_integral_methods() {
    
}
