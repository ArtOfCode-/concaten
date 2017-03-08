#ifndef CONCATEN_DATA_STACK_H
#define CONCATEN_DATA_STACK_H

#include "object.h"

// linked-list has O(1) push/pop always, and can be copied in O(1) (as long as it's not a deep copy!)

struct DST_Node {
    // we want the contents of _this object_, not necessarily the ones it points to,
    // to be immutable, with the notable exception of the refcount.
    struct DST_Node *const next;
    struct Object *const value;
    size_t refcount;
};
struct DataStack {
    struct DST_Node *head;
};

struct DataStack dst_new();
bool dst_push(struct DataStack *, struct Object *);
struct Object *dst_pop(struct DataStack *);
struct DataStack dst_copy(const struct DataStack);
void dst_free(struct DataStack *);

#endif //CONCATEN_DATA_STACK_H
