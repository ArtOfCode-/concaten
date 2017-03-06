#include "../tests.h"

#include "../object.h"

void test_object() {
    int val = 0;
    struct Object t1 = ctno_from(val, NULL); ++val;
    struct Object t2 = ctno_from(val, NULL); ++val;
    struct PropMap props = pm_new(8);
    const char *name = "Foobar";
    struct Object t3_1 = ctno_from(name, NULL);
    pm_set(&props, "1", &t3_1);
    struct Object t3_2 = ctno_from(val, NULL); ++val;
    
    
}
