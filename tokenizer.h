#ifndef CONCATEN_TOKENIZER_H
#define CONCATEN_TOKENIZER_H

#ifndef TKNR_BUF_SIZE
# define TKNR_BUF_SIZE 256
#endif
#if TKNR_BUF_SIZE <= 0
# error TKNR_BUF_SIZE cannot be <= 0
#endif

struct tokenizer_s;
typedef struct tokenizer_s *tokenizer_t;

struct token_s;
typedef struct token_s *token_t;

enum token_type_e {
    TKN_WORD, TKN_CHAR, TKN_STRING, TKN_REGEX, TKN_INTEGER, TKN_REAL
};

token_t tkn_copy(token_t);
enum token_type_e tkn_type(token_t);
char *tkn_type_name(token_t);
char *tkn_origin(token_t);
size_t tkn_line(token_t);
size_t tkn_index(token_t);
char *tkn_raw(token_t);
// token errors are stored in the relevant tokenizer, not in the token itself
bool tkn_err(token_t);
void tkn_free(token_t);
// object_t tkn_value(token_t) // defined in object.h

tokenizer_t tknr_from_string(const char *, const char *origin);
tokenizer_t tknr_from_filepath(const char *path);
tokenizer_t tknr_from_file(FILE *);
token_t tknr_next_present(tokenizer_t);
token_t tknr_next(tokenizer_t);
int tknr_err(tokenizer_t);
char *tknr_err_to_string(int);
void tknr_free(tokenizer_t);

#endif // ndef CONCATEN_TOKENIZER_H