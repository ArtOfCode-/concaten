#include "token_stack.h"

void tst_token_node_free(struct TS_TokenNode *freeing) {
    while (freeing) {
        --freeing->refcount;
        if (!freeing->refcount) {
            struct TS_TokenNode *next = freeing->next;
            free(freeing);
            freeing = next;
        } else {
            break;
        }
    }
}

void tst_level_node_free(struct TS_LevelNode *freeing) {
    while (freeing) {
        --freeing->refcount;
        if (!freeing->refcount) {
            tst_token_node_free(freeing->token_head);
            struct TS_LevelNode *next = freeing->next;
            free(freeing);
            freeing = next;
        } else {
            break;
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
    struct TokenStack ret = {
            .level_head = malloc(sizeof(struct TS_LevelNode)),
            .tknr = underlying,
            .tknr_ours = false,
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
    if (!tst_push_change(to, (struct TS_ChangeNode) {
            .type = TSCN_TOKEN_PUSH
    })) {
        return false;
    }
    struct TS_LevelNode *level = to->level_head;
    struct TS_TokenNode *old_head = level->token_head;
    level->token_head = malloc(sizeof(struct TS_TokenNode));
    if (!level->token_head ||
            !tst_push_change(to, (struct TS_ChangeNode) { .type = TSCN_TOKEN_PUSH })) {
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

bool tst_pop(struct TokenStack *from, struct Token *ret) {
    struct TS_LevelNode *level = from->level_head;
    while (level && !level->token_head) {
        from->level_head = level->next;
        tst_level_node_free(level);
        level = from->level_head;
    }
    if (!tst_push_change(from, (struct TS_ChangeNode) {
            .type = TSCN_TOKEN_POP
    })) {
        return false;
    }
    if (level) {
        *ret = level->token_head->value;
        struct TS_TokenNode *old_head = level->token_head;
        level->token_head = old_head->next;
        tst_token_node_free(old_head);
    } else {
        if (!tknr_next(&from->tknr, ret)) {
            return false;
        }
    }
    from->latest_change->data.popped = *ret;
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
    if (!tst_push_change(into, (struct TS_ChangeNode) { .type = TSCN_LEVEL_PUSH })) {
        return false;
    }
    struct TS_LevelNode *old_head = into->level_head;
    into->level_head = malloc(sizeof(struct TS_LevelNode));
    if (!into->level_head) {
        into->level_head = old_head;
        return false;
    }
    *into->level_head = (struct TS_LevelNode) {
            .next = old_head,
            .token_head = NULL,
            .refcount = 1
    };
    return true;
}

bool tst_pop_level(struct TokenStack *from) {
    if (!tst_push_change(from, (struct TS_ChangeNode) { .type = TSCN_LEVEL_POP })) {
        return false;
    }
    // TODO level pop -- remember to store nodes (incr. refcount)
}
