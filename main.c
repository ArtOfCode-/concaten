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
    if ((err = stl_init()) != NO_ERROR) {
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
        fprintf(stderr, "%zu@%zu:%zu: ", ctkn.origin, ctkn.line, ctkn.index); \
        fprintf(stderr, __VA_ARGS__); \
    } while(0)
    struct Token ctkn;
    while ((err = tst_pop(&tst, &ctkn)) == NO_ERROR) {
        if (ctkn.type == TKN_WORD) {
            struct Runnable *candidates;
            size_t cands_count;
            sst_get_all(sst, ctkn.raw, &candidates, &cands_count);
            if (cands_count == 0) {
                eprint("No word '%s' found.\n", ctkn.raw);
                return 6;
            }
            for (size_t i = 0; i < cands_count; ++i) {
                if ((err = sst_save_state(&sst)) != NO_ERROR) {
                    eprint("Failed to save scope stack:"EFMT"\n", err);
                    return 7;
                }
                if ((err = tst_save_state(&tst)) != NO_ERROR) {
                    eprint("Failed to save token stack:"EFMT"\n", err);
                    return 8;
                }
                struct DataStack dst_c;
                if ((err = dst_copy(dst, &dst_c)) != NO_ERROR) {
                    eprint("Failed to copy data stack:"EFMT"\n", err);
                    return 9;
                }
                
                err = rn_run(candidates[i], &dst, &sst, &tst);
                if (err == ARGUMENT_TYPE_MISMATCH_FAIL) {
                    continue;
                } else if (err != NO_ERROR) {
                    eprint("An error occurred while running %s: "EFMT"\n",
                           ctkn.raw, err);
                    return 10;
                } else {
                    break;
                }
            }
        } else {
            struct Object new_val;
            tkn_value(&ctkn, &new_val);
            dst_push(&dst, &new_val);
        }
    }
    if (err != TST_POP_EMPTY_FAIL) {
        printf("Tried to get token and got unexpected error "EFMT"\n", err);
        return 11;
    }
    return 0;
}

int main(int argc, char **argv) {
    if (argc == 1) {
        puts("Usage: ./concaten paths...");
        return 1;
    }
    for (int argidx = 1; argidx < argc; ++argidx) {
        int res = normal_parse(argv[argidx]);
        if (res != 0) {
            return argidx * 10 + res;
        }
    }
    return 0;
    /* //Pseudocode
      foreach (path in cli-args) {
        tknr = tknr_from_file(path);
        tokens = ts_new(tknr);
        data = ds_new();
        scopes = sst_new();
        while (!tokens.empty()) {
          operating_on = tokens.pop();
          if (operating_on.is_word()) {
            candidates = scopes.get_words_with_name(operating_on.value);
            save_stack_states();
            foreach (c in candidates) {
              error = c(tokens, data, scopes);
              if (error) {
                if (error.type == invalid_argument_type) {
                  restore_stack_states();
                  continue;
                } else {
                  printf("%s: %s\n", error.type, error.message);
                  foreach (line in error.stacktrace) {
                    printf("  %s\n", line);
                  }
                  return error.code;
                }
              } else {
                break;
              }
            }
          } else {
            data.push(operating_on.value);
          }
        }
      }
      return 0;
    */
}
