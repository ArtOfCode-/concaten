#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "tokenizer.h"
#include "stringbuilder.h"

#define ERROR(code) do{\
    printf("Tokenizer error %d unhandled at " __FILE__ ":%d", code, __LINE__);\
    exit(code % 100);\
    }while(0)

#define TOKENIZER_OPS_FAIL            1000
# define CTOR_FAIL                     1100
#  define CTOR_STR_FAIL                 1110
#   define CTOR_STR_MALLOC_FAIL          1111
#   define CTOR_STR_BAD_STRLEN_FAIL      1112
#   define CTOR_STR_NULL_ARG_FAIL        1113
#  define CTOR_FILE_FAIL                1120
#   define CTOR_FILE_MALLOC_FAIL         1121
#   define CTOR_FILE_BAD_STRLEN_FAIL     1122
#   define CTOR_FILE_NULL_ARG_FAIL       1123
# define READ_CHAR_FAIL                1200
#  define FILE_READ_FAIL                1210
#   define FILE_READ_EOF_FAIL            1211
#  define STRING_READ_FAIL              1220
#   define STRING_READ_EOS_FAIL          1221
# define NEXT_TOKEN_FAIL               1300
#  define NT_MALLOC_FAIL                1301
#  define NT_NEW_SB_FAIL                1302
#  define NT_SB_FREE_COPY_FAIL          1303
#define TOKENIZER_SYNTAX_FAIL         1500
# define SYN_NO_SEPARATION_FAIL        1501
# define SYN_STR_FAIL                  1510
#  define SYN_STR_MULTILINE_FAIL        1511
# define SYN_NUM_FAIL                  1520
#  define SYN_NUM_ILLEGAL_DIGIT_FAIL    1521

struct Token {
    char *raw;
    size_t raw_len;
    char *origin;
    size_t origin_len;
    size_t line, index;
    enum token_type_e type;
};

Token tkn_copy(Token copying) {
    Token ret = malloc(sizeof(struct Token));
    if (!ret) return NULL;
    ret->raw = malloc(copying->raw_len * sizeof(char));
    if (!ret->raw) {
        free(ret);
        return NULL;
    }
    strncpy(ret->raw, copying->raw, copying->raw_len);
    ret->origin = malloc(copying->origin_len * sizeof(char));
    if (!ret->origin) {
        free(ret->raw);
        free(ret);
        return NULL;
    }
    strncpy(ret->origin, copying->origin, copying->origin_len);
    ret->index = copying->index;
    ret->line = copying->index;
    ret->type = copying->type;
    return ret;
}

enum token_type_e tkn_type(Token t) {
    return t->type;
}

//char *tkn_type_name(Token);

char *tkn_origin(Token t) {
    return t->origin;
}

size_t tkn_line(Token t) {
    return t->line;
}

size_t tkn_index(Token t) {
    return t->index;
}

char *tkn_raw(Token t) {
    return t->raw;
}

void tkn_free(Token t) {
    if (t) {
        if (t->raw) free(t->raw);
        if (t->origin) free(t->origin);
        free(t);
    }
}

struct FileSource_s {
    FILE *file;
    char next_chars[TKNR_BUF_SIZE];
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
        struct FileSource_s file;
    } source;
    bool is_from_file;
    char next_char;
    char *origin;
    size_t origin_len;
    size_t line, index;
    int error;
    
    Token last_read;
    bool just_started;
};

char read_char(Tokenizer reading);

Tokenizer tknr_from_string(const char *mem, const char *origin) {
    Tokenizer ret = malloc(sizeof(struct Tokenizer));
    if (!ret) {
        return NULL;
    }
    
    ret->origin = NULL;
    ret->source.string = (struct StringSource) {
            .begin = NULL,
            .end = NULL,
            .cur_pos = NULL
    };
    ret->is_from_file = false;
    ret->line = 1;
    ret->index = 0;
    ret->error = 0;
    ret->next_char = '\0';
    ret->last_read = NULL;
    ret->just_started = true;
    
    if (!mem || !origin) {
        ret->error = CTOR_STR_NULL_ARG_FAIL;
        return ret;
    }
    size_t mem_len = strlen(mem);
    size_t origin_len = strlen(origin);
    if (!mem_len || !origin_len) {
        ret->error = CTOR_STR_BAD_STRLEN_FAIL;
        return ret;
    }
    
    // .source
    char *mem_c = malloc(mem_len * sizeof(char));
    if (!mem_c) {
        ret->error = CTOR_STR_MALLOC_FAIL;
        return ret;
    }
    strcpy(mem_c, mem);
    ret->source.string = (struct StringSource) {
            .begin = mem_c,
            .end = mem_c + mem_len + 1,
            .cur_pos = mem_c
    };
    
    // location
    char *origin_c = malloc(origin_len * sizeof(char));
    if (!origin_c) {
        free(ret->source.string.begin);
        ret->error = CTOR_STR_MALLOC_FAIL;
        return ret;
    }
    strcpy(origin_c, origin);
    ret->origin = origin_c;
    
    read_char(ret);
    --ret->index;
    
    return ret;
}

