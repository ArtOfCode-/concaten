#ifndef CONCATEN_TYPE_H
#define CONCATEN_TYPE_H

#include "object.h"

struct Type {
    char **mthds;
    size_t mthds_count;
    size_t *mthd_lens;
};

extern const struct Type MATCH_ALL;

ERROR tp_new(char **, size_t, struct Type *);
ERROR tp_copy(const struct Type, struct Type *);
bool tp_matches(const struct Type, const struct Object);
bool tp_eq(const struct Type, const struct Type);
void tp_free(struct Type *);

#endif //CONCATEN_TYPE_H
