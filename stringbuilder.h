#ifndef CONCATEN_STRINGBUILDER_H
#define CONCATEN_STRINGBUILDER_H

#include <stddef.h>
#include <stdbool.h>

// TODO No pointers to objects; does bad things with `const` apparently
struct StringBuilder;
struct StringBuilder sb_new(size_t);
bool sb_append(struct StringBuilder, char);
char *sb_as_string(struct StringBuilder);
char *sb_free_copy(struct StringBuilder);
size_t sb_size(struct StringBuilder);
void sb_free(struct StringBuilder);

#endif //CONCATEN_STRINGBUILDER_H
