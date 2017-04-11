#include "runnable.h"

ERROR rn_from_ctn(const struct CodeBlock from, struct Runnable *into) {
    struct CodeBlock copy;
    if (cb_copy(from, &copy) != NO_ERROR) {
        return RN_NEW_CB_COPY_FAIL;
    }
    *into = (struct Runnable) {
            .code.ctn = copy,
            .is_c = false
    };
    return NO_ERROR;
}

ERROR rn_from_c(CFuncType fptr, struct Runnable *into) {
    *into = (struct Runnable) {
            .code.c = fptr,
            .is_c = true
    };
    return NO_ERROR;
}

ERROR rn_copy(const struct Runnable from, struct Runnable *into) {
    if (from.is_c) {
        *into = (struct Runnable) {
                .is_c = true,
                .code.c = from.code.c
        };
        return NO_ERROR;
    } else {
        struct CodeBlock copy;
        if (cb_copy(from.code.ctn, &copy) != NO_ERROR) {
            return RN_COPY_CB_COPY_FAIL;
        }
        *into = (struct Runnable) {
                .is_c = false,
                .code.ctn = copy
        };
        return NO_ERROR;
    }
}

bool rn_eq(const struct Runnable lhs, const struct Runnable rhs) {
    if (lhs.is_c != rhs.is_c) {
        return false;
    } else if (lhs.is_c) {
        return lhs.code.c == rhs.code.c;
    } else {
        for (size_t i = 0; i < lhs.code.ctn.count; ++i) {
            struct Token lt = lhs.code.ctn.tokens[i];
            struct Token rt = rhs.code.ctn.tokens[i];
            if (!tkn_eq(lt, rt)) return false;
        }
        return true;
    }
}

ERROR rn_run(const struct Runnable run, struct DataStack *ds,
             struct ScopeStack *ss, struct TokenStack *ts) {
    if (run.is_c) {
        return run.code.c(ds, ss, ts);
    } else {
        tst_push_level(ts);
        for (size_t i = run.code.ctn.count; i > 0; --i) {
            if (tst_push(ts, run.code.ctn.tokens[i - 1]) != NO_ERROR) {
                return RN_RUN_TST_PUSH_FAIL;
            }
        }
        return NO_ERROR;
    }
}

void rn_free(struct Runnable *rn) {
    if (!rn->is_c) {
        cb_free(&rn->code.ctn);
    }
}
