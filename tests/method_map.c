#include "../tests.h"

#include "../method_map.h"

void intermediate_test(const struct MethodMap mm) {
    char *names[] = {
            "first", "second", "third", "fourth", "fifth",
            "eleventh", "twelfth", "bar", "woof", "sixteenth",
            "nineteenth", "twentieth", "foo", "eighteenth"
    };
    size_t amt = sizeof(names) / sizeof(char *);
    for (size_t i = 0; i < amt; ++i) {
        MM_FUNC_TYPE res = mm_get(mm, names[i]);
        printf("Result for %-2zu (%-10s) = ", i, names[i]);
        if (res) {
            res((int) i, names[i]);
        } else {
            puts("N/A");
        }
    }
}

#define str2(n) #n
#define str(n) str2(n)
#define test_f(num) \
    void f##num(int i, char *s) { printf("f" str(num) ": %d; %s\n", i, s); }

test_f(0)  test_f(1)  test_f(2)  test_f(3)  test_f(4)
test_f(5)  test_f(6)  test_f(7)  test_f(8)  test_f(9)
test_f(10) test_f(11) test_f(12) test_f(13) test_f(14)
test_f(15) test_f(16) test_f(17) test_f(18) test_f(19)

#define __mm_set(a,b,c) printf("set %s: %d\n",b,mm_set(a,b,c));
#define __mm_remove(a,b) printf("remove %s: %d\n",b,mm_remove(a,b));

void test_method_map() {
    // TODO convert to success counter
    // very low number of buckets initially so it has to rehash
    // ...eventually. I hate the pigeonhole principle.
    struct MethodMap mm = mm_new(2);
    __mm_set(&mm, "first", f0);
    __mm_set(&mm, "second", f1);
    __mm_set(&mm, "third", f2);
    __mm_set(&mm, "fourth", f3);
    __mm_set(&mm, "fifth", f4);
    printf("fifth %s key\n", mm_is_key(mm, "fifth") ? "is" : "isn't");
    printf("bar %s key\n", mm_is_key(mm, "bar") ? "is" : "isn't");
    printf("tenth %s key\n", mm_is_key(mm, "tenth") ? "is" : "isn't");
    __mm_set(&mm, "sixth", f5);
    __mm_set(&mm, "seventh", f6);
    __mm_set(&mm, "eighth", f7);
    __mm_set(&mm, "ninth", f8);
    __mm_set(&mm, "tenth", f9);
    printf("f1 %s value\n", mm_is_value(mm, f1) ? "is" : "isn't");
    printf("f12 %s value\n", mm_is_value(mm, f12) ? "is" : "isn't");
    intermediate_test(mm);
    __mm_set(&mm, "eleventh", f10);
    __mm_set(&mm, "twelfth", f11);
    __mm_set(&mm, "thirteenth", f12);
    __mm_set(&mm, "fourteenth", f13);
    __mm_set(&mm, "fifteenth", f14);
    printf("fifth %s key\n", mm_is_key(mm, "fifth") ? "is" : "isn't");
    printf("bar %s key\n", mm_is_key(mm, "bar") ? "is" : "isn't");
    printf("tenth %s key\n", mm_is_key(mm, "tenth") ? "is" : "isn't");
    __mm_set(&mm, "sixteenth", f15);
    __mm_set(&mm, "seventeenth", f16);
    __mm_set(&mm, "eighteenth", f17);
    __mm_set(&mm, "nineteenth", f18);
    __mm_set(&mm, "twentieth", f19);
    printf("f1 %s value\n", mm_is_value(mm, f1) ? "is" : "isn't");
    printf("f12 %s value\n", mm_is_value(mm, f12) ? "is" : "isn't");
    
    intermediate_test(mm);
    __mm_remove(&mm, "eleventh");
    __mm_remove(&mm, "sixteenth");
    intermediate_test(mm);
    __mm_remove(&mm, "fifth");
    __mm_remove(&mm, "first");
    __mm_remove(&mm, "woof");
    intermediate_test(mm);
    mm_free(&mm);
}
