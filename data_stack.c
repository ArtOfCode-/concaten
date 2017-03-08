#include "data_stack.h"

#include <stdlib.h>

void node_free(struct DST_Node *nd) {
    if (nd) {
        --nd->refcount;
        if (nd->refcount == 0) {
            ctno_free(nd->value);
            free(nd); // this is safe because nd is never made except by `malloc` in my code
        }
    }
}

struct DataStack dst_new() {
    return (struct DataStack) {
            .count = 0,
            .head = NULL
    };
}

bool dst_push(struct DataStack *dst, struct Object *pushing) {
    struct DST_Node *adding = malloc(sizeof(*adding));
    if (!adding) return false;
    *adding = (struct DST_Node) {
            .next = dst->head,
            .refcount = 1,
            .value = pushing
    };
    dst->head = adding;
    ++dst->count;
    return true;
}

struct Object *dst_pop(struct DataStack *dst) {
    if (dst->count == 0) return NULL;
    struct Object *ret = dst->head->value;
    node_free(dst->head);
    dst->head = dst->head->next;
    --dst->count;
    return ret;
}

struct DataStack dst_copy(const struct DataStack copying) {
    struct DataStack ret = (struct DataStack) {
            .count = copying.count,
            .head = malloc(sizeof(ret.head))
    };
    *ret.head = (struct DST_Node) {
            .value = copying.head->value,
            .next = copying.head->next,
            .refcount = 1
    };
    return ret;
}

void dst_free(struct DataStack *freeing) {
    struct DST_Node *cur = freeing->head;
    while (cur) {
        node_free(cur);
        cur = cur->next;
    }
}