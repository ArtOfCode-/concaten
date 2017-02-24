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

//define TOKENIZER_OPS_FAIL            1000
// define CTOR_FAIL                     1100
//  define CTOR_STR_FAIL                 1110
#    define CTOR_STR_MALLOC_FAIL          1111
#    define CTOR_STR_BAD_STRLEN_FAIL      1112
#    define CTOR_STR_NULL_ARG_FAIL        1113
//  define CTOR_FILE_FAIL                1120
#    define CTOR_FILE_MALLOC_FAIL         1121
#    define CTOR_FILE_BAD_STRLEN_FAIL     1122
#    define CTOR_FILE_NULL_ARG_FAIL       1123
#    define CTOR_FILE_FOPEN_FAIL          1124
// define READ_CHAR_FAIL                1200
#   define FILE_READ_FAIL                1210
#    define FILE_READ_EOF_FAIL            1211
//  define STRING_READ_FAIL              1220
#    define STRING_READ_EOS_FAIL          1221
// define NEXT_TOKEN_FAIL               1300
#   define NT_MALLOC_FAIL                1301
#   define NT_NEW_SB_FAIL                1302
#   define NT_SB_FREE_COPY_FAIL          1303
//define TOKENIZER_SYNTAX_FAIL         1500
#  define SYN_NO_SEPARATION_FAIL        1501
#  define SYN_UNEXPECTED_END_FAIL       1502
// define SYN_STR_FAIL                  1510
#   define SYN_STR_MULTILINE_FAIL        1511
// define SYN_NUM_FAIL                  1520
#   define SYN_NUM_ILLEGAL_DIGIT_FAIL    1521
// define SYN_RGX_FAIL                  1530
#   define SYN_RGX_BAD_FLAG_FAIL         1531

struct Token tkn_empty(size_t line, size_t index) {
    return (struct Token) {
            .type = TKN_UNKNOWN,
            .index = index,
            .line = line,
            .origin = NULL,
            .raw = NULL,
            .origin_len = 0,
            .raw_len = 0,
    };
}

bool tkn_copy(struct Token copying, struct Token *into) {
    struct Token ret = (struct Token) {
            .index = copying.index,
            .line = copying.line,
            .origin = NULL,
            .origin_len = copying.origin_len,
            .raw = NULL,
            .raw_len = copying.origin_len,
            .type = copying.type
    };
    ret.raw = malloc(copying.raw_len);
    if (!ret.raw) {
        return false;
    }
    memcpy(ret.raw, copying.raw, copying.raw_len);
    ret.origin = malloc(copying.origin_len);
    if (!ret.origin) {
        free(ret.raw);
        return false;
    }
    memcpy(ret.origin, copying.origin, copying.origin_len);
    ret.index = copying.index;
    ret.line = copying.index;
    ret.type = copying.type;
    *into = ret;
    return true;
}

char *tkn_type_name(struct Token t) {
    switch (t.type) {
        case TKN_UNKNOWN:
            return "unknown";
        case TKN_WORD:
            return "word";
        case TKN_STRING:
            return "string";
        case TKN_REGEX:
            return "regex";
        case TKN_INTEGER:
            return "integer";
        case TKN_REAL:
            return "real";
        case TKN_IDENTIFIER:
            return "identifier";
    }
    abort();
}

enum TokenType tkn_type(struct Token t) {
    return t.type;
}

//char *tkn_type_name(Token);

char *tkn_origin(struct Token t) {
    return t.origin;
}

size_t tkn_line(struct Token t) {
    return t.line;
}

size_t tkn_index(struct Token t) {
    return t.index;
}

char *tkn_raw(struct Token t) {
    return t.raw;
}

void tkn_free(struct Token *t) {
    free(t->raw);
    t->raw = NULL;
    free(t->origin);
    t->origin = NULL;
}

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
    ret->next_char = '\0';
    ret->just_started = true;
    ret->error = 0;
    
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
    char *mem_c = malloc(mem_len * sizeof(char) + 1);
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
    char *origin_c = malloc(origin_len * sizeof(char) + 1);
    if (!origin_c) {
        free(ret->source.string.begin);
        ret->error = CTOR_STR_MALLOC_FAIL;
        return ret;
    }
    strcpy(origin_c, origin);
    ret->origin = origin_c;
    
    read_char(ret);
    ret->line = 1;
    ret->index = 0;
    
    return ret;
}

