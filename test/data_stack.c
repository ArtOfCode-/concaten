#include "../tests.h"

#include "../data_stack.h"

#define str2(x) # x
#define str(x) str2(x)
#define LINE str(__LINE__) ": "

void test_data_stack() {
    struct DataStack dst1 = dst_new();
    int val = 12;
    struct Object foo = ctno_from(val, NULL); ++val;
    struct Object bar = ctno_from(val, NULL); ++val;
    struct Object baz = ctno_from(val, NULL); ++val;
    struct Object wuf = ctno_from(val, NULL); ++val;
    dst_push(&dst1, &foo);
    dst_push(&dst1, &bar);
    dst_push(&dst1, &baz);
    dst_push(&dst1, &wuf);
    struct DataStack dst2 = dst_copy(dst1);
    struct Object *dst2_got = dst_pop(&dst2);
    printf(LINE "2 got expected value: %c\n", dst2_got ? (dst2_got == &wuf ? 't' : 'f') : 'n');
    dst2_got = dst_pop(&dst2);
    printf(LINE "2 got expected value: %c\n", dst2_got ? (dst2_got == &baz ? 't' : 'f') : 'n');
    dst2_got = dst_pop(&dst2);
    printf(LINE "2 got expected value: %c\n", dst2_got ? (dst2_got == &bar ? 't' : 'f') : 'n');
    dst_push(&dst2, &wuf);
    dst_push(&dst2, &baz);
    dst2_got = dst_pop(&dst2);
    printf(LINE "2 got expected value: %c\n", dst2_got ? (dst2_got == &baz ? 't' : 'f') : 'n');
    dst_push(&dst1, &foo);
    dst2_got = dst_pop(&dst2);
    printf(LINE "2 got expected value: %c\n", dst2_got ? (dst2_got == &wuf ? 't' : 'f') : 'n');
    dst2_got = dst_pop(&dst2);
    dst_push(&dst1, &bar);
    printf(LINE "2 got expected value: %c\n", dst2_got ? (dst2_got == &foo ? 't' : 'f') : 'n');
    dst2_got = dst_pop(&dst2);
    printf(LINE "2 got expected value: %c\n", dst2_got ? 'f' : 't');
    puts("");
    struct Object *dst1_got = dst_pop(&dst1);
    printf(LINE "1 got expected value: %c\n", dst1_got ? (dst1_got == &bar ? 't' : 'f') : 'n');
    dst1_got = dst_pop(&dst1);
    printf(LINE "1 got expected value: %c\n", dst1_got ? (dst1_got == &foo ? 't' : 'f') : 'n');
    dst1_got = dst_pop(&dst1);
    printf(LINE "1 got expected value: %c\n", dst1_got ? (dst1_got == &wuf ? 't' : 'f') : 'n');
    dst1_got = dst_pop(&dst1);
    printf(LINE "1 got expected value: %c\n", dst1_got ? (dst1_got == &baz ? 't' : 'f') : 'n');
    dst1_got = dst_pop(&dst1);
    printf(LINE "1 got expected value: %c\n", dst1_got ? (dst1_got == &bar ? 't' : 'f') : 'n');
    dst1_got = dst_pop(&dst1);
    printf(LINE "1 got expected value: %c\n", dst1_got ? (dst1_got == &foo ? 't' : 'f') : 'n');
    dst1_got = dst_pop(&dst1);
    printf(LINE "1 got expected value: %c\n", dst1_got ? 'f' : 't');
    
}