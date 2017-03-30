#ifndef CONCATEN_CTN_OBJECT_H
#define CONCATEN_CTN_OBJECT_H

#include "error.h"

#include "prop_map.h"
#include "method_map.h"

struct LiteralData {
    size_t size;
    void *value;
};

struct Object {
    union {
        struct PropMap properties;
        struct LiteralData literal;
    } data;
    bool is_literal;
    // NB: This intentionally stores a pointer to it, not a copy. That way, we
    // can modify the methods that every object created from a class has, by
    // modifying the class's central copy. If the object's specific version is
    // modified, the methods are copied then.
    struct MethodMap *methods;
    size_t refcount;
};

ERROR ctno_literal(const void *, size_t, struct MethodMap *, struct Object *);
ERROR ctno_dynamic(const struct PropMap, struct MethodMap *, struct Object *);
ERROR ctno_copy(const struct Object, struct Object *);
ERROR ctno_set_prop(struct Object *, const char *, struct Object *);
ERROR ctno_get_prop(const struct Object, const char *, struct Object *);

// TODO create literal "constructors" for:
// signed long long
// signed double
// char */size_t (or separate `string` struct?)
// DataStack
// TokenStack
// CodeBlock
// List
// Map
// StringBuilder
// etc. etc.

#define ctno_to(ctno, type) \
    ((ctno).is_literal ? ((type *) (ctno).data.literal.value) : NULL)
ERROR ctno_claim(struct Object *);
void ctno_free(struct Object *);

struct Token;
// consumes the Token, no output to out param if error
ERROR tkn_value(struct Token *, struct Object *);

#endif //CONCATEN_CTN_OBJECT_H
