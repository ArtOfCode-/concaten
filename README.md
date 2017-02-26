#Concaten

Concaten is a concatenative, stack-based, optionally strictly but always strongly typed programming language.

That's about all I've got for right now; it's late and I need to sleep.

---

##Current milestone

### `ctn_object.h` - 0.1.0.1

The building block of the language. In Concaten, everything is either a token (an instruction to the parser) or an object (data stored by the parser). Even words, Concaten's equivalent for functions, are objects.

 - [ ] `prop_map.h`
    * PropMap is the property map in `Object`s. It'll also serve nicely as the template from which to draw when I'm writing `MethodMap` and, possibly, `ScopeStack` (much later)
    * [x] Define interface
    * [ ] Implement methods
    * [ ] Test methods
- [ ] `method_map.h`
    * Should be mostly the same as `prop_map`, with a different value.
    * [ ] Define interface (if different)
    * [ ] Implement methods (probably via copy/paste)
    * [ ] Test methods (even if mostly similar)
- [ ] `ctn_object.h`
    * [ ] Define specific tasks for this and beyond :)

###Previous milestones

- `tokenizer.h` - 0.0.0

    Converts a stream of characters into a stream of tokens. Can read from both a string in memory and a file on the disk directly. The former will make `eval` easier to code, and the latter makes the interpreter a little easier to write.

