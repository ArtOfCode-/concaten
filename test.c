#include "tests.h"

#include <stdio.h>

int main() {
    puts("Testing Tokenizer:");
    test_tokenizer();
    puts("Testing PropMap:");
    test_prop_map();
    puts("Testing MethodMap:");
    test_method_map();
    puts("Testing Object:");
    test_object();
    puts("Testing DataStack:");
    test_data_stack();
    puts("Testing TokenStack:");
    test_token_stack();
    puts("Done with all tests");
}
