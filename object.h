#ifndef CONCATEN_CTN_OBJECT_H
#define CONCATEN_CTN_OBJECT_H

#include "error.h"

#include "prop_map.h"
#include "method_map.h"

typedef signed long long Integral;
typedef double Real;
typedef bool Boolean;

#define all_types_X \
    X(integral, Integral) \
    X(real, signed Real) \
    X(boolean, bool) \
    X(dataStack, struct DataStack) \
    X(tokenStack, struct TokenStack) \
//    X(runnable, struct Runnable)
//    X(scopeStack, struct ScopeStack)
//    X(list, struct List)
//    X(map, struct Map)
//    X(regex, struct Regex)

#define X(Special, _) LTL_##Special, // of type _
enum LiteralType {
    all_types_X
    LTL_string
};
#undef X

struct LiteralData {
    size_t size;
    enum LiteralType type;
    void *value;
    void (*val_free)(void *);
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

ERROR ctno_literal(const void *, size_t, enum LiteralType, struct MethodMap *,
                   struct Object *);
ERROR ctno_dynamic(const struct PropMap, struct MethodMap *, struct Object *);
ERROR ctno_copy(const struct Object, struct Object *);
bool ctno_eq(const struct Object, const struct Object);
ERROR ctno_set_prop(struct Object *, const char *, struct Object *);
ERROR ctno_get_prop(const struct Object, const char *, struct Object *);
#define ctno_to(ctno, type) \
    (((ctno).is_literal && (ctno).data.literal.type == LTL_##type) \
        ? ((type *) (ctno).data.literal.value) \
        : NULL)
ERROR ctno_claim(struct Object *);
void ctno_free(struct Object *);

struct Token;
// consumes the Token, no output to out param if error
ERROR tkn_value(struct Token *, struct Object *);

#endif //CONCATEN_CTN_OBJECT_H
