#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <math.h>
#include "error.h"
#include "tokenizer.h"
#include "object.h"

const ERROR TTO_WORDS_VALUELESS_FAIL = 9000;
const ERROR TTO_UNKNOWN_TYPE_FAIL = 9001;
const ERROR TTO_NOT_IMPLEMENTED_FAIL = 9002;
const ERROR TTO_STRING_ESCAPE_FAIL = 9003;
const ERROR TTO_ESCAPE_END_FAIL = 9004;
const ERROR TTO_MALLOC_FAIL = 9005;
const ERROR TTO_ESCAPE_BAD_HEX_FAIL = 9006;
const ERROR TTO_OUT_OF_RANGE_FAIL = 9007;
const ERROR TTO_INVALID_BASE_FAIL = 9008;
const ERROR TTO_INVALID_DIGIT_FAIL = 9009;
const ERROR TTO_FLP_CONVERT_FAIL = 9010;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
ERROR tkn_value_word(struct Token *from, struct Object *into) {
    return TTO_WORDS_VALUELESS_FAIL;
}
#pragma GCC diagnostic pop

char tto_hexchar_to_val(const char c) {
    switch (c) {
        case '0': return 0;
        case '1': return 1;
        case '2': return 2;
        case '3': return 3;
        case '4': return 4;
        case '5': return 5;
        case '6': return 6;
        case '7': return 7;
        case '8': return 8;
        case '9': return 9;
        case 'a': case 'A': return 10;
        case 'b': case 'B': return 11;
        case 'c': case 'C': return 12;
        case 'd': case 'D': return 13;
        case 'e': case 'E': return 14;
        case 'f': case 'F': return 15;
        default: return 16;
    }
}
ERROR tto_escape_hex(const char **pos, char **ret_pos, const char *const end) {
    char x_val;
    int t;
    x_val = 0;
    ++(*pos);
    if (*pos == end) {
        return TTO_ESCAPE_END_FAIL;
    }
    t = tto_hexchar_to_val(**pos);
    if (t > 15) {
        return TTO_ESCAPE_BAD_HEX_FAIL;
    }
    x_val |= t << 4;
    ++(*pos);
    if (*pos == end) {
        return TTO_ESCAPE_END_FAIL;
    }
    t = tto_hexchar_to_val(**pos);
    if (t > 15) {
        return TTO_ESCAPE_BAD_HEX_FAIL;
    }
    x_val |= t;
    **ret_pos = x_val;
    return NO_ERROR;
}
ERROR tto_escape_string(const char *const str, size_t val_len, char **out) {
    ERROR err;
    char *ret = malloc(val_len);
    if (!ret) return TTO_MALLOC_FAIL;
    char *ret_pos = ret;
    const char *const end_null = str + val_len - 1;
    for (const char *pos = str; pos < end_null; ++pos, ++ret_pos) {
        if (*pos == '\\') {
            ++pos;
            if (pos == end_null) {
                err = TTO_ESCAPE_END_FAIL;
                goto error_handler;
            }
            switch (*pos) {
                case '0': *ret_pos = '\0'; break;
                case 'a': *ret_pos = '\a'; break;
                case 'b': *ret_pos = '\b'; break;
                case 'e': *ret_pos = '\x1B'; break;
                case 'f': *ret_pos = '\f'; break;
                case 'n': *ret_pos = '\n'; break;
                case 'r': *ret_pos = '\r'; break;
                case 't': *ret_pos = '\t'; break;
                case 'v': *ret_pos = '\v'; break;
                case 'x':
                    err = tto_escape_hex(&pos, &ret_pos, end_null);
                    if (err != NO_ERROR) goto error_handler;
                    break;
                default:
                    *ret_pos = *pos;
            }
        } else {
            *ret_pos = *pos;
        }
    }
    *ret_pos = 0;
    ++ret_pos;
    char *new_ret = realloc(ret, ret_pos - ret);
    if (!new_ret) {
        *out = ret;
    } else {
        *out = new_ret;
    }
    return NO_ERROR;
error_handler:;
    free(ret);
    return err;
}
ERROR tkn_value_string(struct Token *from, struct Object *into) {
    size_t val_len = from->raw_len - 2;
    ++from->raw;
    from->raw[val_len] = '\0';
    char *escaped;
    if (tto_escape_string(from->raw, val_len, &escaped) != NO_ERROR) {
        return TTO_STRING_ESCAPE_FAIL;
    }
    ERROR err = ctno_literal(from->raw, val_len, LTL_string, NULL, into);
    --from->raw;
    tkn_free(from);
    return err;
}

ERROR tkn_value_integer(struct Token *from, struct Object *into) {
    int b = 10;
    char *raw = from->raw;
    size_t len = from->raw_len;
    bool neg = raw[0] == '-';
    if (neg || raw[0] == '+') {
        ++raw;
    }
    if (raw[0] == '0' && len > 2) {
        switch (raw[1]) {
            case 'x':
                b = 16;
                break;
            case 'o':
                b = 8;
                break;
            case 'b':
                b = 2;
                break;
            default:
                return TTO_INVALID_BASE_FAIL;
        }
    }
    if (b != 10) {
        len -= 2;
        raw += 2;
    }
    if (neg) {
        // no length mod because we "forgot" to account for it before
        --raw;
        // this is safe because we're guaranteed to have advanced by
        // at least one, so retreating by one is fine
        *raw = '-';
    }
    char *num_end;
    errno = 0;
    Integral val = strtoll(raw, &num_end, b);
    if ((val == LLONG_MAX || val == LLONG_MIN) && errno == ERANGE) {
        return TTO_OUT_OF_RANGE_FAIL;
    }
    if (num_end != raw + len - 1) {
        return TTO_INVALID_DIGIT_FAIL;
    }
    ERROR err = ctno_literal(&val, sizeof(val), LTL_integral, NULL, into);
    tkn_free(from);
    return err;
}
ERROR tkn_value_real(struct Token *from, struct Object *into) {
    char *end;
    errno = 0;
    Real val = strtod(from->raw, &end);
    if ((val == 0 || val == HUGE_VAL) && errno != 0) {
        return TTO_FLP_CONVERT_FAIL;
    }
    if (end != from->raw + from->raw_len - 1) {
        return TTO_INVALID_DIGIT_FAIL;
    }
    ERROR err = ctno_literal(&val, sizeof(val), LTL_real, NULL, into);
    tkn_free(from);
    return err;
}
ERROR tkn_value_identifier(struct Token *from, struct Object *into) {
    ERROR err = ctno_literal(from->raw + 1, from->raw_len - 1, LTL_string,
                             NULL, into);
    tkn_free(from);
    return err;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
ERROR tkn_value_regex(struct Token *from, struct Object *into) {
    // this isn't implemented in this version
    return TTO_NOT_IMPLEMENTED_FAIL;
}
#pragma GCC diagnostic pop

ERROR tkn_value(struct Token *from, struct Object *into) {
    switch (from->type) {
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
