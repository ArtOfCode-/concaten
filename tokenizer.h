#ifndef CONCATEN_TOKENIZER_H
#define CONCATEN_TOKENIZER_H

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

#ifndef TKNR_BUF_SIZE
# define TKNR_BUF_SIZE 256
#endif
#if TKNR_BUF_SIZE <= 0
# error TKNR_BUF_SIZE cannot be <= 0
#endif

struct Tokenizer;
typedef struct Tokenizer *Tokenizer;

struct Token;
typedef struct Token *Token;

enum token_type_e {
    TKN_UNKNOWN, TKN_WORD, TKN_STRING, TKN_REGEX,
    TKN_INTEGER, TKN_REAL, TKN_IDENTIFIER
};

Token tkn_copy(Token);
enum token_type_e tkn_type(Token);
char *tkn_type_name(Token);
char *tkn_origin(Token);
size_t tkn_line(Token);
size_t tkn_index(Token);
char *tkn_raw(Token);
void tkn_free(Token);
// object_t tkn_value(Token) // defined in object.h

Tokenizer tknr_from_string(const char *, const char *origin);
Tokenizer tknr_from_filepath(const char *path);
Tokenizer tknr_from_file(FILE *);
Token tknr_next(Tokenizer);
int tknr_err(Tokenizer);
char *tknr_err_to_string(int);
bool tknr_end(Tokenizer);
void tknr_free(Tokenizer);

#endif // ndef CONCATEN_TOKENIZER_H