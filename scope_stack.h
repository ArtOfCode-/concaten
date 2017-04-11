#ifndef CONCATEN_SCOPE_STACK_H
#define CONCATEN_SCOPE_STACK_H

#include "method_map.h"

enum SS_ChangeType {
    SSCN_SCOPE_PUSH, SSCN_SCOPE_POP, SSCN_SET, SSCN_ADD
};

struct SS_ChangeNode {
    struct SS_ChangeNode *next;
    enum SS_ChangeType type;
    union {
        struct {
            const char *key;
            struct Runnable val;
            size_t layer_idx;
        } single;
        struct MethodMap removed_layer;
    } set_data;
};
struct ScopeStack {
    struct MethodMap *layers;
    size_t cap;
    size_t count;
    bool saving_state;
    struct SS_ChangeNode *change_head;
};
ERROR ss_new(size_t, struct ScopeStack *);
ERROR ss_save_state(struct ScopeStack *);
ERROR ss_restore_state(struct ScopeStack *);
ERROR ss_get(const struct ScopeStack, const char *, struct Runnable *);
ERROR ss_get_all(const struct ScopeStack, const char *,
                 struct Runnable **, size_t *);
ERROR ss_set(struct ScopeStack *, const char *, struct Runnable);
ERROR ss_redefine(struct ScopeStack *, const char *, struct Runnable);
ERROR ss_push_scope(struct ScopeStack *);
ERROR ss_pop_scope(struct ScopeStack *);
void ss_free(struct ScopeStack *);

#endif //CONCATEN_SCOPE_STACK_H
