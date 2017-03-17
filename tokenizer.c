#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "tokenizer.h"
#include "stringbuilder.h"

#define ERROR(code) do { \
    printf("Tokenizer error " ERROR_FMT " unhandled at "__FILE__":%d", \
        code, __LINE__); \
    exit(code % 100); \
} while(0)

// const ERROR TOKENIZER_OPS_FAIL     = 1000;
// const ERROR CTOR_FAIL              =  1100;
// const ERROR CTOR_STR_FAIL          =   1110;
const ERROR CTOR_STR_MALLOC_FAIL      =    1111;
const ERROR CTOR_STR_BAD_STRLEN_FAIL  =    1112;
const ERROR CTOR_STR_NULL_ARG_FAIL    =    1113;
// const ERROR CTOR_FILE_FAIL         =   1120;
const ERROR CTOR_FILE_MALLOC_FAIL     =    1121;
const ERROR CTOR_FILE_BAD_STRLEN_FAIL =    1122;
const ERROR CTOR_FILE_NULL_ARG_FAIL   =    1123;
const ERROR CTOR_FILE_FOPEN_FAIL      =    1124;
// const ERROR READ_CHAR_FAIL         =  1200;
const ERROR FILE_READ_FAIL            =   1210;
const ERROR FILE_READ_EOF_FAIL        =    1211;
// const ERROR STRING_READ_FAIL       =   1220;
const ERROR STRING_READ_EOS_FAIL      =    1221;
// const ERROR NEXT_TOKEN_FAIL        =  1300;
const ERROR NT_MALLOC_FAIL            =   1301;
const ERROR NT_NEW_SB_FAIL            =   1302;
const ERROR NT_SB_FREE_COPY_FAIL      =   1303;
// const ERROR TOKENIZER_SYNTAX_FAIL  = 1500;
const ERROR SYN_NO_SEPARATION_FAIL    =  1501;
const ERROR SYN_UNEXPECTED_END_FAIL   =  1502;
// const ERROR SYN_STR_FAIL           =  1510;
const ERROR SYN_STR_MULTILINE_FAIL    =   1511;
// const ERROR SYN_NUM_FAIL           =  1520;
const ERROR SYN_NUM_ILLEGAL_DIGIT_FAIL=   1521;
// const ERROR SYN_RGX_FAIL           =  1530;
const ERROR SYN_RGX_BAD_FLAG_FAIL     =   1531;

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

char *tkn_type_name(int t) {
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
        return FILE_READ_EOF_FAIL;
    } else if (fs->next_chars_pos == BUF_SIZE) {
        size_t count = fread(fs->buf, sizeof(char), BUF_SIZE, fs->fptr);
        if (count != BUF_SIZE) {
            if (feof(fs->fptr)) {
                fs->eof = count;
            } else {
                return FILE_READ_FAIL;
            }
        }
        fs->next_chars_pos = 0;
    }
    from->next_char = fs->buf[fs->next_chars_pos++];
    return SUCCESS;
}

ERROR get_next_char_string(struct Tokenizer *from) {
    struct StringSource *ss = &from->source.string;
    if (tknr_end(from)) {
        return STRING_READ_EOS_FAIL;
    } else {
        from->next_char = *ss->cur_pos;
        ++ss->cur_pos;
    }
    return  SUCCESS;
}

ERROR read_char(struct Tokenizer *reading, char *into) {
    if (reading->error) {
        ERROR(reading->error);
    }
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

struct Tokenizer tknr_from_string(const char *mem, const char *origin) {
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
            .error = 0
    };
    
    if (!mem || !origin) {
        ret.error = CTOR_STR_NULL_ARG_FAIL;
        return ret;
    }
    size_t mem_len = strlen(mem);
    size_t origin_len = strlen(origin);
    if (!mem_len || !origin_len) {
        ret.error = CTOR_STR_BAD_STRLEN_FAIL;
        return ret;
    }
    ret.origin_len = origin_len + 1;
    
    // .source
    char *mem_c = malloc(mem_len + 1);
    if (!mem_c) {
        ret.error = CTOR_STR_MALLOC_FAIL;
        return ret;
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
        ret.error = CTOR_STR_MALLOC_FAIL;
        return ret;
    }
    strcpy(origin_c, origin);
    ret.origin = origin_c;
    
    read_char(&ret, NULL);
    ret.line = 1;
    ret.index = 0;
    
    return ret;
}

