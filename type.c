#include "type.h"

#include <string.h>

const struct Type MATCH_ALL = {
        .mthds = NULL,
        .mthds_count = 0,
        .mthd_lens = 0
};

ERROR tp_new(char **names, size_t name_count, struct Type *out) {
    char **names_c = malloc(sizeof(*names_c) * name_count);
    if (!names_c) { return TP_CTOR_ARY_MALLOC_FAIL; }
    size_t *lens = malloc(sizeof(*lens) * name_count);
    if (!lens) {
        free(names_c);
        return TP_CTOR_ARY_MALLOC_FAIL;
    }
    for (size_t i = 0; i < name_count; ++i) {
        lens[i] = strlen(names[i]) + 1;
        names_c[i] = malloc(lens[i]);
        if (!names_c[i]) {
            for (; i > 0; --i) {
                free(names_c[i]);
            }
            free(names_c[0]);
            free(names_c);
            free(lens);
            return TP_CTOR_NAME_MALLOC_FAIL;
        }
        strncpy(names_c[i], names[i], lens[i]);
    }
    *out = (struct Type) {
            .mthds = names_c,
            .mthds_count = name_count,
            .mthd_lens = lens
    };
    return NO_ERROR;
}

ERROR tp_copy(const struct Type from, struct Type *into) {
    char **names_c = malloc(sizeof(*names_c) * from.mthds_count);
    if (!names_c) { return TP_COPY_ARY_MALLOC_FAIL; }
    size_t *lens = malloc(sizeof(*lens) * from.mthds_count);
    if (!lens) {
        free(names_c);
        return TP_COPY_ARY_MALLOC_FAIL;
    }
    for (size_t i = 0; i < from.mthds_count; ++i) {
        lens[i] = from.mthd_lens[i];
        names_c[i] = malloc(lens[i]);
        if (!names_c[i]) {
            for (; i > 0; --i) {
                free(names_c[i]);
            }
            free(names_c[0]);
            free(names_c);
            free(lens);
            return TP_COPY_NAME_MALLOC_FAIL;
        }
        strncpy(names_c[i], from.mthds[i], lens[i]);
    }
    *into = (struct Type) {
            .mthds = names_c,
            .mthds_count = from.mthds_count,
            .mthd_lens = lens
    };
    return NO_ERROR;
}

bool tp_matches(const struct Type this, const struct Object *checking) {
    // we're checking that the methods in the type are a subset of the methods
    // that the object has, not vice versa. Having extra methods is fine, but
    // the object has to have all ofthem.
    for (size_t midx = 0; midx < this.mthds_count; ++midx) {
        if (!mm_is_key(*checking->methods, this.mthds[midx])) {
            return false;
        }
    }
    return true;
}

bool tp_eq(const struct Type lhs, const struct Type rhs) {
    if (lhs.mthds_count != rhs.mthds_count) { return false; }
    for (size_t i = 0; i < lhs.mthds_count; ++i) {
        if (lhs.mthd_lens[i] != rhs.mthd_lens[i]) {
            return false;
        }
        if (strncmp(lhs.mthds[i], rhs.mthds[i], lhs.mthd_lens[i]) != 0) {
            return false;
        }
    }
    return true;
}

void tp_free(struct Type *this) {
    free(this->mthd_lens);
    for (size_t i = 0; i < this->mthds_count; ++i) {
        free(this->mthds[i]);
    }
    free(this->mthds);
}