void tknr_free(Tokenizer freeing) {
    // is this it? wow!
    if (freeing->origin) free(freeing->origin);
    if (freeing->is_from_file) {
        fclose(freeing->source.file.file);
    } else {
        free(freeing->source.string.begin);
    }
    free(freeing);
}

int get_next_char_file(Tokenizer from) {
    struct FileSource_s *fs = &from->source.file;
    if (tknr_end(from)) {
        ERROR(FILE_READ_EOF_FAIL);
    } else if (fs->next_chars_pos == TKNR_BUF_SIZE) {
        size_t count = fread(fs->next_chars, sizeof(char), TKNR_BUF_SIZE + 1, fs->file);
        if (count != TKNR_BUF_SIZE) {
            if (feof(fs->file)) {
                fs->eof = count;
            } else {
                return FILE_READ_FAIL;
            }
        }
        fs->next_chars_pos = 0;
    }
    from->next_char = fs->next_chars[fs->next_chars_pos++];
    return 0;
}

int get_next_char_string(Tokenizer from) {
    struct StringSource *ss = &from->source.string;
    if (tknr_end(from)) {
        ERROR(STRING_READ_EOS_FAIL);
    } else {
        from->next_char = *ss->cur_pos;
        ++ss->cur_pos;
    }
    return 0;
}

char read_char(Tokenizer reading) {
    if (reading->error) {
        ERROR(reading->error);
    }
    char ret = reading->next_char;
    int err = 0;
    if (reading->is_from_file) {
        err = get_next_char_file(reading);
    } else {
        err = get_next_char_string(reading);
    }
    if (err) {
        reading->error = err;
        // note that it's possible that '\0' is the next character. That's why the functions
        // that use this one not only check the return value of this but tknr_err(Tokenizer)
        return 0;
    }
    if (ret == '\n') {
        reading->index = 0;
        ++reading->line;
    } else {
        ++reading->index;
    }
    return ret;
}

char peek_char(Tokenizer peeking) {
    if (peeking->error) {
        ERROR(peeking->error);
    }
    return peeking->next_char;
}

bool is_ws(char c) {
    return c == ' ' ||
           c == '\t' ||
           c == '\n' ||
           c == '\r';
}

bool skip_ws(Tokenizer from) {
    bool skipped = false;
    while (is_ws(peek_char(from))) {
        skipped = true;
        read_char(from);
    }
    return skipped;
}

bool skip_slc(Tokenizer from) {
    if (peek_char(from) != '#') {
        return false;
    }
    while (peek_char(from) != '\n') read_char(from);
    read_char(from); // take the newline
    return true;
}

bool skip_between(Tokenizer from) {
    bool skipped = false;
    while (skip_ws(from) || skip_slc(from)) skipped = true;
    return skipped;
}

char escape_next(Tokenizer from) {
    // TODO actually escape things
    return read_char(from);
}

bool in_ranges(char c, char *begins, char *ends, size_t count) {
    for (int i = 0; i < count; ++i) {
        if (begins[i] <= c && c <= ends[i]) return true;
    }
    return false;
}

bool add_while_in_ranges(Tokenizer from, StringBuilder raw, char *next_char,
                                char *begins, char *ends, size_t num_ranges) {
    *next_char = peek_char(from);
    while (in_ranges(*next_char, begins, ends, num_ranges)) {
        sb_append(raw, read_char(from));
        *next_char = peek_char(from);
        if (!(*next_char) && tknr_err(from)) {
            return false;
        }
    }
    return true;
}

bool add_while_in_range(Tokenizer from, StringBuilder raw, char *next_char,
                               char begin, char end) {
    return add_while_in_ranges(from, raw, next_char, &begin, &end, 1);
}

#define STARTING_RAW_MEM 16

Token get_string(Tokenizer from, char *next_char, Token partial) {
    StringBuilder raw = sb_new(STARTING_RAW_MEM);
    if (!raw) {
        from->error = NT_NEW_SB_FAIL;
        return NULL;
    }
    while (1) {
        sb_append(raw, read_char(from));
        *next_char = peek_char(from);
        if (*next_char == '\\') {
            sb_append(raw, read_char(from));
            if (tknr_err(from)) {
                return NULL;
            }
            sb_append(raw, escape_next(from));
            if (tknr_err(from)) {
                return NULL;
            }
        }
        if (*next_char == '\n') {
            from->error = SYN_STR_MULTILINE_FAIL;
            sb_free(raw);
            return NULL;
        }
        if (*next_char == '"') {
            // add the quote
            sb_append(raw, read_char(from));
            break;
        }
    }
    size_t raw_cstr_len = sb_size(raw);
    char *raw_cstr = sb_free_copy(raw);
    if (!raw_cstr) {
        from->error = NT_SB_FREE_COPY_FAIL;
        sb_free(raw);
        return NULL;
    }
    partial->raw = raw_cstr;
    partial->raw_len = raw_cstr_len;
    partial->type = TKN_STRING;
    return partial;
}

