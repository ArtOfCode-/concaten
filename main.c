#include <stdio.h>

#include "tokenizer.h"
#include "data_stack.h"
#include "token_stack.h"
#include "scope_stack.h"
#include "object.h"
#include "stl.h"

#define mms \
X( 0) X( 1) X( 2) X( 3) X( 4) X( 5) X( 6) X( 7) X( 8) X( 9) \
X(10) X(11) X(12) X(13) X(14) X(15) X(16) X(17) X(18) X(19) \
X(20) X(21) X(22) X(23) X(24) X(25) X(26) X(27) X(28) X(29) \
X(30) X(31) X(32) X(33) X(34) X(35) X(36) X(37) X(38) X(39) \
X(40) X(41) X(42) X(43) X(44) X(45) X(46) X(47) X(48) X(49) \
X(50) X(51) X(52) X(53) X(54) X(55) X(56) X(57) X(58) X(59) \
X(60) X(61) X(62) X(63) X(64) X(65) X(66) X(67) X(68) X(69) \
X(70) X(71) X(72) X(73) X(74) X(75) X(76) X(77) X(78) X(79) \
X(80) X(81) X(82) X(83) X(84) X(85) X(86) X(87) X(88) X(89) \
X(90) X(91) X(92) X(93) X(94) X(95) X(96) X(97) X(98) X(99) \

#define X(n) \
ERROR tst_pop_repl_##n(struct TokenStack *t, struct Token *o) { \
    return tst_pop(t, o); \
}
mms
#undef X

ERROR (*tsts[])(struct TokenStack *, struct Token *) = {0};
ERROR tst_pop_repl(struct TokenStack *t, struct Token *o, size_t idx) {
    #define X(n) tst_pop_repl_##n,
    ERROR (*tsts[])(struct TokenStack *, struct Token *) = {
        mms
    };
    #undef X
    return tsts[idx](t, o);
}

