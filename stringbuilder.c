#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "stringbuilder.h"

struct StringBuilder sb_new() {
    return (struct StringBuilder) {
            .cap = 0,
            .count = 0,
            .mem = NULL
    };
}
#define DEFAULT_INIT_CAP 16
bool sb_init(struct StringBuilder *initing, size_t init_cap) {
    if (init_cap == 0) {
        init_cap = DEFAULT_INIT_CAP;
    }
    char *mem = malloc(init_cap);
    if (!mem) {
        return false;
    }
    initing->mem = mem;
    initing->mem[0] = 0;
    initing->cap = init_cap;
    initing->count = 0;
    return true;
}

bool mult_overflows(size_t a, size_t b) {
    return b > SIZE_MAX / a;
}
#define LOAD_FACTOR 2
bool sb_append(struct StringBuilder *to, char c) {
    if (to->count + 1 == to->cap) {
        if (mult_overflows(to->cap, LOAD_FACTOR)) {
            return false;
        }
        char *new_mem = realloc(to->mem, to->cap * LOAD_FACTOR);
        if (!new_mem) {
            return false;
        }
        to->mem = new_mem;
        to->cap *= LOAD_FACTOR;
    }
    to->mem[to->count] = c;
    to->mem[to->count + 1] = '\0';
    ++to->count;
    return true;
}
char *sb_as_string(struct StringBuilder sb) {
    return sb.mem;
}
char *sb_into_string(struct StringBuilder sb) {
    char *ret = malloc(sb.count + 1);
    if (!ret) {
        return NULL;
    }
    memcpy(ret, sb.mem, sb.count);
    ret[sb.count] = 0;
    sb_free(&sb);
    return ret;
}
size_t sb_size(struct StringBuilder sb) {
    return sb.count;
}
void sb_free(struct StringBuilder *sb) {
    free(sb->mem);
    sb->mem = NULL;
}