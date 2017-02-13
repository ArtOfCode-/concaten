#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "tokenizer.h"
#include "stringbuilder.h"

#define ERROR(code) do{\
    printf("Tokenizer error %d unhandled at " __FILE__ ":%d", code, __LINE__);\
    exit(code);\
    }while(0)

#define TOKENIZER_OPS_FAIL 1000
# define CTOR_FAIL (TOKENIZER_OPS_FAIL + 100)
#  define CTOR_STR_FAIL (CTOR_FAIL + 10)
#   define CTOR_STR_MALLOC_FAIL (CTOR_STR_FAIL + 1)
#   define CTOR_STR_BAD_STRLEN_FAIL (CTOR_STR_FAIL + 2)
#   define CTOR_STR_NULL_ARG_FAIL (CTOR_STR_FAIL + 3)
#  define CTOR_FILE_FAIL (CTOR_FAIL + 20)
#   define CTOR_FILE_MALLOC_FAIL (CTOR_FILE_FAIL + 1)
#   define CTOR_FILE_BAD_STRLEN_FAIL (CTOR_FILE_FAIL + 2)
#   define CTOR_FILE_NULL_ARG_FAIL (CTOR_FILE_FAIL + 3)
# define READ_CHAR_FAIL (TOKENIZER_OPS_FAIL + 200)
#  define FILE_READ_FAIL (READ_CHAR_FAIL + 10)
#   define FILE_READ_EOF_FAIL (FILE_READ_FAIL + 1)
#  define STRING_READ_FAIL (READ_CHAR_FAIL + 20)
#   define STRING_READ_EOS_FAIL (STRING_READ_FAIL + 1)
#define TOKENIZER_SYNTAX_FAIL 1500
# define SYN_NO_SEPARATION_FAIL (TOKENIZER_SYNTAX_FAIL + 1)
# define SYN_STR_FAIL (TOKENIZER_SYNTAX_FAIL + 10)
#  define

struct token_s {
    char *raw; size_t raw_len;
    size_t line, index;
    enum token_type_e type;
    
    bool error;
};

struct file_source_s {
    FILE *file;
    char next_chars[TKNR_BUF_SIZE];
    size_t next_chars_pos;
    size_t eof; // if we're at EOF, this marks where in next_chars it is
};

struct string_source_s {
    char *begin;
    char *end;
    char *cur_pos;
};

struct tokenizer_s {
    union {
        struct string_source_s string;
        struct file_source_s file;
    } source;
    bool is_from_file;
    char next_char;
    char *origin;
    size_t line, index;
    int error;
};

char read_char(tokenizer_t reading);
tokenizer_t tknr_from_string(const char *mem, const char *origin) {
    tokenizer_t ret = malloc(sizeof(struct tokenizer_s));
    if (!ret) {
        return NULL;
    }
    
    ret->origin = NULL;
    ret->source.string = (struct string_source_s) {
            .begin = NULL,
            .end = NULL,
            .cur_pos = NULL
    };
    ret->is_from_file = false;
    ret->line = 0;
    ret->index = 0;
    ret->error = 0;
    ret->next_char = '\0';
    
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
    ret->source.string = (struct string_source_s) {
            .begin = mem_c,
            .end = mem_c + mem_len,
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
    
    return ret;
}

void tknr_free(tokenizer_t freeing) {
    // is this it? wow!
    if (freeing->origin) free(freeing->origin);
    if (freeing->is_from_file) {
        fclose(freeing->source.file.file);
    } else {
        free(freeing->source.string.begin);
    }
    free(freeing);
}

int get_next_char_file(tokenizer_t from) {
    struct file_source_s *fs = &from->source.file;
    if (fs->next_chars_pos == fs->eof) {
        return FILE_READ_EOF_FAIL;
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

int get_next_char_string(tokenizer_t from) {
    struct string_source_s *ss = &from->source.string;
    if (ss->end == ss->cur_pos) {
        return STRING_READ_EOS_FAIL;
    } else {
        from->next_char = *(ss->cur_pos++);
    }
    return 0;
}

char read_char(tokenizer_t reading) {
    if (reading->error) {
        printf("Tokenizer error %d unhandled at %d", reading->error, __LINE__);
        exit(reading->error);
    }
    char ret = reading->next_char;
    int err;
    if (reading->is_from_file) {
        err = get_next_char_file(reading);
    } else {
        err = get_next_char_string(reading);
    }
    if (err) {
        reading->error = err;
        // note that it's possible that '\0' is the next character. That's why the functions
        // that use this one not only check the return value of this but tknr_err(tokenizer_t)
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

char peek_char(tokenizer_t peeking) {
    if (peeking->error) {
        printf("Tokenizer error %d unhandled at %s:%d", peeking->error, __FILE__, __LINE__);
        exit(peeking->error);
    }
    return peeking->next_char;
}

bool is_ws(char c) {
    return c == ' ' ||
           c == '\t' ||
           c == '\n' ||
           c == '\r';
}
bool skip_ws(tokenizer_t from) {
    bool skipped = false;
    while (is_ws(peek_char(from))) {
        read_char(from);
    }
    return skipped;
}
bool skip_slc(tokenizer_t from) {
    if (peek_char(from) != '#') {
        return false;
    }
    while (peek_char(from) != '\n') read_char(from);
    read_char(from); // take the newline
    return true;
}
bool skip_between(tokenizer_t from) {
    bool skipped = false;
    while (skip_ws(from) || skip_slc(from)) skipped = true;
    return skipped;
}

#define STARTING_TOKEN_MEM 16
token_t tknr_next(tokenizer_t from) {
    if (from->error) {
        ERROR(fsrom->error);
    }
    token_t ret = malloc(sizeof(struct token_s));
    if (!ret) {
        return NULL;
    }
    if (!skip_between(from)) {
        ret->error = true;
        from->error = SYN_NO_SEPARATION_FAIL;
        return ret;
    };
    size_t char_count = 0;
    size_t char_cap = STARTING_TOKEN_MEM;
    char *token_raw = malloc(STARTING_TOKEN_MEM * sizeof(char));
    char next_char = peek_char(from);
    if (!next_char && from->error) {
        ret->error = true;
        return ret;
    }
    stringbuilder_t sb = sb_new(STARTING_TOKEN_MEM);
    if ('0' <= next_char && next_char <= '9') {
        // TODO support for alternate bases
        do {
            sb_append(sb, next_char);
            next_char = peek_char(from);
        } while ('0' <= next_char && next_char <= '9');
        if (next_char == '.') {
            do {
                sb_append(sb, next_char);
                next_char = peek_char(from);
            } while ('0' <= next_char && next_char <= '9');
        }
        if (next_char == 'e') {
            do {
                sb_append(sb, next_char);
                next_char = peek_char(from);
            } while ('0' <= next_char && next_char <= '9');
        }
        
    } else if (next_char == '"') {
        
    } else if (next_char == ':') {
        //
    }
}
