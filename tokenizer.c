#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "tokenizer.h"
#include "stringbuilder.h"

//const ERROR TOKENIZER_FAIL             = 8000;
//const ERROR TKNR_CTOR_FAIL                  =  8100;
//const ERROR TKNR_CTOR_STR_FAIL              =   8110;
  const ERROR TKNR_CTOR_STR_MALLOC_FAIL       =    8111;
  const ERROR TKNR_CTOR_STR_BAD_STRLEN_FAIL   =    8112;
  const ERROR TKNR_CTOR_STR_NULL_ARG_FAIL     =    8113;
//const ERROR TKNR_CTOR_FILE_FAIL             =   8120;
  const ERROR TKNR_CTOR_FILE_MALLOC_FAIL      =    8121;
  const ERROR TKNR_CTOR_FILE_BAD_STRLEN_FAIL  =    8122;
  const ERROR TKNR_CTOR_FILE_NULL_ARG_FAIL    =    8123;
  const ERROR TKNR_CTOR_FILE_FOPEN_FAIL       =    8124;
//const ERROR READ_CHAR_FAIL             =  8200;
  const ERROR TKNR_FILE_READ_FAIL             =   8210;
  const ERROR TKNR_FILE_READ_EOF_FAIL         =    8211;
//const ERROR TKNR_STRING_READ_FAIL           =   8220;
  const ERROR TKNR_STRING_READ_EOS_FAIL       =    8221;
//const ERROR TKNR_NEXT_TOKEN_FAIL            =  8300;
  const ERROR TKNR_NT_MALLOC_FAIL             =   8301;
  const ERROR TKNR_NT_NEW_SB_FAIL             =   8302;
  const ERROR TKNR_NT_SB_FREE_COPY_FAIL       =   8303;
  const ERROR TKNR_NT_INPUT_END_FAIL          =   8304;
//const ERROR TKNR_SYNTAX_FAIL      = 8500;
  const ERROR TKNR_SYN_NO_SEPARATION_FAIL     =  8501;
  const ERROR TKNR_SYN_UNEXPECTED_END_FAIL    =  8502;
//const ERROR TKNR_SYN_STR_FAIL               =  8510;
  const ERROR TKNR_SYN_STR_MULTILINE_FAIL     =   8511;
//const ERROR TKNR_SYN_NUM_FAIL               =  8520;
  const ERROR TKNR_SYN_NUM_ILLEGAL_DIGIT_FAIL =   8521;
//const ERROR TKNR_SYN_RGX_FAIL               =  8530;
  const ERROR TKNR_SYN_RGX_BAD_FLAG_FAIL      =   8531;

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

char *tkn_type_name(enum TokenType t) {
    switch (t) {
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
        default:
            return NULL;
    }
}

void tkn_free(struct Token *t) {
    free(t->raw);
    t->raw = NULL;
    free(t->origin);
    t->origin = NULL;
}

ERROR get_next_char_file(struct Tokenizer *from) {
    struct FileSource *fs = &from->source.file;
    if (tknr_end(from)) {
        return TKNR_FILE_READ_EOF_FAIL;
    } else if (fs->next_chars_pos == BUF_SIZE) {
        size_t count = fread(fs->buf, sizeof(char), BUF_SIZE, fs->fptr);
        if (count != BUF_SIZE) {
            if (feof(fs->fptr)) {
                fs->eof = count;
            } else {
                return TKNR_FILE_READ_FAIL;
            }
        }
        fs->next_chars_pos = 0;
    }
    from->next_char = fs->buf[fs->next_chars_pos++];
    return NO_ERROR;
}

ERROR get_next_char_string(struct Tokenizer *from) {
    struct StringSource *ss = &from->source.string;
    if (tknr_end(from)) {
        return TKNR_STRING_READ_EOS_FAIL;
    } else {
        from->next_char = *ss->cur_pos;
        ++ss->cur_pos;
    }
    return  NO_ERROR;
}

ERROR read_char(struct Tokenizer *reading, char *into) {
    char ret = reading->next_char;
    ERROR err;
    if (reading->is_from_file) {
        err = get_next_char_file(reading);
    } else {
        err = get_next_char_string(reading);
    }
    if (ret == '\n') {
        reading->index = 0;
        ++reading->line;
    } else {
        ++reading->index;
    }
    if (into) *into = ret;
    return err;
}

