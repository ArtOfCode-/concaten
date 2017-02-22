#include <stdlib.h>
#include <stdio.h>
#include "stringbuilder.h"

void dbg_print(StringBuilder sb) {
    printf("%zu chars: %s\n", sb_size(sb), sb_as_string(sb));
}

int main() {
    StringBuilder sb = sb_new(4);
    sb_append(sb, 'a');
    sb_append(sb, 'b');
    sb_append(sb, 'c');
    dbg_print(sb);
    sb_append(sb, 'd');
    dbg_print(sb);
    sb_append(sb, 'e');
    dbg_print(sb);
    sb_append(sb, 'f');
    sb_append(sb, 'g');
    sb_append(sb, 'h');
    dbg_print(sb);
    sb_append(sb, 'a');
    sb_append(sb, 'b');
    sb_append(sb, 'c');
    dbg_print(sb);
    sb_append(sb, '1');
    dbg_print(sb);
    char *str = sb_free_copy(sb);
    if (!str) {
        puts("Error!");
    } else {
        puts(str);
    }
}