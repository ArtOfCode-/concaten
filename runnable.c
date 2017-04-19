#include "runnable.h"
#include "object.h"

ERROR rn_from_ctn(const struct CodeBlock from, struct Runnable *into) {
    struct CodeBlock copy;
    if (cb_copy(from, &copy) != NO_ERROR) {
        return RN_NEW_CB_COPY_FAIL;
    }
    *into = (struct Runnable) {
            .code.ctn = copy,
            .type = RNT_CTN_CODE
    };
    return NO_ERROR;
}

ERROR rn_from_c(CFuncType fptr, struct Runnable *into) {
    *into = (struct Runnable) {
            .code.c = fptr,
            .type = RNT_C_CODE
    };
    return NO_ERROR;
}

ERROR rn_ctn_pusher(struct Object *pushing, struct Runnable *into) {
    if (ctno_claim(pushing) != NO_ERROR) {
        return RN_NEW_CTNO_CLAIM_FAIL;
    }
    *into = (struct Runnable) {
            .code.pushing = pushing,
            .type = RNT_CTN_PUSHER
    };
    return NO_ERROR;
}

ERROR rn_copy(const struct Runnable from, struct Runnable *into) {
    if (from.type == RNT_C_CODE) {
        *into = (struct Runnable) {
                .type = RNT_C_CODE,
                .code.c = from.code.c
        };
        return NO_ERROR;
    } else if (from.type == RNT_CTN_CODE) {
        struct CodeBlock copy;
        if (cb_copy(from.code.ctn, &copy) != NO_ERROR) {
            return RN_COPY_CB_COPY_FAIL;
        }
        *into = (struct Runnable) {
                .type = RNT_CTN_CODE,
                .code.ctn = copy
        };
        return NO_ERROR;
    } else {
        if (ctno_claim(from.code.pushing)) {
            return RN_COPY_CTNO_CLAIM_FAIL;
        }
        *into = (struct Runnable) {
                .code.pushing = from.code.pushing,
                .type = RNT_CTN_PUSHER
        };
        return NO_ERROR;
    }
}

bool rn_eq(const struct Runnable lhs, const struct Runnable rhs) {
    if (lhs.type!= rhs.type) {
        return false;
    } else if (lhs.type == RNT_C_CODE) {
        return lhs.code.c == rhs.code.c;
    } else if (lhs.type == RNT_CTN_CODE) {
        for (size_t i = 0; i < lhs.code.ctn.count; ++i) {
            struct Token lt = lhs.code.ctn.tokens[i];
            struct Token rt = rhs.code.ctn.tokens[i];
            if (!tkn_eq(lt, rt)) return false;
        }
        return true;
    } else {
        return ctno_eq(lhs.code.pushing, rhs.code.pushing);
    }
}

ERROR rn_run(const struct Runnable run, struct DataStack *ds,
             struct ScopeStack *ss, struct TokenStack *ts) {
    if (run.type == RNT_C_CODE) {
        return run.code.c(ds, ss, ts);
    } else if (run.type == RNT_CTN_CODE) {
        if (tst_push_level(ts) != NO_ERROR) return RN_RUN_TST_LEVEL_FAIL;
        for (size_t i = run.code.ctn.count; i > 0; --i) {
            struct Token pushing;
            if (tkn_copy(run.code.ctn.tokens[i - 1], &pushing) != NO_ERROR) {
                return RN_RUN_TKN_COPY_FAIL;
            }
            if (tst_push(ts, pushing) != NO_ERROR) {
                return RN_RUN_TST_PUSH_FAIL;
            }
        }
        return NO_ERROR;
    } else {
        if (dst_push(ds, run.code.pushing) != NO_ERROR) {
            return RN_RUN_DST_PUSH_FAIL;
        }
        return NO_ERROR;
    }
}

void rn_free(struct Runnable *rn) {
    // if it's C, we can't free anything, so don't bother.
    if (rn->type == RNT_CTN_CODE) {
        cb_free(&rn->code.ctn);
    } else if (rn->type == RNT_CTN_PUSHER) {
        ctno_free(rn->code.pushing);
    }
}
