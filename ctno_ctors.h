#ifndef CONCATEN_CTNO_CTORS_H
#define CONCATEN_CTNO_CTORS_H

#include "object.h"

#define X(name, type) ERROR ctno_from_##name(type, struct Object *);
all_types_X
#undef X
ERROR ctno_from_cstring(char *, size_t, struct Object *);
ERROR ctno_identifier(char *, size_t, struct Object *);

#endif //CONCATEN_CTNO_CTORS_H
