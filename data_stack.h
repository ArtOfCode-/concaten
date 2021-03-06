#ifndef CONCATEN_DATA_STACK_H
#define CONCATEN_DATA_STACK_H

#include <stddef.h>
#include <stdbool.h>

#include "error.h"

// linked-list has O(1) push/pop always, and can be copied in O(1)
// (as long as it's not a deep copy!)

struct DST_Node {
    struct DST_Node *next;
    struct Object *value;
    size_t refcount;
};

struct DataStack {
    struct DST_Node *head;
};

struct Object;
ERROR dst_new(struct DataStack *);
ERROR dst_copy(const struct DataStack, struct DataStack *);
ERROR dst_push(struct DataStack *, struct Object *);
ERROR dst_pop(struct DataStack *, struct Object **);
ERROR dst_peek(struct DataStack *, struct Object **);
bool dst_empty(const struct DataStack);
void dst_free(struct DataStack *);

#endif //CONCATEN_DATA_STACK_H
