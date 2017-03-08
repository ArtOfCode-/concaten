#include "data_stack.h"

#include <stdlib.h>

void dst_node_claim(struct DST_Node *claiming) {
    ++claiming->refcount;
}

void dst_node_free(struct DST_Node *freeing) {
    if (freeing) {
        --freeing->refcount;
        if (freeing->refcount == 0) {
            ctno_free(freeing->value);
            dst_node_free(freeing->next);
        }
    }
}

struct DST_Node dst_node_init(struct Object *value, struct DST_Node *next) {
    dst_node_claim(next);
    return (struct DST_Node) {
            .value = ctno_claim(value),
            .next = next,
            .refcount = 0
    };
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
    struct DST_Node *val = malloc(sizeof(*val));
    if (!val) {
        return false;
    }
    *val = (struct DST_Node) {
            .refcount = 1, // claimed for the stack by default
            .next = dst->head,
            .value = pushing
    };
    // head would be claimed by `val`, then released by stack, so refcount stays the same
    dst->head = val;
    return true;
}

struct Object *dst_pop(struct DataStack *dst) {
    if (!dst->head) return NULL;
    struct Object *ret = dst->head->value;
    struct DST_Node *next = dst->head->next;
    // stack is no longer pointing to it; other things might, so it might stay
    dst_node_free(dst->head);
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
        dst_node_free(cur);
        cur = cur->next;
    }
}