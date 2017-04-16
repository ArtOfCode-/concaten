#ifndef CONCATEN_TOKENIZER_H
#define CONCATEN_TOKENIZER_H

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

#include "error.h"

#ifndef BUF_SIZE
# define BUF_SIZE 256
#endif
#if BUF_SIZE <= 0
# error BUF_SIZE cannot be <= 0
#endif

struct FileSource {
    FILE *fptr;
    unsigned char buf[BUF_SIZE];
    size_t next_chars_pos;
    size_t eof; // if we're at EOF, this marks where in buf it is
};
struct StringSource {
    char *begin;
    char *end;
    char *cur_pos;
};
struct Tokenizer {
    union {
        struct StringSource string;
        struct FileSource file;
    } source;
    bool is_from_file;
    char next_char;
    char *origin;
    size_t origin_len;
    size_t line, index;
    
    bool just_started;
};

enum TokenType {
    TKN_UNKNOWN, TKN_WORD, TKN_STRING, TKN_REGEX,
    TKN_INTEGRAL, TKN_REAL, TKN_IDENTIFIER
};
struct Token {
    char *raw;
    size_t raw_len;
    char *origin;
    size_t origin_len;
    size_t line, index;
    enum TokenType type;
};

const char *tkn_type_name(enum TokenType);
ERROR tkn_copy(const struct Token, struct Token *);
bool tkn_eq(const struct Token, const struct Token);
void tkn_free(struct Token *);
// object_t tkn_value(Token) // defined in object.h

ERROR tknr_from_string(const char *, const char *origin, struct Tokenizer *);
ERROR tknr_from_filepath(const char *path, struct Tokenizer *);
ERROR tknr_next(struct Tokenizer *, struct Token *);
bool tknr_end(const struct Tokenizer);
void tknr_free(struct Tokenizer *);

#endif // ndef CONCATEN_TOKENIZER_H
