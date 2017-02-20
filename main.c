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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "tokenizer.h"

int main() {
    // TODO test edge cases
    char *test_cases[] = {
            "",
            "\"string\" 42 0x1Fe94\n"
            "0b11001 0o127635 1.2e3\n"
            ":foobar foobar2\n"
            "r/asdf boofar/xgi"
    };
    size_t test_count = sizeof(test_cases) / sizeof(char *);
    for (size_t i = 0; i < test_count; ++i) {
        printf("Test case #%zu:\n", i + 1);
        char *test_string = test_cases[i];
        Tokenizer tknr = tknr_from_string(test_string, "test");
        int err = tknr_err(tknr);
        if (err != 0) {
            printf(" Error %d occured when initializing.\n", err);
            continue;
        }
        Token next;
        while ((next = tknr_next(tknr)) != NULL) {
            printf(" `%s` (%d) at %s@%zu:%zu\n", tkn_raw(next), tkn_type(next),
                    tkn_origin(next), tkn_line(next), tkn_index(next));
        }
        err = tknr_err(tknr);
        if (err != 0 && !tknr_end(tknr)) {
            printf(" Error %d occured while tokenizing.\n", err);
        }
        tknr_free(tknr);
    }
    puts("Finished running all test cases.");
}