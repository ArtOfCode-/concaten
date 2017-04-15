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
ERROR sst_new(size_t, struct ScopeStack *);
ERROR sst_save_state(struct ScopeStack *);
ERROR sst_restore_state(struct ScopeStack *);
ERROR sst_get(const struct ScopeStack, const char *, struct Runnable *);
ERROR sst_get_all(const struct ScopeStack, const char *,
                 struct Runnable **, size_t *);
ERROR sst_set(struct ScopeStack *, const char *, struct Runnable);
ERROR sst_redefine(struct ScopeStack *, const char *, struct Runnable);
ERROR sst_push_scope(struct ScopeStack *);
ERROR sst_pop_scope(struct ScopeStack *);
void sst_free(struct ScopeStack *);

#endif //CONCATEN_SCOPE_STACK_H
