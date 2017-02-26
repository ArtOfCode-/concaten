##Concaten v0.1.1.0

Concaten is a concatenative, stack-based, optionally strictly but always strongly typed programming language.

For now, not much is done -- see "Previous milestones" for what's been done, and "Current milestone" for what
I'm working on right now.

---

> **A note about versions**: My format is `major.milestone.task.commit`, where each is the number for the
> most recently finished. For example, version `1.1.0.5` is 5 commits in to the first task of milestone 2 of
> verison 1 -- probably something about expanding the standard library. Major version `0` is pre-completion;
> some of the parts may be done, but not all of it. Major version `1` is the first version of the language;
> from there, I'll iterate based on feedback and input from the users.

###Current milestone

#### `ctn_object.h` - 0.1

The building block of the language. In Concaten, everything is either a token (an instruction to the parser)
or an object (data stored by the parser). Even words, Concaten's equivalent for functions, are objects.

- [x] `prop_map.h` - 0.1.0
  * PropMap is the property map in `Object`s. It'll also serve nicely as the template from which to draw
    when I'm writing `MethodMap` and, possibly, `ScopeStack` (much later)
  * [x] Define interface
  * [x] Implement methods
  * [x] Test methods
- [ ] `method_map.h` - 0.1.1
  * Should be mostly the same as `prop_map`, with a different value.
  * [ ] Define interface (if different)
  * [ ] Implement methods (probably via copy/paste)
  * [ ] Test methods (even if mostly similar)
- [ ] `ctn_object.h` 0.1.2
  * The basic data container for the language. Most commonly seen *everywhere*.
  * [ ] Define interface
  * [ ] Type erasure - 0.1.3
    * object which stores `void *` and size, and has function (macro?) to convert to or from arbitrary types.
      Note that *no type-checking is necessary*, due to how it'll be used. However, it'll make debugging
      easier if it's added, so it'll be worthwhile.
  * [ ] Refcounter - 0.1.4
    * An object should be freed once its references reach 0. The trick will be architecting things such that
      it can work like that, without being deleted when it's transferred from (for example) the data stack
      to C code that's just using its value.
  * [ ] Circular references - 0.1.5
    * The big downside to refcounters is that they don't catch circular references. I need to figure out
      a way to deal with that. Maybe something that keeps track of all objects with refcount > 0 and searches
      through their references to ensure no loops? Graph theory should have an answer.
  * [ ] Tests at every stage, but especially after 0.1.5
  


###Previous milestones

- [x] `tokenizer.h` - 0.0

  Converts a stream of characters into a stream of tokens. Can read from both a string in memory and a file
  on the disk directly. The former will make `eval` easier to code, and the latter makes the interpreter a
  little easier to write.

