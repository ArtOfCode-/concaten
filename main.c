//#include "callable.h"
//#include "stacks.h"
//#include "errors.h"
//
//#include <stdlib.h>
//#include <stdbool.h>
//
//char *get_message(scope_stack_t sst, object_t top) {
//    callable_t *
//}
//
//ctn_err_t try_candidates(callable_t *callables, size_t count,
//                         data_stack_t dst, scope_stack_t sst, token_stack_t tst) {
//    bool success = false;
//    data_stack_t dst_backup = dst_copy(dst);
//    scope_stack_t sst_backup = sst_copy(sst);
//    token_stack_t tst_backup = tst_copy(tst);
//    for (size_t i = 0; i < count; ++i) {
//        // the copies are freed by the function call
//        ctn_err_t err = ccl_call(callables[i], dst, sst, tst);
//        if (err == CTN_E_TYPE_MISMATCH) {
//            dst_restore(dst, dst_copy(dst_backup));
//            sst_restore(sst, sst_copy(sst_backup));
//            tst_restore(tst, tst_copy(tst_backup));
//            continue;
//        } else { // includes CTN_E_SUCCESS
//            dst_free(dst_backup);
//            sst_free(sst_backup);
//            tst_free(tst_backup);
//            return err;
//        }
//    }
//    return CTN_E_NO_WORD;
//}
//
//int main() {
//    char *code = "1 2.3 4.5e6 7e8 0x9";
//    Tokenizer tokenizer = tknr_from_string(code, "memory");
//    data_stack_t dst = dst_empty();
//    scope_stack_t sst = sst_globals();
//    token_stack_t tst = tst_from_tokenizer(tokenizer);
//    Token next;
//    while (next = tst_pop_token(tst)) {
//        char *tkn_err_res = tkn_err(next);
//        if (tkn_err_res) {
//            return ctn_error(CTN_E_TOKENIZATION_ERROR, next, tkn_err_res);
//        }
//        if (tkn_type(next) != TKN_WORD && !tkn_err(next)) {
//            dst_push(tkn_ctn_value(next));
//        } else {
//            callable_t *callables;
//            size_t count = sst_search(sst, tkn_raw(next), &callables);
//            if (count == 0) {
//                ctn_err_t err = try_candidates(callables, count, dst, sst, tst);
//                if (err != CTN_E_SUCCESS) {
//                    return ctn_error(err, next, get_message(sst, dst_pop(dst)));
//                }
//            }
//            return ctn_error(CTN_E_NO_WORD, next, NULL);
//        }
//        tkn_free(next);
//    }
//    tknr_free(tokenizer);
//    return 0;
//}

#include "tokenizer.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int main() {
    char *test_strings[] = {
            "",
            "\"hello, world\" 123.4e5 0x1eF 0b1001 0o7513",
//            "\"string here\" 123.4e5"
    };
    size_t test_count = sizeof(test_strings) / sizeof(char *);
    char *origin = malloc(strlen("test case #0") * sizeof(char));
    strcpy(origin, "test case #0");
    --origin[11];
#define _printf(...) printf(" " __VA_ARGS__)
    for (int i = 0; i < test_count; ++i) {
        ++origin[11];
        puts(origin);
        Tokenizer tknr = tknr_from_string(test_strings[i], origin);
        if (tknr_err(tknr)) {
            _printf("Error %d occured when initializing\n", tknr_err(tknr));
            continue;
        }
        Token next;
        while ((next = tknr_next(tknr)) != NULL) {
            _printf("`%s` (%d) at %s@%zu:%zu\n", tkn_raw(next), tkn_type(next),
                   tkn_origin(next), tkn_line(next), tkn_index(next));
        }
        if (tknr_err(tknr) && !tknr_end(tknr)) {
            _printf("Error %d occured in tokenizer", tknr_err(tknr));
        }
        tknr_free(tknr);
    }
}