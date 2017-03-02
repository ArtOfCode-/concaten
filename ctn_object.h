#ifndef CONCATEN_CTN_OBJECT_H
#define CONCATEN_CTN_OBJECT_H

#include "prop_map.h"
#include "method_map.h"

struct Object {
    union {
        struct PropMap properties;
        struct {
            size_t size;
            void *value;
        } literal;
    } data;
    bool is_literal;
    // NB: This intentionally stores a pointer to it, not a copy. That way, we can modify
    // the methods that every object created from a class has, by modifying the class's
    // central copy. If the object's specific version is modified, the methods are copied then.
    struct MethodMap *methods;
    
    int error;
};

#define ctno_from(sth, mthds) ctno_literal(&sth, sizeof(sth), mthds)
struct Object ctno_literal(const void *, size_t, struct MethodMap *);
struct Object ctno_dynamic(const struct PropMap, struct MethodMap *);
struct Object ctno_copy(const struct Object);
// ctno_mk_* methods for every literal type; might be defined elsewhere
#define p_ctno_to(ctno, type) (ctno)->is_literal ? ((type *) (ctno)->value) : NULL;
#define ctno_to(ctno, type, invalid) (cnto).is_literal ? *((type *) (ctno).value) : (invalid);
struct Object ctno_free(struct Object *);

// with reference to tokenizer.h
struct Token;
struct Object tkn_to_ctno(struct Token);

#endif //CONCATEN_CTN_OBJECT_H
