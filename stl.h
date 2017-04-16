#ifndef CONCATEN_STL_H
#define CONCATEN_STL_H

#include "object.h"
#define X(name, _) \
    extern struct MethodMap *name##_methods; \
    ERROR init_##name##_methods();
all_types_X
X(string, _)
#undef X

struct MethodMap global_funcs;
ERROR init_globals(int argc, char **argv);

ERROR init_stl(int argc, char **argv);

#endif //CONCATEN_STL_H