Tokenizer tknr_from_filepath(const char *path) {
    Tokenizer ret = malloc(sizeof(struct Tokenizer));
    if (!ret) {
        return NULL;
    }
    
    ret->origin = NULL;
    ret->source.file = (struct FileSource) {
            .eof = TKNR_FILE_BUF_SIZE,
            .fptr = NULL,
            .next_chars = {0},
            .next_chars_pos = 0,
    };
    ret->is_from_file = true;
    ret->next_char = '\0';
    ret->just_started = true;
    ret->error = 0;
    
    if (!path) {
        ret->error = CTOR_FILE_NULL_ARG_FAIL;
        return ret;
    }
    size_t path_len = strlen(path);
    if (!path_len) {
        ret->error = CTOR_FILE_BAD_STRLEN_FAIL;
        return ret;
    }
    
    // .source
    FILE *fptr = fopen(path, "rb");
    if (!fptr) {
        ret->error = CTOR_FILE_FOPEN_FAIL;
        return ret;
    }
    ret->source.file.fptr = fptr;
    
    // location
    char *path_c = malloc(path_len * sizeof(char) + 1);
    if (!path_c) {
        fclose(ret->source.file.fptr);
        ret->error = CTOR_FILE_MALLOC_FAIL;
        return ret;
    }
    strcpy(path_c, path);
    ret->origin = path_c;
    
    struct FileSource *fs = &ret->source.file;
    size_t count = fread(fs->next_chars, sizeof(char), TKNR_FILE_BUF_SIZE, fs->fptr);
    if (count != TKNR_FILE_BUF_SIZE) {
        if (feof(fs->fptr)) {
            fs->eof = count;
        } else {
            ret->error = FILE_READ_FAIL;
            fclose(fptr);
            free(path_c);
            return ret;
        }
    }
    ret->next_char = fs->next_chars[0];
    fs->next_chars_pos = 1;
    
    ret->line = 1;
    ret->index = 0;
    
    return ret;
}

void tknr_free(Tokenizer freeing) {
    // is this it? wow!
    if (freeing->origin) free(freeing->origin);
    if (freeing->is_from_file) {
        if (freeing->source.file.fptr) fclose(freeing->source.file.fptr);
    } else {
        free(freeing->source.string.begin);
    }
    free(freeing);
}

