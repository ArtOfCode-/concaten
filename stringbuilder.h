#ifndef CONCATEN_STRINGBUILDER_H
#define CONCATEN_STRINGBUILDER_H

#include <stddef.h>
#include <stdbool.h>

struct stringbuilder_s;
typedef struct stringbuilder_s *stringbuilder_t;
stringbuilder_t sb_new(size_t);
bool sb_append(stringbuilder_t, char);
char *sb_as_string(stringbuilder_t);

#endif //CONCATEN_STRINGBUILDER_H
