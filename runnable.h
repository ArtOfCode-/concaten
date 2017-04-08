#ifndef CONCATEN_RUNNABLE_H
#define CONCATEN_RUNNABLE_H

#include "code_block.h"
#include "data_stack.h"
#include "token_stack.h"

typedef ERROR (*CFuncType)(struct DataStack *, struct TokenStack *);

struct Runnable {
    union {
        struct CodeBlock ctn;
        CFuncType c;
    } code;
    bool is_c;
};

ERROR rn_from_ctn(const struct CodeBlock, struct Runnable *);
ERROR rn_from_c(CFuncType fptr, struct Runnable *);
ERROR rn_copy(const struct Runnable, struct Runnable *);
bool rn_eq(const struct Runnable, const struct Runnable);
ERROR rn_run(const struct Runnable, struct DataStack *, struct TokenStack *);
void rn_free(struct Runnable *);

#endif //CONCATEN_RUNNABLE_H
