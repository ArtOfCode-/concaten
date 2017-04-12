#ifndef CONCATEN_TYPE_H
#define CONCATEN_TYPE_H

#include "object.h"

struct Type {
    char **mthds;
    size_t mthds_count;
    size_t *mthd_lens;
};

ERROR tp_new(char **, size_t, size_t *);
ERROR tp_cp(const struct Type, struct Type *);
ERROR tp_matches(const struct Type, const struct Object);
void tp_free(struct Type *);

#endif //CONCATEN_TYPE_H
