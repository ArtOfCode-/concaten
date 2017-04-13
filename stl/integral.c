#include "integral.h"
#include "../method_map.h"
#include "../object.h"

ERROR to_s(struct DataStack *d, struct ScopeStack *s, struct TokenStack *t) {
    (void)s;(void)t;
    struct Object *top;
    ERROR pop_err = dst_pop(d, &top);
    if (pop_err != NO_ERROR) {
        return pop_err;
    }
    integral val = ctno_to()
    size_t len = snprintf(NULL, 0, )
}

struct MethodMap *integral_methods = NULL;
void init_integral_methods() {
    
}
