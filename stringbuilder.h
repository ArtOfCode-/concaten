#ifndef CONCATEN_STRINGBUILDER_H
#define CONCATEN_STRINGBUILDER_H

#include <stddef.h>
#include <stdbool.h>

#include "error.h"

struct StringBuilder {
    char *mem;
    size_t count;
    size_t cap;
};
ERROR sb_new(size_t, struct StringBuilder *);
ERROR sb_append(struct StringBuilder *, char);
ERROR sb_into_string(struct StringBuilder *, char **);
void sb_free(struct StringBuilder *);

#endif //CONCATEN_STRINGBUILDER_H
