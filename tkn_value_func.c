#include <stdlib.h>
#include <string.h>
#include "error.h"
#include "tokenizer.h"
#include "object.h"

const ERROR TTO_WORDS_VALUELESS_FAIL = 9000;
const ERROR TTO_UNKNOWN_TYPE_FAIL = 9001;
const ERROR TTO_NOT_IMPLEMENTED_FAIL = 9002;
const ERROR TTO_CREATE_OBJ_FAIL = 9003;
const ERROR TTO_STRING_ESCAPE_FAIL = 9004;
const ERROR TTO_ESCAPE_END_FAIL = 9005;
const ERROR TTO_MALLOC_FAIL = 9006;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
ERROR tkn_value_word(const struct Token from, struct Object **into) {
    return TTO_WORDS_VALUELESS_FAIL;
}

ERROR tto_escape_string(char **escape_in_place, size_t val_len) {
    char *ret = malloc(val_len);
    if (!ret) return TTO_MALLOC_FAIL;
    char *ret_pos = ret;
    char *const end = *escape_in_place + val_len;
    for (char *pos = *escape_in_place; pos < end; ++pos, ++ret_pos) {
        if (*pos == '\\') {
            // TODO this bit
            ++pos;
            switch (*pos) {
                case 'b': *ret_pos = '\b'; break;
                case 'a': *ret_pos = '\a'; break;
                case 't': *ret_pos = '\t'; break;
                case 'n': *ret_pos = '\n'; break;
                case 'r': *ret_pos = '\r'; break;
                case 'f': *ret_pos = '\f'; break;
                case 'v': *ret_pos = '\v'; break;
                case '0': *ret_pos = '\0'; break;
                case 'e': *ret_pos = '\x1B'; break;
                case 'x':
                    // TODO this bit
                default:
                    *ret_pos = *pos;
            }
        } else {
            *ret_pos = *pos;
        }
    }
    char *new_ret = realloc(ret, ret_pos - ret);
    if (!new_ret) {
        *escape_in_place = ret;
    } else {
        *escape_in_place = new_ret;
    }
    return NO_ERROR;
}
ERROR tkn_value_string(const struct Token from, struct Object **into) {
    size_t val_len = from.raw_len - 2;
    char *str_val = malloc(val_len);
    strncpy(str_val, from.raw + 1, val_len);
    str_val[val_len] = '\0';
    if (tto_escape_string(&str_val, val_len) != NO_ERROR) {
        return TTO_STRING_ESCAPE_FAIL;
    }
    if (ctno_literal(str_val, val_len, NULL, *into) != NO_ERROR) {
        free(str_val);
        return TTO_CREATE_OBJ_FAIL;
    }
    free(str_val);
    return NO_ERROR;
}
ERROR tkn_value_regex(const struct Token from, struct Object **into) {
    return TTO_NOT_IMPLEMENTED_FAIL;
}
ERROR tkn_value_integer(const struct Token from, struct Object **into) {
    return TTO_NOT_IMPLEMENTED_FAIL;
}
ERROR tkn_value_real(const struct Token from, struct Object **into) {
    return TTO_NOT_IMPLEMENTED_FAIL;
}
ERROR tkn_value_identifier(const struct Token from, struct Object **into) {
    return TTO_NOT_IMPLEMENTED_FAIL;
}
#pragma GCC diagnostic pop

ERROR tkn_value(const struct Token from, struct Object **into) {
    switch (from.type) {
        case TKN_WORD:
            return tkn_value_word(from, into);
        case TKN_STRING:
            return tkn_value_string(from, into);
        case TKN_REGEX:
            return tkn_value_regex(from, into);
        case TKN_INTEGER:
            return tkn_value_integer(from, into);
        case TKN_REAL:
            return tkn_value_real(from, into);
        case TKN_IDENTIFIER:
            return tkn_value_identifier(from, into);
        default:
            return TTO_UNKNOWN_TYPE_FAIL;
    }
}