#ifndef CONCATEN_STRINGBUILDER_H
#define CONCATEN_STRINGBUILDER_H

#include <stddef.h>
#include <stdbool.h>

struct StringBuilder {
    char *mem;
    size_t count;
    size_t cap;
};
struct StringBuilder sb_new();
bool sb_init(struct StringBuilder *, size_t);
bool sb_append(struct StringBuilder *, char);
char *sb_into_string(struct StringBuilder);
void sb_free(struct StringBuilder*);

#endif //CONCATEN_STRINGBUILDER_H
