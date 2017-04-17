#include "data_stack.h"
#include "object.h"

#include <stdlib.h>

void dst_node_free(struct DST_Node *nd) {
    while (nd) {
        --nd->refcount;
        if (nd->refcount == 0) {
            struct DST_Node *next = nd->next;
            ctno_free(nd->value);
            free(nd);
            nd = next;
        } else {
            break;
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
    if (ctno_claim(pushing) != NO_ERROR) {
        return DST_PUSH_CLAIM_FAIL;
    }
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
    if (!dst->head) {
        return DST_POP_EMPTY_FAIL;
    }
    struct Object *ret = dst->head->value;
    struct DST_Node *next = dst->head->next;
    // claim for top of stack
    if (next) ++next->refcount;
    dst_node_free(dst->head);
    dst->head = next;
    if (into) *into = ret;
    return NO_ERROR;
}

ERROR dst_peek(struct DataStack *dst, struct Object **into) {
    if (!dst->head) {
        return DST_PEEK_EMPTY_FAIL;
    }
    *into = dst->head->value;
    return NO_ERROR;
}

ERROR dst_copy(const struct DataStack dst, struct DataStack *into) {
    struct DataStack ret = (struct DataStack) {
            .head = dst.head
    };
    if (ret.head) ++ret.head->refcount;
    *into = ret;
    return NO_ERROR;
}

bool dst_empty(const struct DataStack this) {
    return this.head == NULL;
}

void dst_free(struct DataStack *dst) {
    dst_node_free(dst->head);
    dst->head = NULL;
}