ERROR tknr_from_string(const char *mem, const char *origin,
                       struct Tokenizer *into) {
    struct Tokenizer ret = (struct Tokenizer) {
            .origin = NULL,
            .is_from_file = false,
            .source.string = (struct StringSource) {
                    .begin = NULL,
                    .end = NULL,
                    .cur_pos = NULL
            },
            .next_char = '\0',
            .just_started = true,
    };
    if (!mem || !origin) {
        return TKNR_CTOR_STR_NULL_ARG_FAIL;
    }
    size_t mem_len = strlen(mem);
    size_t origin_len = strlen(origin);
    if (!mem_len || !origin_len) {
        return TKNR_CTOR_STR_BAD_STRLEN_FAIL;
    }
    ret.origin_len = origin_len + 1;
    
    // .source
    char *mem_c = malloc(mem_len + 1);
    if (!mem_c) {
        return TKNR_CTOR_STR_MALLOC_FAIL;
    }
    strcpy(mem_c, mem);
    ret.source.string = (struct StringSource) {
            .begin = mem_c,
            .end = mem_c + mem_len + 1,
            .cur_pos = mem_c
    };
    
    // location
    char *origin_c = malloc(origin_len * sizeof(char) + 1);
    if (!origin_c) {
        free(ret.source.string.begin);
        return TKNR_CTOR_STR_MALLOC_FAIL;
    }
    strcpy(origin_c, origin);
    ret.origin = origin_c;
    
    ERROR err = read_char(&ret, NULL);
    if (err != NO_ERROR) return err;
    ret.line = 1;
    ret.index = 0;
    
    *into = ret;
    return NO_ERROR;
}

ERROR tknr_from_filepath(const char *path, struct Tokenizer *into) {
    struct Tokenizer ret = (struct Tokenizer) {
            .origin = NULL,
            .source.file = (struct FileSource) {
                    .eof = BUF_SIZE,
                    .fptr = NULL,
                    .buf = {0},
                    .next_chars_pos = 0
            },
            .is_from_file = true,
            .next_char = '\0',
            .just_started = true,
    };
    
    if (!path) {
        return TKNR_CTOR_FILE_NULL_ARG_FAIL;
    }
    size_t path_len = strlen(path);
    if (!path_len) {
        return TKNR_CTOR_FILE_BAD_STRLEN_FAIL;
    }
    ret.origin_len = path_len + 1;
    
    // .source
    FILE *fptr = fopen(path, "rb");
    if (!fptr) {
        return TKNR_CTOR_FILE_FOPEN_FAIL;
    }
    ret.source.file.fptr = fptr;
    
    // location
    char *path_c = malloc(path_len * sizeof(char) + 1);
    if (!path_c) {
        fclose(fptr);
        return TKNR_CTOR_FILE_MALLOC_FAIL;
    }
    strcpy(path_c, path);
    ret.origin = path_c;
    
    struct FileSource *fs = &ret.source.file;
    size_t count = fread(fs->buf, sizeof(char), BUF_SIZE, fs->fptr);
    if (count != BUF_SIZE) {
        if (feof(fs->fptr)) {
            fs->eof = count;
        } else {
            fclose(fptr);
            free(ret.origin);
            ret.origin = NULL;
            return TKNR_FILE_READ_FAIL;
        }
    }
    ret.next_char = fs->buf[0];
    fs->next_chars_pos = 1;
    
    ret.line = 1;
    ret.index = 0;
    
    *into = ret;
    return NO_ERROR;
}

void tknr_free(struct Tokenizer *freeing) {
    // is this it? wow!
    if (freeing->origin) {
        free(freeing->origin);
        freeing->origin = NULL;
    }
    if (freeing->is_from_file) {
        if (freeing->source.file.fptr) {
            fclose(freeing->source.file.fptr);
            freeing->source.file.fptr = NULL;
        }
    } else {
        free(freeing->source.string.begin);
        freeing->source.string.begin = NULL;
        freeing->source.string.end = NULL;
        freeing->source.string.cur_pos = NULL;
    }
}

