#ifndef CONCATEN_SCOPE_STACK_H
#define CONCATEN_SCOPE_STACK_H

#include "method_map.h"

struct SS_ChangeNode {
    enum {
        SSCN_SCOPE_PUSH, SSCN_SCOPE_POP, 
    };
};
struct ScopeStack {
    struct MethodMap *head;
    size_t cap;
    size_t count;
    bool saving_state;
    
};
ERROR ss_new(size_t, struct ScopeStack *);
ERROR ss_save_state(struct ScopeStack *);
ERROR ss_restore_state(struct ScopeStack *);
ERROR ss_get(const struct ScopeStack, const char *const, struct Runnable *);
ERROR ss_get_all(const struct ScopeStack, const char *const,
                 struct Runnable **, size_t *);
ERROR ss_set(const char *const, struct Runnable);
void ss_free(struct ScopeStack *);

#endif //CONCATEN_SCOPE_STACK_H
