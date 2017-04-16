#include <stdio.h>

#include "tokenizer.h"
#include "data_stack.h"
#include "token_stack.h"
#include "scope_stack.h"
#include "object.h"
#include "stl.h"

bool normal_parse(char *filepath) {
    ERROR err;
    struct Tokenizer tknr;
    if ((err = tknr_from_filepath(filepath, &tknr)) != NO_ERROR) {
        fprintf(stderr, "%s could not be tokenized: "EFMT"%s\n", filepath, err,
                err == TKNR_CTOR_FILE_FOPEN_FAIL ? " (is it readable?)" : "");
        return false;
    }
    struct TokenStack tst;
    if ((err = tst_new(tknr, &tst)) != NO_ERROR) {
        fprintf(stderr, "Failed to initialize token stack: "EFMT"\n", err);
        return false;
    }
    struct DataStack dst;
    if ((err = dst_new(&dst)) != NO_ERROR) {
        fprintf(stderr, "Failed to initialize data stack: "EFMT"\n", err);
        return false;
    }
    struct ScopeStack sst;
    if ((err = sst_new(8, &sst)) != NO_ERROR) {
        fprintf(stderr, "Failed to initialize scope stack: "EFMT"\n", err);
        return false;
    }
    sst.layers[0] = global_funcs;
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
                if ((err = dst_peek(&dst, &top)) != NO_ERROR) {
                    eprint("Failed to retrieve data top: "EFMT"\n", err);
                    return false;
                }
                if (mm_is_key(*top->methods, ctkn.raw)) {
                    struct Runnable trying;
                    if ((err = mm_get(*top->methods, ctkn.raw, &trying))
                            != NO_ERROR) {
                        eprint("Failed to get existing key: "EFMT"\n", err);
                        return false;
                    }
                    err = rn_run(trying, &dst, &sst, &tst);
                    if (err == ARGUMENT_MISMATCH_FAIL) {
                        /* nop */
                    } else if (err != NO_ERROR) {
                        eprint("Failed to run: "EFMT"\n", err);
                        return false;
                    } else {
                        continue;
                    }
                }
            }
            
            // then we look at the scopes
            struct Runnable *candidates;
            size_t cands_count;
            sst_get_all(sst, ctkn.raw, &candidates, &cands_count);
            if (cands_count == 0) {
                eprint("No word found.\n");
                return false;
            }
            for (size_t i = 0; i < cands_count; ++i) {
                if ((err = sst_save_state(&sst)) != NO_ERROR) {
                    eprint("Failed to save scope stack: "EFMT"\n", err);
                    return false;
                }
                if ((err = tst_save_state(&tst)) != NO_ERROR) {
                    eprint("Failed to save token stack: "EFMT"\n", err);
                    return false;
                }
                struct DataStack dst_c;
                if ((err = dst_copy(dst, &dst_c)) != NO_ERROR) {
                    eprint("Failed to copy data stack: "EFMT"\n", err);
                    return false;
                }
                
                err = rn_run(candidates[i], &dst, &sst, &tst);
                if (err == ARGUMENT_MISMATCH_FAIL) {
                    if ((err = tst_restore_state(&tst)) != NO_ERROR) {
                        eprint("Failed to restore token stack: "EFMT"\n", err);
                    }
                    if ((err = sst_restore_state(&sst)) != NO_ERROR) {
                        eprint("Failed to restore scope stack: "EFMT"\n", err);
                    }
                    if ((err = dst_copy(dst_c, &dst)) != NO_ERROR) {
                        eprint("Failed to restore data stack: "EFMT"\n", err);
                    }
                    continue;
                } else if (err != NO_ERROR) {
                    eprint("Failed to run: "EFMT"\n", err);
                    return false;
                } else {
                    break;
                }
            }
            if (err == ARGUMENT_MISMATCH_FAIL) {
                eprint("Argument types all failed to match.\n");
                return false;
            }
        } else {
            struct Object *new_val = malloc(sizeof(*new_val));
            if (!new_val) {
                eprint("Failed to allocate space for new object.\n");
            }
            if ((err = tkn_value(&ctkn, new_val)) != NO_ERROR) {
                eprint("Failed to get token's value: "EFMT"\n", err);
                return false;
            }
            if ((err = dst_push(&dst, new_val)) != NO_ERROR) {
                eprint("Failed to push token's value: "EFMT"\n", err);
                return false;
            }
        }
    }
    if (err != TST_POP_EMPTY_FAIL) {
        fprintf(stderr, "Failed to get next token: "EFMT"\n", err);
        return false;
    }
    return true;
}

int main(int argc, char **argv) {
    if (argc <= 0) {
        fputs("Usage: /path/to/concaten paths...", stderr);
        return 1;
    } else if (argc == 1) {
        fprintf(stderr, "Usage: \"%s\" paths...\n", argv[0]);
        return 1;
    }
    ERROR err;
    if ((err = init_stl()) != NO_ERROR) {
        fprintf(stderr, "Failed to init standard library: "EFMT"\n", err);
    }
    for (int argidx = 1; argidx < argc; ++argidx) {
        int res = normal_parse(argv[argidx]);
        if (!res) {
            return argidx;
        }
    }
    return 0;
}
