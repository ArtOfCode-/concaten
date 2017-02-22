#ifndef CONCATEN_TOKENIZER_H
#define CONCATEN_TOKENIZER_H

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

#ifndef TKNR_FILE_BUF_SIZE
# define TKNR_FILE_BUF_SIZE 256
#endif
#if TKNR_FILE_BUF_SIZE <= 0
# error TKNR_FILE_BUF_SIZE cannot be <= 0
#endif

struct Tokenizer;
typedef struct Tokenizer *Tokenizer;

// TODO Do to this what we just did to StringBuilder
struct Token {
    char *raw;
    size_t raw_len;
    char *origin;
    size_t origin_len;
    size_t line, index;
    enum token_type_e type;
};

enum token_type_e {
    TKN_UNKNOWN, TKN_WORD, TKN_STRING, TKN_REGEX,
    TKN_INTEGER, TKN_REAL, TKN_IDENTIFIER
};

struct Token tkn_copy(struct Token);
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
Token tknr_next(Tokenizer);
int tknr_err(Tokenizer);
char *tknr_err_to_string(int);
bool tknr_end(Tokenizer);
void tknr_free(Tokenizer);

#endif // ndef CONCATEN_TOKENIZER_H