char peek_char(struct Tokenizer *peeking) {
    return peeking->next_char;
}

bool is_ws(char c) {
    return c == ' ' || c == '\t' ||
           c == '\n' || c == '\r';
}

bool skip_ws(struct Tokenizer *from) {
    bool skipped = false;
    while (is_ws(peek_char(from)) && !tknr_end(from)) {
        skipped = true;
        if (read_char(from, NULL) != NO_ERROR) return false;
    }
    return skipped;
}

bool skip_slc(struct Tokenizer *from) {
    if (peek_char(from) != '#') {
        return false;
    }
    while (peek_char(from) != '\n' && !tknr_end(from)) {
        if (read_char(from, NULL) != NO_ERROR) return false;
    }
    // skip the newline, too
    if (read_char(from, NULL) != NO_ERROR) return false;
    return true;
}

bool skip_between(struct Tokenizer *from) {
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

ERROR add_while_in_ranges(struct Tokenizer *from, struct StringBuilder *raw,
                         char *next_char, char *begins, char *ends,
                         size_t num_ranges) {
    *next_char = peek_char(from);
    ERROR err;
    while (in_ranges(*next_char, begins, ends, num_ranges)) {
        err = sb_append(raw, peek_char(from));
        if (err != NO_ERROR) {
            return err;
        }
        err = read_char(from, NULL);
        if (err != NO_ERROR) {
            return err;
        }
        *next_char = peek_char(from);
    }
    return NO_ERROR;
}

ERROR add_while_in_range(struct Tokenizer *from, struct StringBuilder *raw,
                        char *next_char, char begin, char end) {
    return add_while_in_ranges(from, raw, next_char, &begin, &end, 1);
}

#define STARTING_RAW_MEM 16

ERROR get_string(struct Tokenizer *from, struct Token partial,
                 struct Token *out) {
    ERROR err;
    struct StringBuilder raw;
    if (sb_new(STARTING_RAW_MEM, &raw) != NO_ERROR) {
        return TKNR_NT_NEW_SB_FAIL;
    }
    while (1) {
        err = sb_append(&raw, peek_char(from));
        if (err) goto error;
        err = read_char(from, NULL);
        if (err) goto error;
        if (tknr_end(from)) {
            err = TKNR_SYN_UNEXPECTED_END_FAIL;
            goto error;
        }
        char next_char = peek_char(from);
        if (next_char == '\\') {
            err = sb_append(&raw, peek_char(from));
            if (err != NO_ERROR) goto error;
            err = read_char(from, NULL);
            if (err != NO_ERROR) goto error;
            // we don't do escaping at this level
            err = sb_append(&raw, peek_char(from));
            if (err != NO_ERROR) goto error;
            err = read_char(from, NULL);
            if (err != NO_ERROR) goto error;
        }
        if (next_char == '\n') {
            err = TKNR_SYN_STR_MULTILINE_FAIL;
            goto error;
        }
        if (next_char == '"') {
            // add the quote
            err = sb_append(&raw, peek_char(from));
            if (err != NO_ERROR) goto error;
            err = read_char(from, NULL);
            if (err != NO_ERROR) goto error;
            break;
        }
    }
    size_t raw_cstr_len = raw.count;
    char *raw_cstr;
    if (sb_into_string(&raw, &raw_cstr) != NO_ERROR) {
        err = TKNR_NT_SB_FREE_COPY_FAIL;
        goto error;
    }
    partial.raw = raw_cstr;
    partial.raw_len = raw_cstr_len;
    partial.type = TKN_STRING;
    *out = partial;
    return NO_ERROR;
error:;
    sb_free(&raw);
    tkn_free(&partial);
    return err;
}

ERROR get_number(struct Tokenizer *from, struct Token partial, bool neg,
                 struct Token *out) {
    ERROR err;
    struct StringBuilder raw;
    char next_char = peek_char(from);
    if (sb_new(STARTING_RAW_MEM, &raw) != NO_ERROR) {
        err = TKNR_NT_NEW_SB_FAIL;
        goto error;
    }
    if (neg) {
        err = sb_append(&raw, '-');
        if (err != NO_ERROR) goto error;
    }
    partial.type = TKN_INTEGER;
    enum {
        B2, B8, B10, B16
    } base = B10;
    if (next_char == '0') {
        err = sb_append(&raw, peek_char(from));
        if (err != NO_ERROR) goto error;
        err = read_char(from, NULL);
        if (err != NO_ERROR) goto error;
        next_char = peek_char(from);
        if (next_char == 'x') {
            base = B16;
        } else if (next_char == 'o') {
            base = B8;
        } else if (next_char == 'b') {
            base = B2;
        }
        err = sb_append(&raw, peek_char(from));
        if (err != NO_ERROR) goto error;
        err = read_char(from, NULL);
        if (err != NO_ERROR) goto error;
    }
    bool decimal = false;
    if (base == B16) {
        err = add_while_in_ranges(from, &raw, &next_char, "0aA", "9fF", 3);
        if (err != NO_ERROR) goto error;
    } else if (base == B8) {
        err = add_while_in_range(from, &raw, &next_char, '0', '7');
        if (err != NO_ERROR) goto error;
    } else if (base == B2) {
        err = add_while_in_range(from, &raw, &next_char, '0', '1');
        if (err != NO_ERROR) goto error;
    } else if (base == B10) {
        // add decimal digits
        err = add_while_in_range(from, &raw, &next_char, '0', '9');
        if (err != NO_ERROR) goto error;
        // un punto
        if (next_char == '.') {
            decimal = true;
            err = sb_append(&raw, peek_char(from));
            if (err != NO_ERROR) goto error;
            err = read_char(from, NULL);
            if (err != NO_ERROR) goto error;
            err = add_while_in_range(from, &raw, &next_char, '0', '9');
            if (err != NO_ERROR) goto error;
        }
        // exponents
        if (next_char == 'e') {
            decimal = true;
            err = sb_append(&raw, peek_char(from));
            if (err != NO_ERROR) goto error;
            err = read_char(from, NULL);
            if (err != NO_ERROR) goto error;
            err = add_while_in_range(from, &raw, &next_char, '0', '9');
            if (err != NO_ERROR) goto error;
        }
    }
    if (!is_ws(next_char) && !tknr_end(from)) {
        err = TKNR_SYN_NUM_ILLEGAL_DIGIT_FAIL;
        goto error;
    }
    partial.raw_len = raw.count;
    err = sb_into_string(&raw, &partial.raw);
    if (err != NO_ERROR) {
        err = TKNR_NT_SB_FREE_COPY_FAIL;
        goto error;
    }
    partial.type = decimal ? TKN_REAL : TKN_INTEGER;
    *out = partial;
    return NO_ERROR;
error:;
    sb_free(&raw);
    tkn_free(&partial);
    return err;
}

bool is_flag(char c) {
    return c == 'g' || c == 'x' ||
           c == 'i' || c == 'a' ||
           c == 'm' || c == 's';
}

ERROR get_regex(struct Tokenizer *from, struct StringBuilder raw,
               struct Token ret, struct Token *out) {
    ERROR err;
    err = sb_append(&raw, peek_char(from));
    if (err != NO_ERROR) goto error;
    err = read_char(from, NULL);
    if (err != NO_ERROR) goto error;
    char next_char = peek_char(from);
    while (true) {
        if (next_char == '\\') {
            err = sb_append(&raw, peek_char(from));
            if (err != NO_ERROR) goto error;
            err = read_char(from, NULL);
            if (err) goto error;
            if (tknr_end(from)) {
                err = TKNR_SYN_UNEXPECTED_END_FAIL;
                goto error;
            }
            err = sb_append(&raw, peek_char(from));
            if (err != NO_ERROR) goto error;
            err = read_char(from, NULL);
            if (err) goto error;
            if (tknr_end(from)) {
                err = TKNR_SYN_UNEXPECTED_END_FAIL;
                goto error;
            }
        }
        if (next_char == '/') {
            err = sb_append(&raw, peek_char(from));
            if (err != NO_ERROR) goto error;
            err = read_char(from, NULL);
            if (err) goto error;
            break;
        }
        err = sb_append(&raw, peek_char(from));
        if (err != NO_ERROR) goto error;
        err = read_char(from, NULL);
        if (err) goto error;
        if (tknr_end(from)) {
            err = TKNR_SYN_UNEXPECTED_END_FAIL;
            goto error;
        }
        next_char = peek_char(from);
    }
    next_char = peek_char(from);
    while (is_flag(next_char)) {
        err = sb_append(&raw, peek_char(from));
        if (err != NO_ERROR) goto error;
        err = read_char(from, NULL);
        if (err) return err;
        next_char = peek_char(from);
    }
    if (!is_ws(next_char) && !tknr_end(from)) {
        err = TKNR_SYN_RGX_BAD_FLAG_FAIL;
        goto error;
    }
    ret.type = TKN_REGEX;
    ret.raw_len = raw.count;
    err = sb_into_string(&raw, &ret.raw);
    if (err != NO_ERROR) {
        err = TKNR_NT_SB_FREE_COPY_FAIL;
        goto error;
    }
    *out = ret;
    return NO_ERROR;
error:;
    sb_free(&raw);
    tkn_free(&ret);
    return err;
}

ERROR tknr_next(struct Tokenizer *from, struct Token *out) {
    if (tknr_end(from)) {
        return TKNR_NT_INPUT_END_FAIL;
    }
    if (!skip_between(from)) {
        // at the very beginning, it's OK not to have separation
        // (files can start with code)
        if (!from->just_started) {
            return TKNR_SYN_NO_SEPARATION_FAIL;
        }
    }
    if (from->just_started) {
        from->just_started = false;
    }
    if (tknr_end(from)) {
        return TKNR_NT_INPUT_END_FAIL;
    }
    
    ERROR err;
    struct Token ret = tkn_empty(from->line, from->index);
    ret.origin = malloc(from->origin_len * sizeof(char));
    if (!ret.origin) {
        err = TKNR_NT_MALLOC_FAIL;
        goto error;
    }
    strcpy(ret.origin, from->origin);
    char next_char = peek_char(from);
    if (next_char == '"') { // single-line string
        return get_string(from, ret, out);
    } else if ('0' <= next_char && next_char <= '9') {
        return get_number(from, ret, false, out);
    }
    struct StringBuilder raw;
    if (sb_new(STARTING_RAW_MEM, &raw) != NO_ERROR) {
        err = TKNR_NT_NEW_SB_FAIL;
        goto error;
    }
    if (next_char == ':') {
        ret.type = TKN_IDENTIFIER;
    } else if (next_char == '-') {
        err = sb_append(&raw, next_char);
        if (err != NO_ERROR) goto error;
        err = read_char(from, NULL);
        if (err != NO_ERROR) goto error;
        next_char = peek_char(from);
        if ('0' <= next_char && next_char <= '9') {
            return get_number(from, ret, true, out);
        }
    } else if (next_char == 'r') {
        err = sb_append(&raw, peek_char(from));
        if (err != NO_ERROR) goto error;
        err = read_char(from, NULL);
        if (err != NO_ERROR) goto error;
        next_char = peek_char(from);
        if (next_char == '/') {
            return get_regex(from, raw, ret, out);
        }
    }
    while (!is_ws(next_char) && !tknr_end(from)) {
        err = sb_append(&raw, peek_char(from));
        if (err != NO_ERROR) goto error;
        err = read_char(from, NULL);
        if (err != NO_ERROR) goto error;
        next_char = peek_char(from);
    }
    if (ret.type == TKN_UNKNOWN) ret.type = TKN_WORD;
    ret.raw_len = raw.count;
    err = sb_into_string(&raw, &ret.raw);
    if (err != NO_ERROR) {
        err = TKNR_NT_SB_FREE_COPY_FAIL;
        goto error;
    }
    *out = ret;
    return NO_ERROR;
error:;
    sb_free(&raw);
    tkn_free(&ret);
    return err;
}

bool tknr_end(struct Tokenizer *t) {
    if (t->is_from_file) {
        return t->source.file.eof < t->source.file.next_chars_pos;
    } else {
        return t->source.string.end == t->source.string.cur_pos;
    }
}
