#ifndef CONCATEN_STACKS_H
#define CONCATEN_STACKS_H

#include "tokenizer.h"
#include "object.h"
#include "callable.h"

struct data_stack_s;
typedef struct data_stack_s *data_stack_t;

struct scope_stack_s;
typedef struct scope_stack_s *scope_stack_t;

struct token_stack_s;
typedef struct token_stack_s *token_stack_t;

data_stack_t dst_empty();
data_stack_t dst_copy(data_stack_t);
void dst_push(object_t);
object_t dst_pop(data_stack_t);
void dst_restore(data_stack_t to, data_stack_t from);
void dst_free(data_stack_t);

scope_stack_t sst_globals();
scope_stack_t sst_copy(scope_stack_t);
size_t sst_search(scope_stack_t, char *, callable_t **);
void sst_restore(scope_stack_t to, scope_stack_t from);
void sst_free(scope_stack_t);

token_stack_t tst_from_tokenizer(tokenizer_t);
token_stack_t tst_copy(token_stack_t);
token_t tst_pop_token(token_stack_t);
bool tst_is_empty();
bool tst_is_top_empty();
void tst_restore(token_stack_t to, token_stack_t from);
void tst_free(token_stack_t);

#endif //CONCATEN_STACKS_H
