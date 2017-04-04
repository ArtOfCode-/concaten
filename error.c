#include <stdlib.h>
#include "error.h"

static const ERROR ERROR_SAVE_FAIL_ERROR = {
        "error.c", 9, 2, NULL
};

const ERROR *err_save(const ERROR from) {
    ERROR *new = malloc(sizeof(*new));
    if (!new) {
        return &ERROR_SAVE_FAIL_ERROR;
    }
    *new = from;
    return new;
}