#include <stdlib.h>
#include <string.h>
#include "ctn_object.h"

struct Object ctno_literal(const void *data, size_t data_size, struct MethodMap *methods) {
    void *new_data = malloc(data_size);
    if (!new_data) {
        return (struct Object) { .error = 1 };
    }
    memcpy(new_data, data, data_size);
    return (struct Object) {
            .data.literal.size = data_size,
            .data.literal.value = new_data,
            .is_literal = true,
            .methods = methods,
            .error = 0
    };
}

struct Object ctno_dynamic(struct PropMap pm, struct MethodMap *methods) {
    struct PropMap copy = pm_copy(pm);
    return (struct Object) {
            .data.properties = copy,
            .is_literal = false,
            .methods = methods,
            .error = 0
    };
}
