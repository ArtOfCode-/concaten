#include <stdio.h>

int main() {
    // This is not going to be filled out until v0.7 -- for the code that's
    // actually getting exectued, see test.c, its `int main()`, and the files
    // in tests/
    puts("Hello, World!");
    // in pseudocode, though, this is what's going to happen:
    /*
     foreach (path in cli-args) {
       tknr = tknr_from_file(path);
       tokens = ts_new(tknr);
       data = ds_new();
       scopes = ss_new();
       while (!tokens.empty()) {
         operating_on = tokens.pop();
         if (operating_on.is_word()) {
           error = get_word(operating_on.value)(tokens, data, scopes);
           if (error) {
             printf("%s: %s\n", error.typename, error.message)
             foreach (line in error.stacktrace) {
               printf("  %s\n", line);
             }
             return error.code;
           }
         } else {
           data.push(operating_on.value);
         }
         // TODO anything else here?
       }
     }
     */
}
