#include "token_stack.h"

void tst_token_node_free(struct TS_TokenNode *freeing) {
    if (freeing) {
        --freeing->refcount;
        if (!freeing->refcount) {
            tst_token_node_free(freeing->next);
            free(freeing);
        }
    }
}

void tst_layer_node_free(struct TS_LevelNode *freeing) {
    if (freeing) {
        --freeing->refcount;
        if (!freeing->refcount) {
            tst_token_node_free(freeing->token_head);
            tst_layer_node_free(freeing->next);
            free(freeing);
        }
    }
}

struct TokenStack tst_new(const struct Tokenizer underlying) {
    struct TokenStack ret = (struct TokenStack) {
            .level_head = malloc(sizeof(*ret.level_head)),
            .tknr = underlying,
            .tracking_changes = false,
            .latest_change = NULL
    };
    *ret.level_head = (struct TS_LevelNode) {
            .token_head = NULL,
            .next = NULL,
            .refcount = 1 // claimed by stack
    };
    return ret;
}

bool tst_push(struct TokenStack *to, const struct Token pushing) {
    struct TS_LevelNode *level = to->level_head;
    struct TS_TokenNode *old_head = level->token_head;
    level->token_head = malloc(sizeof(*old_head));
    
}