Token get_number(Tokenizer from, char *next_char, Token partial) {
    // TODO Arbitrary-precision rationals?
    StringBuilder raw = sb_new(STARTING_RAW_MEM);
    if (!raw) {
        from->error = NT_NEW_SB_FAIL;
        return NULL;
    }
    partial->type = TKN_INTEGER;
    enum { B2, B8, B10, B16 } base = B10;
    if (*next_char == '0') {
        sb_append(raw, read_char(from));
        *next_char = peek_char(from);
        if (*next_char == 'x') {
            base = B16;
        } else if (*next_char == 'o') {
            base = B8;
        } else if (*next_char == 'b') {
            base = B2;
        }
        sb_append(raw, read_char(from));
    }
    bool decimal = false;
    if (base == B16) {
        if (!add_while_in_ranges(from, raw, next_char, "0aA", "9fF", 3)) {
            sb_free(raw);
            return NULL;
        }
    } else if (base == B8) {
        if (!add_while_in_range(from, raw, next_char, '0', '7')) {
            sb_free(raw);
            return NULL;
        }
    } else if (base == B2) {
        if (!add_while_in_range(from, raw, next_char, '0', '1')) {
            sb_free(raw);
            return NULL;
        }
    } else if (base == B10) {
        // add decimal digits
        if (!add_while_in_range(from, raw, next_char, '0', '9')) {
            sb_free(raw);
            return NULL;
        }
        // un punto
        if (*next_char == '.') {
            decimal = true;
            sb_append(raw, read_char(from));
            if (!add_while_in_range(from, raw, next_char, '0', '9')) {
                sb_free(raw);
                return NULL;
            }
        }
        // exponents
        if (*next_char == 'e') {
            decimal = true;
            sb_append(raw, read_char(from));
            if (!add_while_in_range(from, raw, next_char, '0', '9')) {
                sb_free(raw);
                return NULL;
            }
        }
    }
    if (tknr_err(from)) {
        sb_free(raw);
        return NULL;
    }
    if (!is_ws(*next_char)) {
        from->error = SYN_NUM_ILLEGAL_DIGIT_FAIL;
        sb_free(raw);
        return NULL;
    }
    partial->raw_len = sb_size(raw);
    partial->raw = sb_free_copy(raw);
    if (!partial->raw) {
        from->error = NT_SB_FREE_COPY_FAIL;
        sb_free(raw);
        return NULL;
    }
    if (!is_ws(*next_char)) {
        from->error = SYN_NUM_ILLEGAL_DIGIT_FAIL;
        return NULL; // not the GOTO because raw is freed here
    }
    partial->type = decimal ? TKN_REAL : TKN_INTEGER;
    return partial;
}

Token tknr_next(Tokenizer from) {
    if (tknr_err(from)) {
        ERROR(tknr_err(from));
    }
    if (!skip_between(from)) {
        if (tknr_err(from)) {
            return NULL;
        }
        // at the very beginning, it's OK not to have separation
        // (files don't have to start with code)
        if (from->just_started) {
            from->just_started = false;
        } else {
            from->error = SYN_NO_SEPARATION_FAIL;
            return NULL;
        }
    }
    if (tknr_end(from)) {
        from->error = from->is_from_file ? FILE_READ_EOF_FAIL : STRING_READ_EOS_FAIL;
        return NULL;
    }
    Token ret = malloc(sizeof(struct Token));
    if (!ret) {
        from->error = NT_MALLOC_FAIL;
        return NULL;
    }
    if (from->last_read) {
        tkn_free(from->last_read);
    }
    char *origin_c = malloc(from->origin_len * sizeof(char));
    if (!origin_c) {
        from->error = NT_MALLOC_FAIL;
        tkn_free(ret);
        return NULL;
    }
    strcpy(origin_c, from->origin);
    from->last_read = ret;
    ret->index = from->index;
    ret->line = from->line;
    ret->origin = origin_c;
    ret->type = TKN_UNKNOWN;
    char next_char = peek_char(from);
    if (!next_char && tknr_err(from)) {
        return NULL;
    }
    if (next_char == '"') { // single-line string
        ret = get_string(from, &next_char, ret);
    } else if ('0' <= next_char && next_char <= '9') {
        ret = get_number(from, &next_char, ret);
    }
    if (ret != NULL) return ret;
    tkn_free(ret);
    return NULL;
}

bool tknr_end(Tokenizer t) {
    if (t->is_from_file) {
        return t->source.file.eof == t->source.file.next_chars_pos;
    } else {
        return t->source.string.cur_pos == t->source.string.end;
    }
}

int tknr_err(Tokenizer t) {
    return t->error;
}