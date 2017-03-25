#include "token_stack.h"

const ERROR TST_PUSH_MALLOC_FAIL = 3001;
const ERROR TST_PUSH_SAVE_FAIL = 3002;
const ERROR TST_POP_EMPTY_FAIL = 3003;
const ERROR TST_POP_SAVE_FAIL = 3004;
const ERROR TST_PEEK_EMPTY_FAIL = 3005;
const ERROR TST_PUSH_LEVEL_MALLOC_FAIL = 3006;
const ERROR TST_PUSH_LEVEL_SAVE_FAIL = 3007;
const ERROR TST_POP_LEVEL_EMPTY_FAIL = 3008;
const ERROR TST_POP_LEVEL_SAVE_FAIL = 3009;
const ERROR TST_RSTR_NOT_SAVING_FAIL = 3010;
const ERROR TST_RSTR_POP_FAIL = 3011;
const ERROR TST_RSTR_PUSH_FAIL = 3012;
const ERROR TST_RSTR_POP_LEVEL_FAIL = 3013;
const ERROR TST_RSTR_PUSH_LEVEL_FAIL = 3014;

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

ERROR tst_new(const struct Tokenizer this, struct TokenStack *into) {
    *into = (struct TokenStack) {
            .level_head = NULL,
            .tknr = this,
            .tracking_changes = false,
            .latest_change = NULL,
    };
    tst_push_level(into);
    return NO_ERROR;
}

ERROR tst_push(struct TokenStack *this, const struct Token pushing) {
    if (!this->level_head) tst_push_level(this);
    struct TS_TokenNode *pushing_node = malloc(sizeof(struct TS_TokenNode));
    if (!pushing_node) {
        return TST_PUSH_MALLOC_FAIL;
    }
    if (!tst_push_change(this, (struct TS_ChangeNode) {
            .type = TSCN_TOKEN_PUSH
    })) {
        free(pushing_node);
        return TST_PUSH_SAVE_FAIL;
    }
    struct TS_LevelNode *level_head = this->level_head;
    *pushing_node = (struct TS_TokenNode) {
            .value = pushing,
            .next = level_head->token_head
    };
    level_head->token_head = pushing_node;
    return NO_ERROR;
}

ERROR tst_pop(struct TokenStack *this, struct Token *ret) {
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
        if (tknr_next(&this->tknr, &ret_val) != NO_ERROR) {
            return TST_POP_EMPTY_FAIL;
        }
    }
    if (this->tracking_changes) {
        struct Token copy;
        if (tkn_copy(ret_val, &copy) != NO_ERROR) {
            return TST_POP_SAVE_FAIL;
        }
        if (!tst_push_change(this, (struct TS_ChangeNode) {
                .type = TSCN_TOKEN_POP, .data.popped = copy
        })) {
            return TST_POP_SAVE_FAIL;
        }
    }
    if (ret) *ret = ret_val;
    return NO_ERROR;
}

ERROR tst_peek(const struct TokenStack this, struct Token *ret) {
    if (!this.level_head || !this.level_head->token_head) {
        return TST_PEEK_EMPTY_FAIL;
    } else {
        *ret = this.level_head->token_head->value;
        return NO_ERROR;
    }
}

ERROR tst_push_level(struct TokenStack *this) {
    struct TS_LevelNode *pushing = malloc(sizeof(struct TS_LevelNode));
    if (!pushing) {
        return TST_PUSH_LEVEL_MALLOC_FAIL;
    }
    if (!tst_push_change(this, (struct TS_ChangeNode) {
            .type = TSCN_LEVEL_PUSH
    })) {
        free(pushing);
        return TST_PUSH_LEVEL_SAVE_FAIL;
    }
    *pushing = (struct TS_LevelNode) {
            .token_head = NULL,
            .next = this->level_head
    };
    this->level_head = pushing;
    return NO_ERROR;
}

ERROR tst_pop_level(struct TokenStack *this) {
    if (!this->level_head) return TST_POP_LEVEL_EMPTY_FAIL;
    struct TS_LevelNode *old_head = this->level_head;
    this->level_head = old_head->next;
    if (this->tracking_changes) {
        if (!tst_push_change(this, (struct TS_ChangeNode) {
                .type = TSCN_LEVEL_POP,
                .data.popped_head = old_head->token_head
        })) {
            this->level_head = old_head;
            return TST_POP_LEVEL_SAVE_FAIL;
        }
        free(old_head); // NOT the one that frees the children under it
        return NO_ERROR;
    } else {
        tst_level_node_free(old_head);
        return NO_ERROR;
    }
}

ERROR tst_save_state(struct TokenStack *this) {
    this->tracking_changes = true;
    return NO_ERROR;
}

ERROR tst_restore_state(struct TokenStack *this) {
    if (!this->tracking_changes) return TST_RSTR_NOT_SAVING_FAIL;
    this->tracking_changes = false;
    struct TS_ChangeNode *todo = this->latest_change;
    while (todo) {
        // errors are determined by the action we're trying to reverse
        switch (todo->type) {
            case TSCN_TOKEN_POP:
                if (tst_push(this, todo->data.popped) != NO_ERROR) {
                    return TST_RSTR_POP_FAIL;
                }
                break;
            case TSCN_TOKEN_PUSH:
                if (tst_pop(this, NULL) != NO_ERROR) {
                    return TST_RSTR_PUSH_FAIL;
                }
                break;
            case TSCN_LEVEL_POP:
                if (tst_push_level(this) != NO_ERROR) {
                    return TST_RSTR_POP_LEVEL_FAIL;
                }
                this->level_head->token_head = todo->data.popped_head;
                break;
            case TSCN_LEVEL_PUSH:
                if (tst_pop_level(this) != NO_ERROR) {
                    return TST_RSTR_PUSH_LEVEL_FAIL;
                }
                break;
        }
        struct TS_ChangeNode *next = todo->prev;
        free(todo);
        todo = next;
    }
    this->latest_change = NULL;
    return NO_ERROR;
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
