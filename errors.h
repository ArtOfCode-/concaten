#ifndef CONCATEN_ERRORS_H
#define CONCATEN_ERRORS_H

#include "tokenizer.h"

enum ctn_err_e {
    CTN_E_SUCCESS, // included for the sake of completeness
    CTN_E_NO_WORD,
    CTN_E_TYPE_MISMATCH,
    CTN_E_TOKENIZATION_ERROR,
    CTN_E_USER_DEFINED_ERROR,
};
typedef enum ctn_err_e ctn_err_t;
int ctn_error(ctn_err_t errcode, Token tkn, char *detail);

#endif //CONCATEN_ERRORS_H
