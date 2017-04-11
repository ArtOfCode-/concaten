#include <stdio.h>

#include "tokenizer.h"
#include "data_stack.h"
#include "token_stack.h"
#include "scope_stack.h"

int normal_parse(char *filepath) {
    ERROR err;
    struct Tokenizer tknr;
    if ((err = tknr_from_filepath(filepath, &tknr)) != NO_ERROR) {
        printf("File '%s' not found. Please make sure it exists."
                       " (error "EFMT")\n", filepath, err);
        return 1;
    }
    struct TokenStack tst;
    if ((err = tst_new(tknr, &tst)) != NO_ERROR) {
        printf("Failed to initialize token stack (error "EFMT")\n", err);
        return 2;
    }
    struct DataStack dst;
    if ((err = dst_new(&dst)) != NO_ERROR) {
        printf("Failed to initialize data stack (error "EFMT")\n", err);
        return 3;
    }
    struct ScopeStack sst;
    if ((err = sst_new(8, &sst)) != NO_ERROR) {
        printf("Failed to initialize scope stack (error "EFMT")\n", err);
        return 3;
    }
    struct Token parsing;
    while ((err = tst_pop(&tst, &parsing)) == NO_ERROR) {
        if (parsing.type == TKN_WORD) {
            struct Runnable *candidates;
            size_t cands_count;
            sst_get_all(sst, parsing.raw, &candidates, &cands_count);
            for (size_t i = 0; i < cands_count; ++i) {
                sst_save_state(&sst);
                tst_save_state(&tst);
                struct DataStack dst_c;
                if ((err = dst_copy(dst, &dst_c)) != NO_ERROR) {
                    printf("Failed to copy data stack (error "EFMT")\n", err);
                }
                
                rn_run(candidates[i], &dst, &sst, &tst);
                
                // TODO finish this
            }
        }
    }
    if (err != TST_POP_EMPTY_FAIL) {
        printf("Tried to get token and got unexpected error "EFMT"\n", err);
        return 4;
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
        if (res != 0) { return res; }
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
