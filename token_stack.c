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

bool tst_push_change(struct TokenStack *this,
                     const struct TS_ChangeNode change) {
    if (!this->tracking_changes) {
        return true;
    }
    struct TS_ChangeNode *pushing = malloc(sizeof(struct TS_ChangeNode));
    if (!pushing) {
        return false;
    }
    *pushing = (struct TS_ChangeNode) {
            .type = change.type,
            .data = change.data,
            .prev = this->latest_change
    };
    this->latest_change = pushing;
    return true;
}

struct TokenStack tst_new(const struct Tokenizer this) {
    struct TokenStack ret = {
            .level_head = NULL,
            .tknr = this,
            .tracking_changes = false,
            .latest_change = NULL,
    };
    tst_push_level(&ret);
    return ret;
}

bool tst_push(struct TokenStack *this, const struct Token pushing) {
    if (!this->level_head) tst_push_level(this);
    struct TS_TokenNode *pushing_node = malloc(sizeof(struct TS_TokenNode));
    if (!pushing_node || !tst_push_change(this, (struct TS_ChangeNode) {
            .type = TSCN_TOKEN_PUSH
    })) {
        free(pushing_node);
        return false;
    }
    struct TS_LevelNode *level_head = this->level_head;
    *pushing_node = (struct TS_TokenNode) {
            .value = pushing,
            .next = level_head->token_head
    };
    level_head->token_head = pushing_node;
    return true;
}

bool tst_pop(struct TokenStack *this, struct Token *ret) {
    struct Token ret_val;
    struct TS_LevelNode *level = this->level_head;
    while (level && !level->token_head) {
        struct TS_LevelNode *next = level->next;
        tst_pop_level(this);
        level = next;
    }
    this->level_head = level;
    if (level) {
        ret_val = level->token_head->value;
        struct TS_TokenNode *next = level->token_head->next;
        free(level->token_head);
        level->token_head = next;
    } else {
        if (tknr_next(&this->tknr, &ret_val) != SUCCESS) {
            return false;
        }
    }
    if (!tst_push_change(this, (struct TS_ChangeNode) {
            .type = TSCN_TOKEN_POP, .data.popped = ret_val
    })) {
        return false;
    }
    if (ret) *ret = ret_val;
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
            !tst_push_change(this, (struct TS_ChangeNode) {
                    .type = TSCN_LEVEL_PUSH
            })) {
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
        free(old_head); // NOT the one that frees the children under it
        return true;
    } else {
        tst_level_node_free(old_head);
        return true;
    }
}

void tst_save_state(struct TokenStack *this) {
    this->tracking_changes = true;
}

bool tst_restore_state(struct TokenStack *this) {
    if (!this->tracking_changes) return false;
    this->tracking_changes = false;
    struct TS_ChangeNode *todo = this->latest_change;
    while (todo) {
        switch (todo->type) {
            case TSCN_TOKEN_PUSH:
                if (!tst_pop(this, NULL)) return false;
                break;
            case TSCN_TOKEN_POP:
                if (!tst_push(this, todo->data.popped)) return false;
                break;
            case TSCN_LEVEL_PUSH:
                if (!tst_pop_level(this)) return false;
                break;
            case TSCN_LEVEL_POP:
                if (!tst_push_level(this)) return false;
                this->level_head->token_head = todo->data.popped_head;
                break;
        }
        struct TS_ChangeNode *next = todo->prev;
        free(todo);
        todo = next;
    }
    this->latest_change = NULL;
    return true;
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
        struct TS_ChangeNode *prev = change->prev;
        free(change);
        change = prev;
    }
    this->latest_change = NULL;
    struct TS_LevelNode *level = this->level_head;
    while (level) {
        struct TS_LevelNode *next = level->next;
        tst_level_node_free(level);
        level = next;
    }
    this->level_head = NULL;
}
