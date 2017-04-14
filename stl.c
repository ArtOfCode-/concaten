#include "stl.h"
#include "method_map.h"

ERROR init_stl() {
    ERROR err;
    err = init_integral_methods();
    if (err != NO_ERROR) return err;
    err = init_string_methods();
    if (err != NO_ERROR) {
        mm_free(integral_methods);
        return err;
    }
    return NO_ERROR;
}
