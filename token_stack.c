#include "token_stack.h"

void tst_level_node_free(struct TS_LevelNode *freeing) {
    if (freeing) {
        struct TS_TokenNode *node = freeing->token_head;
        while (node) {
            struct TS_TokenNode *next = node->next;
            free(node);
            node = next;
        }
        free(freeing);
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
    struct TokenStack ret = {
            .level_head = malloc(sizeof(struct TS_LevelNode)),
            .tknr = underlying,
            .tknr_ours = false,
            .tracking_changes = false,
            .latest_change = NULL
    };
    *ret.level_head = (struct TS_LevelNode) {
            .token_head = NULL,
            .next = NULL
    };
    return ret;
}

bool tst_push(struct TokenStack *to, const struct Token pushing) {
    if (!to->level_head) tst_push_level(to);
    struct TS_LevelNode *level_head = to->level_head;
    struct TS_TokenNode *pushing_node = malloc(sizeof(struct TS_TokenNode));
    if (!pushing_node || !tst_push_change(to, (struct TS_ChangeNode) {
            .type = TSCN_TOKEN_PUSH
    })) {
        free(pushing_node);
        return false;
    }
    *pushing_node = (struct TS_TokenNode) {
            .value = pushing,
            .next = level_head->token_head
    };
    level_head->token_head = pushing_node;
    return true;
}

bool tst_pop(struct TokenStack *from, struct Token *ret) {
    if (!tst_push_change(from, (struct TS_ChangeNode) { .type = TSCN_TOKEN_POP })) {
        return false;
    }
    struct TS_LevelNode *level = from->level_head;
    while (level && !level->token_head) {
        tst_pop_level(from);
        level = from->level_head;
    }
    if (!level) {
        struct TS_ChangeNode *undoing = from->latest_change;
        from->latest_change = undoing->prev;
        free(undoing); // we know it's a TSCN_TOKEN_POP, so we don't free it specially
        return false;
    }
    if (ret) *ret = level->token_head->value;
    struct TS_TokenNode *old_head = level->token_head;
    level->token_head = old_head->next;
    free(old_head);
    return true;
}

bool tst_peek(const struct TokenStack from, struct Token *ret) {
    if (!from.level_head || !from.level_head->token_head) {
        return false;
    } else {
        *ret = from.level_head->token_head->value;
        return true;
    }
}

bool tst_push_level(struct TokenStack *into) {
    struct TS_LevelNode *pushing = malloc(sizeof(struct TS_LevelNode));
    if (!pushing ||
            !tst_push_change(into, (struct TS_ChangeNode) { .type = TSCN_LEVEL_PUSH })) {
        free(pushing);
        return false;
    }
    *pushing = (struct TS_LevelNode) {
            .token_head = NULL,
            .next = into->level_head
    };
    into->level_head = pushing;
    return true;
}

bool tst_pop_level(struct TokenStack *from) {
    if (!from->level_head) return false;
    struct TS_LevelNode *old_head = from->level_head;
    from->level_head = old_head->next;
    if (from->tracking_changes) {
        if (!tst_push_change(from, (struct TS_ChangeNode) {
                .type = TSCN_LEVEL_POP,
                .data.popped_head = old_head->token_head
        })) {
            from->level_head = old_head;
            return false;
        }
        free(from); // NOT the one that frees the children under it
        return true;
    } else {
        tst_level_node_free(old_head);
        return true;
    }
}

void tst_save_state(struct TokenStack *saving) {
    saving->tracking_changes = true;
}

void tst_restore_state(struct TokenStack *restoring) {
    struct TS_ChangeNode *todo = restoring->latest_change;
    while (todo) {
        switch (todo->type) {
        case TSCN_TOKEN_PUSH:
            tst_pop(restoring, NULL);
            break;
        case TSCN_TOKEN_POP:
            tst_push(restoring, todo->data.popped);
            break;
        case TSCN_LEVEL_PUSH:
            tst_pop_level(restoring);
            break;
        case TSCN_LEVEL_POP:
            tst_push_level(restoring);
            break;
        }
        todo = todo->prev;
    }
}

void tst_free(struct TokenStack *freeing) {
    freeing->tracking_changes = false;
    struct TS_ChangeNode *change = freeing->latest_change;
    while (change) {
        switch (change->type) {
            case TSCN_TOKEN_POP:break;
            case TSCN_TOKEN_PUSH:break;
            case TSCN_LEVEL_PUSH:break;
            case TSCN_LEVEL_POP:break;
        }
    }
    struct TS_LevelNode *level = freeing->level_head;
    while (level) {
        tst_level_node_free(level);
    }
}
