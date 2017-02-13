#ifndef CONCATEN_CALLABLE_H
#define CONCATEN_CALLABLE_H

#include "errors.h"
#include "stacks.h"

struct callable_s;
typedef struct callable_s *callable_t;

ctn_err_t ccl_call(callable_t, data_stack_t, scope_stack_t, token_stack_t);

#endif //CONCATEN_CALLABLE_H
