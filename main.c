#include <stdio.h>

int main() {
    // This is not going to be filled out until v0.7 -- for the code that's
    // actually getting exectued, see test.c, its `int main()`, and the files
    // in tests/
    puts("Hello, World!");
    // in pseudocode, though, this is what's going to happen:
  /*
    // TODO Something to handle CLI args
    foreach (path in cli-args) {
      tknr = tknr_from_file(path);
      tokens = ts_new(tknr);
      data = ds_new();
      scopes = ss_new();
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
