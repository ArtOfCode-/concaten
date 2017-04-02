#ifndef CONCATEN_CTNO_CTORS_H
#define CONCATEN_CTNO_CTORS_H

#include "object.h"
#include "data_stack.h"
#include "token_stack.h"

#define X(f, t) ctno_mk_##f(t from);
all_types_X
#undef X
ctno_String_bytes(char *, size_t);

#endif //CONCATEN_CTNO_CTORS_H
