#include "error.h"
#include "tokenizer.h"
#include "object.h"

const ERROR TTO_UNKNOWN_TYPE_FAIL = 9001;
const ERROR TTO_NOT_IMPLEMENTED_FAIL = 9002;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
ERROR tkn_value_word(const struct Token from, struct Object **into) {
    *into = NULL;
    return NO_ERROR;
}

ERROR tkn_value_string(const struct Token from, struct Object **into) {
    return TTO_NOT_IMPLEMENTED_FAIL;
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