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

//extern const ERROR TOKENIZER_OPS_FAIL;
//extern const ERROR  CTOR_FAIL;
//extern const ERROR   CTOR_STR_FAIL;
  extern const ERROR    CTOR_STR_MALLOC_FAIL;
  extern const ERROR    CTOR_STR_BAD_STRLEN_FAIL;
  extern const ERROR    CTOR_STR_NULL_ARG_FAIL;
//extern const ERROR   CTOR_FILE_FAIL;
  extern const ERROR    CTOR_FILE_MALLOC_FAIL;
  extern const ERROR    CTOR_FILE_BAD_STRLEN_FAIL;
  extern const ERROR    CTOR_FILE_NULL_ARG_FAIL;
  extern const ERROR    CTOR_FILE_FOPEN_FAIL;
//extern const ERROR  READ_CHAR_FAIL;
  extern const ERROR   FILE_READ_FAIL;
  extern const ERROR    FILE_READ_EOF_FAIL;
//extern const ERROR   STRING_READ_FAIL;
  extern const ERROR    STRING_READ_EOS_FAIL;
//extern const ERROR  NEXT_TOKEN_FAIL;
  extern const ERROR   NT_MALLOC_FAIL;
  extern const ERROR   NT_NEW_SB_FAIL;
  extern const ERROR   NT_SB_FREE_COPY_FAIL;
//extern const ERROR TOKENIZER_SYNTAX_FAIL;
  extern const ERROR  SYN_NO_SEPARATION_FAIL;
  extern const ERROR  SYN_UNEXPECTED_END_FAIL;
  extern const ERROR  SYN_EOI_FAIL;
//extern const ERROR  SYN_STR_FAIL;
  extern const ERROR   SYN_STR_MULTILINE_FAIL;
//extern const ERROR  SYN_NUM_FAIL;
  extern const ERROR   SYN_NUM_ILLEGAL_DIGIT_FAIL;
//extern const ERROR  SYN_RGX_FAIL;
  extern const ERROR   SYN_RGX_BAD_FLAG_FAIL;

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


char *tkn_type_name(enum TokenType);
void tkn_free(struct Token *);
// object_t tkn_value(Token) // defined in object.h

ERROR tknr_from_string(const char *, const char *origin, struct Tokenizer *);
ERROR tknr_from_filepath(const char *path, struct Tokenizer *);
ERROR tknr_next(struct Tokenizer *, struct Token *);
bool tknr_end(struct Tokenizer *);
void tknr_free(struct Tokenizer *);

#endif // ndef CONCATEN_TOKENIZER_H