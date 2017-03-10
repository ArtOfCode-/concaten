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

bool tst_push_change(struct TokenStack *tst, const struct TS_ChangeNode change) {
    if (!tst->tracking_changes) return true;
    struct TS_ChangeNode *old_head = tst->latest_change;
    tst->latest_change = malloc(sizeof(struct TS_ChangeNode));
    if (!tst->latest_change) {
        tst->latest_change = old_head;
        return false;
    }
    *tst->latest_change = change;
//    tst->latest_change->prev_count = old_head ? old_head->prev_count + 1 : 0;
    tst->latest_change->prev = old_head;
    return true;
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
    // we could claim then free it, but that really serves no purpose other
    // than wasting cycles.
    struct TS_TokenNode *old_head = level->token_head;
    level->token_head = malloc(sizeof(*old_head));
    if (!level->token_head ||
            !tst_push_change(to, { .type = TSCN_TOKEN_PUSH })) {
        level->token_head = old_head;
        return false;
    }
    *level->token_head = (struct TS_TokenNode) {
            .refcount = 1,
            .next = old_head,
            .value = pushing
    };
    return true;
}

struct Token *tst_pop(struct TokenStack *from) {
    if (from->level_head) {
        
    } else {
        
    }
}
