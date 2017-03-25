#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "stringbuilder.h"

const ERROR SB_CTOR_BAD_CAP_FAIL       = 7011;
const ERROR SB_CTOR_MALLOC_FAIL        = 7012;
const ERROR SB_APND_MULT_OVERFLOW_FAIL = 7021;
const ERROR SB_APND_MALLOC_FAIL        = 7022;
const ERROR SB_TS_REALLOC_FAIL         = 7031;

ERROR sb_new(size_t init_cap, struct StringBuilder *into) {
    if (init_cap == 0) {
        return SB_CTOR_BAD_CAP_FAIL;
    }
    char *mem = malloc(init_cap);
    if (!mem) {
        return SB_CTOR_MALLOC_FAIL;
    }
    into->mem = mem;
    into->mem[0] = 0;
    into->cap = init_cap;
    into->count = 0;
    return NO_ERROR;
}

bool mult_overflows(size_t a, size_t b) {
    return b > SIZE_MAX / a;
}

#define LOAD_FACTOR 2
ERROR sb_append(struct StringBuilder *to, char c) {
    if (to->count + 1 == to->cap) {
        if (mult_overflows(to->cap, LOAD_FACTOR)) {
            return SB_APND_MULT_OVERFLOW_FAIL;
        }
        char *new_mem = realloc(to->mem, to->cap * LOAD_FACTOR);
        if (!new_mem) {
            return SB_APND_MALLOC_FAIL;
        }
        to->mem = new_mem;
        to->cap *= LOAD_FACTOR;
    }
    to->mem[to->count] = c;
    to->mem[to->count + 1] = '\0';
    ++to->count;
    return NO_ERROR;
}

ERROR sb_into_string(struct StringBuilder *sb, char **into) {
    char *ret = realloc(sb->mem, sb->count + 1);
    if (!ret) {
        return SB_TS_REALLOC_FAIL;
    }
    ret[sb->count] = 0;
    *sb = (struct StringBuilder) {
            .mem = NULL,
            .cap = 0,
            .count = 0
    };
    *into = ret;
    return NO_ERROR;
}

void sb_free(struct StringBuilder *sb) {
    free(sb->mem);
    sb->mem = NULL;
}