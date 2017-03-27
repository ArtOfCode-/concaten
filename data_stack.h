#ifndef CONCATEN_DATA_STACK_H
#define CONCATEN_DATA_STACK_H

#include <stddef.h>

#include "error.h"

// linked-list has O(1) push/pop always, and can be copied in O(1) (as long as it's not a deep copy!)

struct Object;
struct DST_Node {
    // we want the contents of _this object_, not necessarily the ones it points to,
    // to be immutable, with the notable exception of the refcount.
    struct DST_Node *next;
    struct Object *value;
    size_t refcount;
};
struct DataStack {
    struct DST_Node *head;
};

ERROR dst_new(struct DataStack *);
ERROR dst_copy(const struct DataStack, struct DataStack *);
ERROR dst_push(struct DataStack *, struct Object *);
ERROR dst_pop(struct DataStack *, struct Object **);
void dst_free(struct DataStack *);

#endif //CONCATEN_DATA_STACK_H
