#include "errors.h"

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int ctn_error(ctn_err_t errcode, token_t tkn, char *detail) {
    char *message;
    bool should_free = false;
    char *tkn_name = tkn_raw(tkn);
    switch (errcode) {
        size_t size; // used in multiple places; moved to the top so I can use it
        case CTN_E_SUCCESS:
            message = "Trying to exit on success condition";
            break;
        case CTN_E_NO_WORD:
            size = strlen(tkn_name) + 14 + 1;
            message = malloc(size * sizeof(char));
            if (!message) {
                message = "[memory failure] NO_WORD";
                break;
            }
            should_free = true;
            snprintf(message, size, "No word named %s", tkn_raw(tkn));
            break;
        case CTN_E_TYPE_MISMATCH:
            size = strlen(tkn_name) + 32 + 1;
            message = malloc(size * sizeof(char));
            snprintf(message, size, "Invalid types on stack for word %s", tkn_name);
            break;
        case CTN_E_TOKENIZATION_ERROR:
            message = detail;
            should_free = true;
        case CTN_E_USER_DEFINED_ERROR:
            message = detail;
            should_free = true;
            break;
        default:
            size = 10 + 19 + 1; // max int length + extra chars + null
            message = malloc(size * sizeof(char));
            if (!message) {
                message = "[memory failure] UNKNOWN ERROR";
                break;
            }
            should_free = true;
            snprintf(message, size, "Unknown error code %x", errcode);
            break;
    }
    fprintf(stderr, "%s at %s@%zu:%zu (%s)", message, tkn_origin(tkn),
            tkn_line(tkn), tkn_index(tkn), tkn_raw(tkn));
    if (should_free) free(message);
    tkn_free(tkn);
    return errcode;
}