struct Tokenizer tknr_from_filepath(const char *path) {
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
            .error = 0
    };
    
    if (!path) {
        ret.error = CTOR_FILE_NULL_ARG_FAIL;
        return ret;
    }
    size_t path_len = strlen(path);
    if (!path_len) {
        ret.error = CTOR_FILE_BAD_STRLEN_FAIL;
        return ret;
    }
    ret.origin_len = path_len + 1;
    
    // .source
    FILE *fptr = fopen(path, "rb");
    if (!fptr) {
        ret.error = CTOR_FILE_FOPEN_FAIL;
        return ret;
    }
    ret.source.file.fptr = fptr;
    
    // location
    char *path_c = malloc(path_len * sizeof(char) + 1);
    if (!path_c) {
        fclose(fptr);
        ret.error = CTOR_FILE_MALLOC_FAIL;
        return ret;
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
            ret.error = FILE_READ_FAIL;
            return ret;
        }
    }
    ret.next_char = fs->buf[0];
    fs->next_chars_pos = 1;
    
    ret.line = 1;
    ret.index = 0;
    
    return ret;
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
    if (peeking->error) {
        ERROR(peeking->error);
    }
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
        read_char(from, NULL);
    }
    return skipped;
}

bool skip_slc(struct Tokenizer *from) {
    if (peek_char(from) != '#') {
        return false;
    }
    while (peek_char(from) != '\n' && !tknr_end(from)) {
        read_char(from, NULL);
    }
    read_char(from, NULL); // take the newline
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

bool add_while_in_ranges(struct Tokenizer *from, struct StringBuilder *raw,
                         char *next_char, char *begins, char *ends,
                         size_t num_ranges) {
    *next_char = peek_char(from);
    while (in_ranges(*next_char, begins, ends, num_ranges)) {
        sb_append(raw, peek_char(from));
        read_char(from, NULL);
        *next_char = peek_char(from);
        if (!(*next_char) && from->error) {
            return false;
        }
    }
    return true;
}

bool add_while_in_range(struct Tokenizer *from, struct StringBuilder *raw,
                        char *next_char, char begin, char end) {
    return add_while_in_ranges(from, raw, next_char, &begin, &end, 1);
}

#define STARTING_RAW_MEM 16

bool get_string(struct Tokenizer *from, char *next_char,
                struct Token partial, struct Token *out) {
    struct StringBuilder raw = sb_new();
    if (!sb_init(&raw, STARTING_RAW_MEM)) {
        from->error = NT_NEW_SB_FAIL;
        return false;
    }
    while (1) {
        sb_append(&raw, peek_char(from));
        read_char(from, NULL);
        if (tknr_end(from)) {
            from->error = SYN_UNEXPECTED_END_FAIL;
            goto error;
        }
        *next_char = peek_char(from);
        if (*next_char == '\\') {
            sb_append(&raw, peek_char(from));
            read_char(from, NULL);
            if (from->error) {
                goto error;
            }
            sb_append(&raw, peek_char(from));
            read_char(from, NULL);
            if (from->error) {
                goto error;
            }
        }
        if (*next_char == '\n') {
            from->error = SYN_STR_MULTILINE_FAIL;
            goto error;
        }
        if (*next_char == '"') {
            // add the quote
            sb_append(&raw, peek_char(from));
            read_char(from, NULL);
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

bool get_number(struct Tokenizer *from, char *next_char, struct Token partial,
                bool neg, struct Token *out) {
    struct StringBuilder raw = sb_new();
    if (!sb_init(&raw, STARTING_RAW_MEM)) {
        from->error = NT_NEW_SB_FAIL;
        goto error;
    }
    if (neg) {
        sb_append(&raw, '-');
    }
    partial.type = TKN_INTEGER;
    enum {
        B2, B8, B10, B16
    } base = B10;
    if (*next_char == '0') {
        sb_append(&raw, peek_char(from));
        read_char(from, NULL);
        *next_char = peek_char(from);
        if (*next_char == 'x') {
            base = B16;
        } else if (*next_char == 'o') {
            base = B8;
        } else if (*next_char == 'b') {
            base = B2;
        }
        sb_append(&raw, peek_char(from));
        read_char(from, NULL);
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
            sb_append(&raw, peek_char(from));
            read_char(from, NULL);
            if (!add_while_in_range(from, &raw, next_char, '0', '9')) {
                goto error;
            }
        }
        // exponents
        if (*next_char == 'e') {
            decimal = true;
            sb_append(&raw, peek_char(from));
            read_char(from, NULL);
            if (!add_while_in_range(from, &raw, next_char, '0', '9')) {
                goto error;
            }
        }
    }
    if (from->error) {
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

bool get_regex(struct Tokenizer *from, struct StringBuilder raw,
               struct Token ret, struct Token *out) {
    sb_append(&raw, peek_char(from));
    read_char(from, NULL);
    char next_char = peek_char(from);
    while (true) {
        if (next_char == '\\') {
            sb_append(&raw, peek_char(from));
            read_char(from, NULL);
            if (from->error) {
                goto error;
            } else if (tknr_end(from)) {
                from->error = SYN_UNEXPECTED_END_FAIL;
                goto error;
            }
            sb_append(&raw, peek_char(from));
            read_char(from, NULL);
            if (from->error) {
                goto error;
            } else if (tknr_end(from)) {
                from->error = SYN_UNEXPECTED_END_FAIL;
                goto error;
            }
        }
        if (next_char == '/') {
            sb_append(&raw, peek_char(from));
            read_char(from, NULL);
            break;
        }
        sb_append(&raw, peek_char(from));
        read_char(from, NULL);
        if (from->error) {
            goto error;
        } else if (tknr_end(from)) {
            from->error = SYN_UNEXPECTED_END_FAIL;
            goto error;
        }
        next_char = peek_char(from);
    }
    next_char = peek_char(from);
    while (is_flag(next_char)) {
        sb_append(&raw, peek_char(from));
        read_char(from, NULL);
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

bool tknr_next(struct Tokenizer *from, struct Token *out) {
    if (from->error) {
        ERROR(from->error);
    }
    if (tknr_end(from)) {
        return NULL;
    }
    if (!skip_between(from)) {
        if (from->error) {
            return false;
        }
        // at the very beginning, it's OK not to have separation
        // (files can start with code)
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
    if (!next_char && from->error) {
        goto error;
    }
    if (next_char == '"') { // single-line string
        return get_string(from, &next_char, ret, out);
    } else if ('0' <= next_char && next_char <= '9') {
        return get_number(from, &next_char, ret, false, out);
    }
    if (!sb_init(&raw, STARTING_RAW_MEM)) {
        from->error = NT_NEW_SB_FAIL;
        goto error;
    }
    if (next_char == ':') {
        ret.type = TKN_IDENTIFIER;
    } else if (next_char == '-') {
        sb_append(&raw, peek_char(from));
        read_char(from, NULL);
        next_char = peek_char(from);
        if ('0' <= next_char && next_char <= '9') {
            return get_number(from, &next_char, ret, true, out);
        }
    } else if (next_char == 'r') {
        sb_append(&raw, peek_char(from));
        read_char(from, NULL);
        next_char = peek_char(from);
        if (next_char == '/') {
            return get_regex(from, raw, ret, out);
        }
    }
    while (!is_ws(next_char) && !tknr_end(from)) {
        sb_append(&raw, peek_char(from));
        read_char(from, NULL);
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

bool tknr_end(struct Tokenizer *t) {
    if (t->is_from_file) {
        return t->source.file.eof < t->source.file.next_chars_pos;
    } else {
        return t->source.string.end == t->source.string.cur_pos;
    }
}
