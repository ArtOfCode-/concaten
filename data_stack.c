#include "data_stack.h"

#include <stdlib.h>

void dst_node_init(struct DST_Node *in, struct Object *value, struct DST_Node *next) {
    in = malloc(sizeof(struct DST_Node));
    *in = (struct DST_Node) {
            .refcount = 1,
            .next = next,
            .value = value
    };
}

void dst_node_claim(struct DST_Node *claiming) {
    ++claiming->refcount;
}

void dst_node_free(struct DST_Node *freeing) {
    --freeing->refcount;
    if (freeing->refcount == 0) {
        ctno_free(freeing->value);
        dst_node_free(freeing->next);
    }
}

struct DataStack dst_new() {
    return (struct DataStack) {
            .head = NULL
    };
}

// whenever the LL's head changes, the old one's refcount decreases, and the new one's
// increases, and also whenever a node is created that points to any other node, the
// other's increases, and when a node is permanently destroyed (i.e. refcount hits 0),
// the node it points to has its refcount go down by one.

bool dst_push(struct DataStack *dst, struct Object *pushing) {
    dst->head = dst_node_init(dst->head, pushing, dst->head);
}

struct Object *dst_pop(struct DataStack *dst) {
    if (!dst->head) return NULL;
    struct Object *ret = dst->head->value;
    struct DST_Node *next = dst->head->next;
    dst->head = next;
    return ret;
}

struct DataStack dst_copy(const struct DataStack copying) {
    struct DataStack ret = (struct DataStack) {
            .head = copying.head
    };
    ++ret.head->refcount;
    return ret;
}

void dst_free(struct DataStack *freeing) {
    struct DST_Node *cur = freeing->head;
    while (cur) {
        node_free(cur);
        cur = cur->next;
    }
}