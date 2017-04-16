#include "stl.h"

ERROR init_stl(int argc, char **argv) {
    ERROR err;
    err = init_globals(argc, argv);
    if (err != NO_ERROR) {
        return err;
    }
    err = init_integral_methods();
    if (err != NO_ERROR) {
        mm_free(&global_funcs);
        return err;
    }
    err = init_string_methods();
    if (err != NO_ERROR) {
        mm_free(integral_methods);
        mm_free(&global_funcs);
        return err;
    }
    err = init_real_methods();
    if (err != NO_ERROR) {
        mm_free(string_methods);
        mm_free(integral_methods);
        mm_free(&global_funcs);
        return err;
    }
    return NO_ERROR;
}
