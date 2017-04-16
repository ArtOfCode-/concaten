#ifndef CONCATEN_RUNNABLE_H
#define CONCATEN_RUNNABLE_H

#include "code_block.h"
#include "data_stack.h"
#include "token_stack.h"

struct ScopeStack;
typedef ERROR (*CFuncType)(struct DataStack *, struct ScopeStack *,
                           struct TokenStack *);

enum RunnableType {
    RNT_C_CODE,
    RNT_CTN_CODE,
    RNT_CTN_PUSHER
};

struct Runnable {
    union {
        struct CodeBlock ctn;
        CFuncType c;
        struct Object *pushing;
    } code;
    enum RunnableType type;
};

ERROR rn_from_ctn(const struct CodeBlock, struct Runnable *);
ERROR rn_from_c(CFuncType fptr, struct Runnable *);
ERROR rn_ctn_pusher(struct Object *pushing, struct Runnable *);
ERROR rn_copy(const struct Runnable, struct Runnable *);
bool rn_eq(const struct Runnable, const struct Runnable);
ERROR rn_run(const struct Runnable, struct DataStack *, struct ScopeStack *,
             struct TokenStack *);
void rn_free(struct Runnable *);

#endif //CONCATEN_RUNNABLE_H
