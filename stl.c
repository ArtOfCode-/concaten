#include "stl.h"

void free_stl() {
    mm_free(string_methods);
    mm_free(integral_methods);
    mm_free(real_methods);
    mm_free(&global_funcs);
}

ERROR init_stl(int argc, char **argv) {
    ERROR err;
    err = init_globals(argc, argv);
    if (err != NO_ERROR) {
        return err;
    }
    #define X(name, _) \
        err = init_##name##_methods(); \
        if (err != NO_ERROR) goto error;
//    all_types_X // TODO make this line work
    X(integral, integral)
    X(real, real)
    X(string, _)
    #undef X
    return NO_ERROR;
  error:;
    free_stl();
    return err;
}
