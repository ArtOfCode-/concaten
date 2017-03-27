#include "data_stack.h"

#include <stdlib.h>

const ERROR DST_PUSH_MALLOC_FAIL = 2001;
const ERROR DST_POP_EMPTY_FAIL   = 2002;

void dst_node_free(struct DST_Node *freeing) {
    if (freeing) {
        --freeing->refcount;
        if (freeing->refcount == 0) {
            dst_node_free(freeing->next);
        }
    }
}

ERROR dst_new(struct DataStack *into) {
    *into = (struct DataStack) {
            .head = NULL
    };
    return NO_ERROR;
}

ERROR dst_push(struct DataStack *dst, struct Object *pushing) {
    struct DST_Node *val = malloc(sizeof(*val));
    if (!val) {
        return DST_PUSH_MALLOC_FAIL;
    }
    *val = (struct DST_Node) {
            .refcount = 1, // claimed for the stack by default
            .next = dst->head,
            .value = pushing
    };
    // claim, then immediately free, current head
    dst->head = val;
    return NO_ERROR;
}

ERROR dst_pop(struct DataStack *dst, struct Object **into) {
    if (!dst->head) return DST_POP_EMPTY_FAIL;
    struct Object *ret = dst->head->value;
    struct DST_Node *next = dst->head->next;
    ++next->refcount;
    dst_node_free(dst->head);
    dst->head = next;
    if (into) *into = ret;
    return NO_ERROR;
}

ERROR dst_copy(const struct DataStack copying, struct DataStack *into) {
    struct DataStack ret = (struct DataStack) {
            .head = copying.head
    };
    ++ret.head->refcount;
    *into = ret;
    return NO_ERROR;
}

void dst_free(struct DataStack *freeing) {
    struct DST_Node *cur = freeing->head;
    while (cur) {
        struct DST_Node *next = cur->next;
        dst_node_free(cur);
        cur = next;
    }
    freeing->head = cur;
}