bool parse(struct Tokenizer tknr, struct MethodMap globals) {
    ERROR err;
    struct TokenStack tst;
    if ((err = tst_new(tknr, &tst)) != NO_ERROR) {
        fprintf(stderr, "Failed to initialize token stack: "EFMT"\n", err);
        return false;
    }
    struct DataStack dst;
    if ((err = dst_new(&dst)) != NO_ERROR) {
        fprintf(stderr, "Failed to initialize data stack: "EFMT"\n", err);
        tst_free(&tst);
        return false;
    }
    struct ScopeStack sst;
    if ((err = sst_new(8, globals, &sst)) != NO_ERROR) {
        fprintf(stderr, "Failed to initialize scope stack: "EFMT"\n", err);
        dst_free(&dst);
        tst_free(&tst);
        return false;
    }
    #define eprint(...) do {\
        fprintf(stderr, "%s@%zu:%zu: Error while parsing '%s': ", \
            ctkn.origin, ctkn.line, ctkn.index, ctkn.raw); \
        fprintf(stderr, __VA_ARGS__); \
    } while(0)
    struct Token ctkn;
    size_t idx = 0;
    while ((err = tst_pop_repl(&tst, &ctkn, idx++)) == NO_ERROR) {
        if (ctkn.type == TKN_WORD) {
            // first, look for methods in the top of stack
            if (!dst_empty(dst)) {
                struct Object *top;
                if ((err = dst_peek(&dst, &top)) != NO_ERROR) {
                    eprint("Failed to retrieve data top: "EFMT"\n", err);
                    goto error;
                }
                if (mm_is_key(*top->methods, ctkn.raw)) {
                    struct Runnable trying;
                    if ((err = mm_get(*top->methods, ctkn.raw, &trying))
                            != NO_ERROR) {
                        eprint("Failed to get existing key: "EFMT"\n", err);
                        goto error;
                    }
                    err = rn_run(trying, &dst, &sst, &tst);
                    if (err == ARGUMENT_MISMATCH_FAIL) {
                        /* nop */
                    } else if (err != NO_ERROR) {
                        eprint("Failed to run: "EFMT"\n", err);
                        goto error;
                    } else {
                        continue;
                    }
                }
            }
            
            // then we look at the scopes
            struct Runnable *candidates;
            size_t cands_count;
            if ((err = sst_get_all(sst, ctkn.raw, &candidates, &cands_count))
                    != NO_ERROR) {
                eprint("Failed to get candidates: "EFMT"\n", err);
            }
            if (cands_count == 0) {
                eprint("No word found.\n");
                free(candidates);
                goto error;
            }
            for (size_t i = 0; i < cands_count; ++i) {
                if ((err = sst_save_state(&sst)) != NO_ERROR) {
                    eprint("Failed to save scope stack: "EFMT"\n", err);
                    free(candidates);
                    goto error;
                }
                if ((err = tst_save_state(&tst)) != NO_ERROR) {
                    eprint("Failed to save token stack: "EFMT"\n", err);
                    free(candidates);
                    goto error;
                }
                struct DataStack dst_c;
                if ((err = dst_copy(dst, &dst_c)) != NO_ERROR) {
                    eprint("Failed to copy data stack: "EFMT"\n", err);
                    free(candidates);
                    goto error;
                }
                
                err = rn_run(candidates[i], &dst, &sst, &tst);
                if (err == ARGUMENT_MISMATCH_FAIL) {
                    if ((err = tst_restore_state(&tst)) != NO_ERROR) {
                        eprint("Failed to restore token stack: "EFMT"\n", err);
                        free(candidates);
                        goto error;
                    }
                    if ((err = sst_restore_state(&sst)) != NO_ERROR) {
                        eprint("Failed to restore scope stack: "EFMT"\n", err);
                        free(candidates);
                        goto error;
                    }
                    dst_free(&dst);
                    if ((err = dst_copy(dst_c, &dst)) != NO_ERROR) {
                        eprint("Failed to restore data stack: "EFMT"\n", err);
                        free(candidates);
                        goto error;
                    }
                    dst_free(&dst_c);
                    continue;
                } else if (err != NO_ERROR) {
                    eprint("Failed to run: "EFMT"\n", err);
                    free(candidates);
                    goto error;
                } else {
                    break;
                }
            }
            free(candidates);
            if (err == ARGUMENT_MISMATCH_FAIL) {
                eprint("Argument types all failed to match.\n");
                free(candidates);
                goto error;
            } else if (err != NO_ERROR) {
                eprint("Candidate returned error: "EFMT"\n", err);
                goto error;
            }
            tkn_free(&ctkn);
        } else {
            struct Object *new_val = malloc(sizeof(*new_val));
            if (!new_val) {
                eprint("Failed to allocate space for new object.\n");
                goto error;
            }
            if ((err = tkn_value(&ctkn, new_val)) != NO_ERROR) {
                eprint("Failed to get token's value: "EFMT"\n", err);
                goto error;
            }
            if ((err = dst_push(&dst, new_val)) != NO_ERROR) {
                eprint("Failed to push token's value: "EFMT"\n", err);
                goto error;
            }
            ctno_free(new_val); // we're relinquishing control of it
        }
    }
    if (err != TST_POP_EMPTY_FAIL) {
        fprintf(stderr, "Failed to get next token: "EFMT"\n", err);
        goto error;
    }
    dst_free(&dst);
    sst_free(&sst);
    tst_free(&tst);
    return true;
  error:;
    dst_free(&dst);
    sst_free(&sst);
    tst_free(&tst);
    return false;
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
    if (argv[1][0] == '-') {
        if (argv[1][1] == 'e') {
            if (argc == 2) {
                puts("-e expects an argument; none provided.");
            }
            if ((err = init_stl(argc - 3, argv + 3)) != NO_ERROR) {
                fprintf(stderr, "Failed to init stl: "EFMT"\n", err);
            }
            struct Tokenizer tknr;
            if ((err = tknr_from_string(argv[2], "<cli>", &tknr))
                != NO_ERROR) {
                fprintf(stderr, "Couldn't tokenize `%s`: "EFMT,
                        argv[1], err);
                if (err == TKNR_CTOR_FILE_FOPEN_FAIL) {
                    puts(" (is it readable?)");
                } else {
                    puts("");
                }
                return 1;
            }
            bool res = parse(tknr, global_funcs);
            tknr_free(&tknr);
            if (!res) {
                return 1;
            }
        }
    } else {
        if ((err = init_stl(argc - 3, argv + 3)) != NO_ERROR) {
            fprintf(stderr, "Failed to init stl: "EFMT"\n", err);
        }
        struct Tokenizer tknr;
        if ((err = tknr_from_filepath(argv[1], &tknr)) != NO_ERROR) {
            fprintf(stderr, "Couldn't tokenize %s: "EFMT, argv[1], err);
            if (err == TKNR_CTOR_FILE_FOPEN_FAIL) {
                puts(" (is it readable?)");
            } else {
                puts("");
            }
            return 1;
        }
        bool res = parse(tknr, global_funcs);
        tknr_free(&tknr);
        if (!res) {
            return 1;
        }
    }
    free_stl();
    return 0;
}
