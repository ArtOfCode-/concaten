#include <stdio.h>
#include "../tests.h"

#include "../object.h"

void test_object() {
    puts("startng test...");
    int val = 12;
    struct Object t1 = ctno_from(val, NULL);
    const char *name = "Foobar";
    struct Object t2 = ctno_literal(name, 7, NULL);
    struct Object t3 = ctno_dynamic(pm_new(8), NULL);
    ctno_set_prop(&t3, "1", &t1);
    ctno_set_prop(&t3, "2", &t2);
    puts("expect: 12 Foobar");
    printf("direct: %d %s\n", *ctno_to(t1, int), ctno_to(t2, char));
    struct Object t1_get1 = *ctno_get_prop(t3, "1");
    struct Object t2_get1 = *ctno_get_prop(t3, "2");
    printf("indirect: %d %s\n", *ctno_to(t1_get1, int), ctno_to(t2_get1, char));
    ctno_free(&t1);
    ctno_free(&t2);
    // they should still exist, because t3 has a reference to them!
    printf("freed: %d %s\n", *ctno_to(t1, int), ctno_to(t2, char));
    struct Object t1_get2 = *ctno_get_prop(t3, "1");
    struct Object t2_get2 = *ctno_get_prop(t3, "2");
    printf("retrieved: %d %s\n", *ctno_to(t1_get2, int), ctno_to(t2_get2, char));
    ctno_free(&t3);
    // and _now_ they should be gone.
}
