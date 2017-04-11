#include "scope_stack.h"

bool ss_push_change(struct ScopeStack *this, struct SS_ChangeNode cn) {
    if (!this->saving_state) return true;
    struct SS_ChangeNode *pushing = malloc(sizeof(*pushing));
    if (!pushing) return false;
    *pushing = cn;
    pushing->next = this->change_head;
    this->change_head = pushing;
    return true;
}

ERROR ss_new(size_t init_cap, struct ScopeStack *out) {
    struct MethodMap *layer_space = malloc(sizeof(*layer_space) * init_cap);
    if (!layer_space) {
        return SST_CTOR_MALLOC_FAIL;
    }
    struct ScopeStack ret = (struct ScopeStack) {
            .count = 0,
            .cap = init_cap,
            .change_head = NULL,
            .saving_state = false,
            .layers = layer_space
    };
    ss_push_scope(&ret);
    *out = ret;
    return NO_ERROR;
}

ERROR ss_save_state(struct ScopeStack *this) {
    this->saving_state = true;
    return NO_ERROR;
}

ERROR ss_restore_state(struct ScopeStack *this) {
    struct SS_ChangeNode *cn = this->change_head;
    while (cn) {
        switch (cn->type) {
            case SSCN_SCOPE_POP:
                if (ss_push_scope(this) != NO_ERROR) {
                    return SST_RST_POP_SCOPE_FAIL;
                }
                break;
            case SSCN_SCOPE_PUSH:
                if (ss_pop_scope(this) != NO_ERROR) {
                    return SST_RST_PUSH_SCOPE_FAIL;
                }
                break;
            case SSCN_SET:
                if (mm_set(&this->layers[cn->set_data.layer_idx],
                           cn->set_data.key, cn->set_data.val) != NO_ERROR) {
                    return SST_RST_SET_FAIL;
                }
                break;
            case SSCN_REDEFINE:
                
            default:
                return SST_RST_UNKNOWN_TYPE_FAIL;
        }
        struct SS_ChangeNode *next = cn->next;
        free(cn);
        cn = next;
    }
    return NO_ERROR;
}

ERROR ss_get(const struct ScopeStack this, const char *const key,
             struct Runnable *out) {
    for (size_t i = this.count; i > 0; --i) {
        ERROR err = mm_get(this.layers[i-1], key, out);
        if (err == NO_ERROR) {
            return NO_ERROR;
        } else if (err == MM_GET_NO_KEY_FAIL) {
            continue;
        } else {
            return SST_GET_MM_GET_FAIL;
        }
    }
    return SST_GET_NO_KEY_FAIL;
}

ERROR ss_get_all(const struct ScopeStack this, const char *const key,
                 struct Runnable **out, size_t *out_size) {
    *out = malloc(sizeof(**out) * this.count);
    if (!*out) {
        return SST_GET_ALL_MALLOC_FAIL;
    }
    size_t cidx = 0;
    for (size_t i = this.count; i > 0; --i) {
        ERROR err = mm_get(this.layers[i-1], key, &(*out)[cidx]);
        if (err == NO_ERROR) {
            ++cidx;
        } else if (err == MM_GET_NO_KEY_FAIL) {
            continue;
        } else {
            free(out);
            return SST_GET_ALL_MM_GET_FAIL;
        }
    }
    *out_size = cidx;
    return NO_ERROR;
}

ERROR ss_set(struct ScopeStack *this, const char *const key,
             const struct Runnable val) {
    struct MethodMap *top = &this->layers[this->count - 1];
    struct Runnable old_val;
    bool old_val_pres;
    if (mm_set(top, key, val) != NO_ERROR) {
        return SST_SET_MM_SET_FAIL;
    }
    if (!ss_push_change(this, (struct SS_ChangeNode) {
            .type = SSCN_SET,
            .set_data = {
                    .layer_idx = this->count - 1,
                    .val = 
            }
    }));
    return NO_ERROR;
}

ERROR ss_push_scope(struct ScopeStack *this) {
    if (this->cap == this->count) {
        struct MethodMap *new = realloc(this->layers,
                                        sizeof(*new) * this->cap * 2);
        if (!new) {
            return SST_PUSH_SCOPE_MALLOC_FAIL;
        }
        this->layers = new;
        this->cap *= 2;
    }
    if (mm_new(16, &this->layers[this->count]) != NO_ERROR) {
        return SST_PUSH_SCOPE_MM_NEW_FAIL;
    }
    ++this->count;
    return NO_ERROR;
}

ERROR ss_redefine(struct ScopeStack *this, const char *const key,
                  struct Runnable val) {
    for (size_t i = this->count; i > 0; --i) {
        if (mm_is_key(this->layers[i-1], key)) {
            if (mm_set(&this->layers[i-1], key, val) == NO_ERROR) {
                return NO_ERROR;
            } else {
                return SST_REDF_MM_SET_FAIL;
            }
        }
    }
    return SST_REDF_NO_KEY_FAIL;
}

ERROR ss_pop_scope(struct ScopeStack *this) {
    if (this->count == 1) { //intentionally _not_ 0
        return SST_POP_SCOPE_AT_BOTTOM_FAIL;
    }
    mm_free(&this->layers[--this->count]);
    return NO_ERROR;
}

void ss_free(struct ScopeStack *this) {
    for (size_t i = 0; i < this->count; ++i) {
        mm_free(&this->layers[i]);
    }
}
