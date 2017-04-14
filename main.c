#include <stdio.h>

#include "tokenizer.h"
#include "data_stack.h"
#include "token_stack.h"
#include "scope_stack.h"
#include "object.h"
#include "stl.h"

int normal_parse(char *filepath) {
    ERROR err;
    struct Tokenizer tknr;
    if ((err = init_stl()) != NO_ERROR) {
        printf("Failed to initialize standard library: "EFMT"\n", err);
        return 1;
    }
    if ((err = tknr_from_filepath(filepath, &tknr)) != NO_ERROR) {
        printf("%s could not be tokenized: "EFMT"%s\n", filepath, err,
                err == TKNR_CTOR_FILE_FOPEN_FAIL ? " (is it readable?)" : "");
        return 2;
    }
    struct TokenStack tst;
    if ((err = tst_new(tknr, &tst)) != NO_ERROR) {
        printf("Failed to initialize token stack: "EFMT"\n", err);
        return 3;
    }
    struct DataStack dst;
    if ((err = dst_new(&dst)) != NO_ERROR) {
        printf("Failed to initialize data stack: "EFMT"\n", err);
        return 4;
    }
    struct ScopeStack sst;
    if ((err = sst_new(8, &sst)) != NO_ERROR) {
        printf("Failed to initialize scope stack: "EFMT"\n", err);
        return 5;
    }
    #define eprint(...) do {\
        fprintf(stderr, "%s@%zu:%zu: Error while parsing '%s': ", \
            ctkn.origin, ctkn.line, ctkn.index, ctkn.raw); \
        fprintf(stderr, __VA_ARGS__); \
    } while(0)
    struct Token ctkn;
    while ((err = tst_pop(&tst, &ctkn)) == NO_ERROR) {
        if (ctkn.type == TKN_WORD) {
            // first, look for methods in the top of stack
            if (!dst_empty(dst)) {
                struct Object *top;
                if ((err = dst_pop(&dst, &top)) != NO_ERROR) {
                    eprint("Failed to retrieve data top: "EFMT"\n", err);
                    return 6;
                }
                if (mm_is_key(*top->methods, ctkn.raw)) {
                    struct Runnable trying;
                    if ((err = mm_get(*top->methods, ctkn.raw, &trying))
                            != NO_ERROR) {
                        eprint("Failed to get existing key: "EFMT"\n", err);
                        return 7;
                    }
                    
                }
            }
            
            // then we look at the scopes
            struct Runnable *candidates;
            size_t cands_count;
            sst_get_all(sst, ctkn.raw, &candidates, &cands_count);
            if (cands_count == 0) {
                eprint("No word '%s' found.\n", ctkn.raw);
                return 8;
            }
            for (size_t i = 0; i < cands_count; ++i) {
                if ((err = sst_save_state(&sst)) != NO_ERROR) {
                    eprint("Failed to save scope stack: "EFMT"\n", err);
                    return 9;
                }
                if ((err = tst_save_state(&tst)) != NO_ERROR) {
                    eprint("Failed to save token stack: "EFMT"\n", err);
                    return 10;
                }
                struct DataStack dst_c;
                if ((err = dst_copy(dst, &dst_c)) != NO_ERROR) {
                    eprint("Failed to copy data stack: "EFMT"\n", err);
                    return 11;
                }
                
                err = rn_run(candidates[i], &dst, &sst, &tst);
                if (err == ARGUMENT_TYPE_MISMATCH_FAIL) {
                    continue;
                } else if (err != NO_ERROR) {
                    eprint("Failed to run: "EFMT"\n", err);
                    return 12;
                } else {
                    break;
                }
            }
        } else {
            struct Object new_val;
            if ((err = tkn_value(&ctkn, &new_val)) != NO_ERROR) {
                eprint("Failed to get token's value: "EFMT"\n", err);
                return 13;
            }
            if ((err = dst_push(&dst, &new_val)) != NO_ERROR) {
                eprint("Failed to push token's value: "EFMT"\n", err);
                return 14;
            }
        }
    }
    if (err != TST_POP_EMPTY_FAIL) {
        printf("Failed to get next token: "EFMT"\n", err);
        return 15;
    }
    return 0;
}

int main(int argc, char **argv) {
    if (argc <= 0) {
        puts("Usage: /path/to/concaten paths...");
        return 1;
    } else if (argc == 1) {
        printf("Usage: \"%s\" paths...\n", argv[0]);
        return 1;
    }
    for (int argidx = 1; argidx < argc; ++argidx) {
        int res = normal_parse(argv[argidx]);
        if (res != 0) {
            return argidx * 10 + res;
        }
    }
    return 0;
}
