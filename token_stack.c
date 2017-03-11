#include "token_stack.h"

void tst_level_node_free(struct TS_LevelNode *this) {
    if (this) {
        struct TS_TokenNode *node = this->token_head;
        while (node) {
            struct TS_TokenNode *next = node->next;
            free(node);
            node = next;
        }
        free(this);
    }
}

bool tst_push_change(struct TokenStack *this, const struct TS_ChangeNode change) {
    if (!this->tracking_changes) return true;
    struct TS_ChangeNode *old_head = this->latest_change;
    this->latest_change = malloc(sizeof(struct TS_ChangeNode));
    if (!this->latest_change) {
        this->latest_change = old_head;
        return false;
    }
    *this->latest_change = change;
//    this->latest_change->prev_count = old_head ? old_head->prev_count + 1 : 0;
    this->latest_change->prev = old_head;
    return true;
}

struct TokenStack tst_new(const struct Tokenizer this) {
    struct TokenStack ret = {
            .level_head = malloc(sizeof(struct TS_LevelNode)),
            .tknr = this,
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

bool tst_push(struct TokenStack *this, const struct Token pushing) {
    if (!this->level_head) tst_push_level(this);
    struct TS_LevelNode *level_head = this->level_head;
    struct TS_TokenNode *pushing_node = malloc(sizeof(struct TS_TokenNode));
    if (!pushing_node || !tst_push_change(this, (struct TS_ChangeNode) {
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

bool tst_pop(struct TokenStack *this, struct Token *ret) {
    if (!tst_push_change(this, (struct TS_ChangeNode) { .type = TSCN_TOKEN_POP })) {
        return false;
    }
    struct TS_LevelNode *level = this->level_head;
    while (level && !level->token_head) {
        tst_pop_level(this);
        level = this->level_head;
    }
    if (!level) {
        struct TS_ChangeNode *undoing = this->latest_change;
        this->latest_change = undoing->prev;
        free(undoing); // we know it's a TSCN_TOKEN_POP, so we don't free it specially
        return false;
    }
    if (this->tracking_changes) {
        
    }
    if (ret) *ret = level->token_head->value;
    struct TS_TokenNode *old_head = level->token_head;
    level->token_head = old_head->next;
    free(old_head);
    return true;
}

bool tst_peek(const struct TokenStack this, struct Token *ret) {
    if (!this.level_head || !this.level_head->token_head) {
        return false;
    } else {
        *ret = this.level_head->token_head->value;
        return true;
    }
}

bool tst_push_level(struct TokenStack *this) {
    struct TS_LevelNode *pushing = malloc(sizeof(struct TS_LevelNode));
    if (!pushing ||
            !tst_push_change(this, (struct TS_ChangeNode) { .type = TSCN_LEVEL_PUSH })) {
        free(pushing);
        return false;
    }
    *pushing = (struct TS_LevelNode) {
            .token_head = NULL,
            .next = this->level_head
    };
    this->level_head = pushing;
    return true;
}

bool tst_pop_level(struct TokenStack *this) {
    if (!this->level_head) return false;
    struct TS_LevelNode *old_head = this->level_head;
    this->level_head = old_head->next;
    if (this->tracking_changes) {
        if (!tst_push_change(this, (struct TS_ChangeNode) {
                .type = TSCN_LEVEL_POP,
                .data.popped_head = old_head->token_head
        })) {
            this->level_head = old_head;
            return false;
        }
        free(this); // NOT the one that frees the children under it
        return true;
    } else {
        tst_level_node_free(old_head);
        return true;
    }
}

void tst_save_state(struct TokenStack *this) {
    this->tracking_changes = true;
}

void tst_restore_state(struct TokenStack *this) {
    this->tracking_changes = false;
    struct TS_ChangeNode *todo = this->latest_change;
    while (todo) {
        switch (todo->type) {
            case TSCN_TOKEN_PUSH:
                tst_pop(this, NULL);
                break;
            case TSCN_TOKEN_POP:
                tst_push(this, todo->data.popped);
                break;
            case TSCN_LEVEL_PUSH:
                tst_pop_level(this);
                break;
            case TSCN_LEVEL_POP:
                tst_push_level(this);
                break;
        }
        struct TS_ChangeNode *next = todo->prev;
        free(todo);
        todo = next;
    }
}

void tst_free(struct TokenStack *this) {
    this->tracking_changes = false;
    struct TS_ChangeNode *change = this->latest_change;
    while (change) {
        if (change->type == TSCN_LEVEL_POP) {
            struct TS_TokenNode *inner = change->data.popped_head;
            while (inner) {
                struct TS_TokenNode *next = inner->next;
                free(inner);
                inner = next;
            }
        } else if (change->type == TSCN_TOKEN_POP) {
            tkn_free(&change->data.popped);
        }
        free(change);
    }
    this->latest_change = NULL;
    struct TS_LevelNode *level = this->level_head;
    while (level) {
        struct TS_LevelNode *next = level->next;
        tst_level_node_free(level);
        level = next;
    }
    this->level_head = NULL;
    if (this->tknr_ours) tknr_free(&this->tknr);
}
