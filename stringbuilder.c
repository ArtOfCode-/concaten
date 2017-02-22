#include <stdlib.h>
#include <string.h>
#include "stringbuilder.h"

struct StringBuilder {
    char *mem;
    size_t count;
    size_t cap;
};
StringBuilder sb_new(size_t init_cap) {
    if (init_cap == 0) {
        return NULL;
    }
    StringBuilder ret = malloc(sizeof(struct StringBuilder));
    if (!ret) return NULL;
    ret->mem = calloc(init_cap, sizeof(char));
    if (!ret->mem) {
        free(ret);
        return NULL;
    }
    ret->cap = init_cap;
    ret->count = 0;
    return ret;
}
#define LOAD_FACTOR 2
bool sb_append(StringBuilder to, char c) {
    if (to->count + 1 == to->cap) {
        char *new_mem = realloc(to->mem, to->cap * LOAD_FACTOR);
        if (!new_mem) {
            return false;
        }
        memset(new_mem + to->cap, 0, to->cap);
        to->mem = new_mem;
        to->cap *= LOAD_FACTOR;
    }
    ++to->count;
    to->mem[to->count - 1] = c;
    return true;
}
char *sb_as_string(StringBuilder sb) {
    return sb->mem;
}
char *sb_free_copy(StringBuilder sb) {
    char *ret = malloc((sb->count + 1) * sizeof(char));
    if (!ret) {
        return NULL;
    }
    strcpy(ret, sb->mem);
    ret[sb->count] = 0;
    sb_free(sb);
    return ret;
}
size_t sb_size(StringBuilder sb) {
    return sb->count;
}
void sb_free(StringBuilder sb) {
    if (sb) {
        if (sb->mem) free(sb->mem);
        free(sb);
    }
}