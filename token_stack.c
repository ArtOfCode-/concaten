#include "token_stack.h"

struct PushableTokenizer ptknr_new(const struct Tokenizer from) {
    return (struct PushableTokenizer) {
            .head = NULL,
            .tknr = from
    };
}

void tst_level_node_free(struct TS_LevelNode *this) {
    if (this) {
        struct TS_TokenNode *node = this->token_head;
        while (node) {
            struct TS_TokenNode *next = node->next;
            tkn_free(&node->value);
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
    struct TS_ChangeNode *pushing = malloc(sizeof(*pushing));
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
            .ptknr = ptknr_new(this),
            .tracking_changes = false,
            .latest_change = NULL,
    };
    tst_push_level(into);
    return NO_ERROR;
}

ERROR tst_push(struct TokenStack *this, const struct Token pushing) {
    if (!this->level_head) tst_push_level(this);
    struct TS_TokenNode *pushing_node = malloc(sizeof(*pushing_node));
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
        struct TS_TokenNode *old = level->token_head;
        level->token_head = old->next;
        free(old);
    } else if (this->ptknr.head) {
        ret_val = this->ptknr.head->value;
        struct TS_TokenNode *old = this->ptknr.head;
        this->ptknr.head = old->next;
        free(old);
    } else {
        if (tknr_next(&this->ptknr.tknr, &ret_val) != NO_ERROR) {
            return TST_POP_EMPTY_FAIL;
        }
    }
    if (this->tracking_changes) {
        struct Token copy;
        if (tkn_copy(ret_val, &copy) != NO_ERROR) {
            return TST_POP_SAVE_FAIL;
        }
        if (!tst_push_change(this, (struct TS_ChangeNode) {
                .type = level ? TSCN_TOKEN_POP : TSCN_TOKEN_NEXT,
                .data.popped = copy
        })) {
            return TST_POP_SAVE_FAIL;
        }
    }
    *ret = ret_val;
    return NO_ERROR;
}

ERROR tst_peek(struct TokenStack *this, struct Token *out) {
    struct TS_LevelNode *lh_cur = this->level_head;
    // find the first level with tokens
    while (lh_cur && !lh_cur->token_head) lh_cur = lh_cur->next;
    if (lh_cur) {
        // then some tokens are on the stack; return one
        *out = lh_cur->token_head->value;
        return NO_ERROR;
    }
    // no pushed tokens -- let's see if the tokenizer has any
    if (this->ptknr.head) {
        // then there's something in the ptknr's stack to return
        *out = this->ptknr.head->value;
        return NO_ERROR;
    }
    // we gotta pull something from the tokenizer
    struct TS_TokenNode *nd_pushing = malloc(sizeof(*nd_pushing));
    if (!nd_pushing) {
        return TST_PEEK_MALLOC_FAIL;
    }
    struct Token next_token;
    if (tknr_next(&this->ptknr.tknr, &next_token) != NO_ERROR) {
        free(nd_pushing);
        return TST_PEEK_EMPTY_FAIL;
    }
    *nd_pushing = (struct TS_TokenNode) {
            .value = next_token,
            .next = this->ptknr.head
    };
    this->ptknr.head = nd_pushing;
    *out = next_token;
    return NO_ERROR;
}

ERROR tst_push_level(struct TokenStack *this) {
    struct TS_LevelNode *pushing = malloc(sizeof(*pushing));
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
        struct Token popped;
        struct TS_TokenNode *new_head;
        // errors are determined by the action we're trying to reverse, not
        // the action we take to reverse it
        switch (todo->type) {
            case TSCN_TOKEN_POP:
                if (tst_push(this, todo->data.popped) != NO_ERROR) {
                    return TST_RSTR_POP_FAIL;
                }
                break;
            case TSCN_TOKEN_PUSH:
                if (tst_pop(this, &popped) != NO_ERROR) {
                    return TST_RSTR_PUSH_FAIL;
                } else {
                    tkn_free(&popped);
                }
                break;
            case TSCN_TOKEN_NEXT:
                new_head = malloc(sizeof(*new_head));
                *new_head = (struct TS_TokenNode) {
                        .next = this->ptknr.head,
                        .value = todo->data.popped
                };
                this->ptknr.head = new_head;
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
                tkn_free(&inner->value);
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
    struct TS_TokenNode *ptknr_head = this->ptknr.head;
    while (ptknr_head) {
        tkn_free(&ptknr_head->value);
        struct TS_TokenNode *next = ptknr_head->next;
        free(ptknr_head);
        ptknr_head = next;
    }
}
