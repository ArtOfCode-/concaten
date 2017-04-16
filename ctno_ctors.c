#include "scope_stack.h"
#include "ctno_ctors.h"
#include "stl.h"

#define X(name, type) \
    ERROR ctno_from_##name(type f, struct Object *i) { \
        ERROR err; \
        if ((err = mm_claim(name##_methods)) != NO_ERROR) { \
            return err; \
        } \
        return ctno_literal(&f, sizeof(f), LTL_##name, name##_methods, i); \
    }
X(integral, integral) // TODO make all_types_X work
X(real, real)
#undef X

ERROR ctno_from_cstring(char *raw, size_t len, struct Object *into) {
    ERROR err;
    if ((err = mm_claim(string_methods)) != NO_ERROR) {
        return err;
    }
    return ctno_literal(raw, len, LTL_string, string_methods, into);
}

ERROR ctno_identifier(char *raw, size_t len, struct Object *into) {
    // TODO Make this work (create identifier_methods)
//    ERROR err;
//    if ((err = mm_claim(identifier_methods)) != NO_ERROR) {
//        return err;
//    }
    return ctno_literal(raw, len, LTL_identifier, NULL, into);
}