int get_next_char_file(Tokenizer from) {
    struct FileSource *fs = &from->source.file;
    if (tknr_end(from)) {
        ERROR(FILE_READ_EOF_FAIL);
    } else if (fs->next_chars_pos == TKNR_FILE_BUF_SIZE) {
        size_t count = fread(fs->next_chars, sizeof(char), TKNR_FILE_BUF_SIZE, fs->fptr);
        if (count != TKNR_FILE_BUF_SIZE) {
            if (feof(fs->fptr)) {
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
    return c == ' ' || c == '\t' ||
           c == '\n' || c == '\r';
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

bool in_ranges(char c, char *begins, char *ends, size_t count) {
    for (int i = 0; i < count; ++i) {
        if (begins[i] <= c && c <= ends[i]) return true;
    }
    return false;
}

bool add_while_in_ranges(Tokenizer from, struct StringBuilder *raw, char *next_char,
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

bool add_while_in_range(Tokenizer from, struct StringBuilder *raw, char *next_char,
                        char begin, char end) {
    return add_while_in_ranges(from, raw, next_char, &begin, &end, 1);
}

#define STARTING_RAW_MEM 16

bool get_string(Tokenizer from, char *next_char, struct Token partial, struct Token *out) {
    struct StringBuilder raw = sb_new();
    if (!sb_init(&raw, STARTING_RAW_MEM)) {
        from->error = NT_NEW_SB_FAIL;
        return NULL;
    }
    while (1) {
        sb_append(&raw, read_char(from));
        if (tknr_end(from)) {
            from->error = SYN_UNEXPECTED_END_FAIL;
            goto error;
        }
        *next_char = peek_char(from);
        if (*next_char == '\\') {
            sb_append(&raw, read_char(from));
            if (tknr_err(from)) {
                goto error;
            }
            sb_append(&raw, read_char(from));
            if (tknr_err(from)) {
                goto error;
            }
        }
        if (*next_char == '\n') {
            from->error = SYN_STR_MULTILINE_FAIL;
            goto error;
        }
        if (*next_char == '"') {
            // add the quote
            sb_append(&raw, read_char(from));
            break;
        }
    }
    size_t raw_cstr_len = raw.count;
    char *raw_cstr = sb_into_string(raw);
    if (!raw_cstr) {
        from->error = NT_SB_FREE_COPY_FAIL;
        goto error;
    }
    partial.raw = raw_cstr;
    partial.raw_len = raw_cstr_len;
    partial.type = TKN_STRING;
    *out = partial;
    return true;
error:;
    sb_free(&raw);
    tkn_free(&partial);
    return false;
}

bool get_number(Tokenizer from, char *next_char, struct Token partial, struct Token *out) {
    struct StringBuilder raw = sb_new();
    if (!sb_init(&raw, STARTING_RAW_MEM)) {
        from->error = NT_NEW_SB_FAIL;
        goto error;
    }
    partial.type = TKN_INTEGER;
    enum {
        B2, B8, B10, B16
    } base = B10;
    if (*next_char == '0') {
        sb_append(&raw, read_char(from));
        *next_char = peek_char(from);
        if (*next_char == 'x') {
            base = B16;
        } else if (*next_char == 'o') {
            base = B8;
        } else if (*next_char == 'b') {
            base = B2;
        }
        sb_append(&raw, read_char(from));
    }
    bool decimal = false;
    if (base == B16) {
        if (!add_while_in_ranges(from, &raw, next_char, "0aA", "9fF", 3)) {
            goto error;
        }
    } else if (base == B8) {
        if (!add_while_in_range(from, &raw, next_char, '0', '7')) {
            goto error;
        }
    } else if (base == B2) {
        if (!add_while_in_range(from, &raw, next_char, '0', '1')) {
            goto error;
        }
    } else if (base == B10) {
        // add decimal digits
        if (!add_while_in_range(from, &raw, next_char, '0', '9')) {
            goto error;
        }
        // un punto
        if (*next_char == '.') {
            decimal = true;
            sb_append(&raw, read_char(from));
            if (!add_while_in_range(from, &raw, next_char, '0', '9')) {
                goto error;
            }
        }
        // exponents
        if (*next_char == 'e') {
            decimal = true;
            sb_append(&raw, read_char(from));
            if (!add_while_in_range(from, &raw, next_char, '0', '9')) {
                goto error;
            }
        }
    }
    if (tknr_err(from)) {
        goto error;
    }
    if (!is_ws(*next_char) && !tknr_end(from)) {
        from->error = SYN_NUM_ILLEGAL_DIGIT_FAIL;
        goto error;
    }
    partial.raw_len = raw.count;
    partial.raw = sb_into_string(raw);
    if (!partial.raw) {
        from->error = NT_SB_FREE_COPY_FAIL;
        goto error;
    }
    partial.type = decimal ? TKN_REAL : TKN_INTEGER;
    *out = partial;
    return true;
error:;
    sb_free(&raw);
    tkn_free(&partial);
    return false;
}

bool is_flag(char c) {
    return c == 'g' || c == 'x' ||
           c == 'i' || c == 'a' ||
           c == 'm' || c == 's';
}

bool get_regex(Tokenizer from, struct StringBuilder raw, struct Token ret, struct Token *out) {
    sb_append(&raw, read_char(from));
    char next_char = peek_char(from);
    while (true) {
        if (next_char == '\\') {
            sb_append(&raw, read_char(from));
            if (tknr_err(from)) {
                goto error;
            } else if (tknr_end(from)) {
                from->error = SYN_UNEXPECTED_END_FAIL;
                goto error;
            }
            sb_append(&raw, read_char(from));
            if (tknr_err(from)) {
                goto error;
            } else if (tknr_end(from)) {
                from->error = SYN_UNEXPECTED_END_FAIL;
                goto error;
            }
        }
        if (next_char == '/') {
            sb_append(&raw, read_char(from));
            break;
        }
        sb_append(&raw, read_char(from));
        if (tknr_err(from)) {
            goto error;
        } else if (tknr_end(from)) {
            from->error = SYN_UNEXPECTED_END_FAIL;
            goto error;
        }
        next_char = peek_char(from);
    }
    next_char = peek_char(from);
    while (is_flag(next_char)) {
        sb_append(&raw, read_char(from));
        next_char = peek_char(from);
    }
    if (!is_ws(next_char) && !tknr_end(from)) {
        from->error = SYN_RGX_BAD_FLAG_FAIL;
        goto error;
    }
    ret.type = TKN_REGEX;
    ret.raw_len = raw.count;
    ret.raw = sb_into_string(raw);
    if (!ret.raw) {
        from->error = NT_SB_FREE_COPY_FAIL;
        goto error;
    }
    *out = ret;
    return true;
error:;
    sb_free(&raw);
    tkn_free(&ret);
    return false;
}

// TODO Look through for any possible code prettifications. this is ugly.
bool tknr_next(Tokenizer from, struct Token *out) {
    if (tknr_err(from)) {
        ERROR(tknr_err(from));
    }
    if (tknr_end(from)) {
        return NULL;
    }
    if (!skip_between(from)) {
        if (tknr_err(from)) {
            return false;
        }
        // at the very beginning, it's OK not to have separation
        // (files don't have to start with code)
        if (!from->just_started) {
            from->error = SYN_NO_SEPARATION_FAIL;
            return false;
        }
    }
    if (from->just_started) {
        from->just_started = false;
    }
    if (tknr_end(from)) {
        return false;
    }
    
    struct StringBuilder raw = sb_new();
    struct Token ret = tkn_empty(from->line, from->index);
    ret.origin = malloc(from->origin_len * sizeof(char));
    if (!ret.origin) {
        from->error = NT_MALLOC_FAIL;
        goto error;
    }
    strcpy(ret.origin, from->origin);
    char next_char = peek_char(from);
    if (!next_char && tknr_err(from)) {
        goto error;
    }
    if (next_char == '"') { // single-line string
        return get_string(from, &next_char, ret, out);
    } else if ('0' <= next_char && next_char <= '9') {
        return get_number(from, &next_char, ret, out);
    }
    if (!sb_init(&raw, STARTING_RAW_MEM)) {
        from->error = NT_NEW_SB_FAIL;
        goto error;
    }
    if (next_char == ':') {
        ret.type = TKN_IDENTIFIER;
    } else if (next_char == 'r') {
        sb_append(&raw, read_char(from));
        next_char = peek_char(from);
        if (next_char == '/') {
            return get_regex(from, raw, ret, out);
        }
    }
    while (!is_ws(next_char) && !tknr_end(from)) {
        sb_append(&raw, read_char(from));
        next_char = peek_char(from);
    }
    if (ret.type == TKN_UNKNOWN) ret.type = TKN_WORD;
    ret.raw_len = raw.count;
    ret.raw = sb_into_string(raw);
    if (!ret.raw) {
        from->error = NT_SB_FREE_COPY_FAIL;
        sb_free(&raw);
    }
    *out = ret;
    return true;
error:;
    sb_free(&raw);
    tkn_free(&ret);
    return false;
}

bool tknr_end(Tokenizer t) {
    if (t->is_from_file) {
        return t->source.file.eof < t->source.file.next_chars_pos;
    } else {
        return t->source.string.end == t->source.string.cur_pos;
    }
}

int tknr_err(Tokenizer t) {
    return t->error;
}
