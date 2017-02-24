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

// TODO 2 down, one to go...
struct Tokenizer;
typedef struct Tokenizer *Tokenizer;

enum TokenType {
    TKN_UNKNOWN, TKN_WORD, TKN_STRING, TKN_REGEX,
    TKN_INTEGER, TKN_REAL, TKN_IDENTIFIER
};
struct Token {
    char *raw;
    size_t raw_len;
    char *origin;
    size_t origin_len;
    size_t line, index;
    enum TokenType type;
};

bool tkn_copy(struct Token, struct Token *);
enum TokenType tkn_type(struct Token);
char *tkn_type_name(struct Token);
char *tkn_origin(struct Token);
size_t tkn_line(struct Token);
size_t tkn_index(struct Token);
char *tkn_raw(struct Token);
void tkn_free(struct Token *);
// object_t tkn_value(Token) // defined in object.h

Tokenizer tknr_from_string(const char *, const char *origin);
Tokenizer tknr_from_filepath(const char *path);
bool tknr_next(Tokenizer, struct Token *);
int tknr_err(Tokenizer);
char *tknr_err_to_string(int);
bool tknr_end(Tokenizer);
void tknr_free(Tokenizer);

#endif // ndef CONCATEN_TOKENIZER_H