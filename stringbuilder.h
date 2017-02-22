#ifndef CONCATEN_STRINGBUILDER_H
#define CONCATEN_STRINGBUILDER_H

#include <stddef.h>
#include <stdbool.h>

// TODO No pointers to objects; does bad things with `const` apparently
struct StringBuilder;
typedef struct StringBuilder *StringBuilder;
StringBuilder sb_new(size_t);
bool sb_append(StringBuilder, char);
char *sb_as_string(StringBuilder);
char *sb_free_copy(StringBuilder);
size_t sb_size(StringBuilder);
void sb_free(StringBuilder);

#endif //CONCATEN_STRINGBUILDER_H
