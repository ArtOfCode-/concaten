#ifndef CONCATEN_CTN_OBJECT_H
#define CONCATEN_CTN_OBJECT_H

#include "prop_map.h"
#include "method_map.h"

enum lit_type_id {
    TID_long,
    TID_double,
    TID_char, // AKA string (stores series of chars :P)
    TID_bool,
    TID_MethodMap,
    TID_CodeBlock,
    TID_DataStack,
    TID_TokenStack,
    // TODO ScopeStack,
    // TODO List,
    // TODO Map,
    
};

struct LiteralData {
    size_t size;
    void *value;
    enum lit_type_id type_id;
};

struct Object {
    union {
        struct PropMap properties;
        struct LiteralData literal;
    } data;
    bool is_literal;
    // NB: This intentionally stores a pointer to it, not a copy. That way, we can modify
    // the methods that every object created from a class has, by modifying the class's
    // central copy. If the object's specific version is modified, the methods are copied then.
    struct MethodMap *methods;
    size_t refcount;
    
    int error;
};

struct Object ctno_literal(const void *, size_t, enum lit_type_id id,
                           struct MethodMap *);
struct Object ctno_dynamic(const struct PropMap, struct MethodMap *);
struct Object ctno_copy(const struct Object);
// ctno_mk_* methods for every literal type; might be defined elsewhere
bool ctno_set_prop(struct Object *, const char *, struct Object *);
struct Object *ctno_get_prop(const struct Object, const char *);
// this has to be a macro so we get type-correct stuff done :/
#define ctno_to(ctno, type) \
    ((ctno).is_literal ? \
        ((ctno).data.literal.type_id == TID_##type ? \
            ((type *) (ctno).data.literal.value) \
        : NULL) \
    : NULL)
struct Object *ctno_claim(struct Object *);
void ctno_free(struct Object *);

// with reference to tokenizer.h
struct Token;
struct Object tkn_value(struct Token);

#endif //CONCATEN_CTN_OBJECT_H
