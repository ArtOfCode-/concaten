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

struct FileSource {
    FILE *fptr;
    unsigned char next_chars[TKNR_FILE_BUF_SIZE];
    size_t next_chars_pos;
    size_t eof; // if we're at EOF, this marks where in next_chars it is
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
    int error;
    
    bool just_started;
};

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

struct Token tkn_empty(size_t line, size_t index);
char *tkn_type_name(int);
void tkn_free(struct Token *);
// object_t tkn_value(Token) // defined in object.h

struct Tokenizer tknr_from_string(const char *, const char *origin);
struct Tokenizer tknr_from_filepath(const char *path);
bool tknr_next(struct Tokenizer *, struct Token *);
char *tknr_err_to_string(int);
bool tknr_end(struct Tokenizer *);
void tknr_free(struct Tokenizer *);

#endif // ndef CONCATEN_TOKENIZER_H