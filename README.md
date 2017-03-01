##Concaten v0.1.1.0

Concaten is a concatenative, stack-based, strongly but optionally strictly typed, hyperdynamic,
garbage-collected programming language. In order, that means that:

1. **concatenative** - Each "word" (Concaten's version of a function) is applied to the results of the one
   that appears before it in the source code. There are a few exceptions, though; notably, `{` and `->`. 
2. **stack-based** - Results, rather than being passed directly to methods or stored in variables, go onto
   an intermediary data stack, on which they can stay indefinitely. Note that this may occur in other
   languages like C, as part of the implementation of passing parameters to methods; however, Concaten's is
   persistent.
3. **strongly but optionally strictly typed** - Most methods related to variable assignment take a type as
   an optional parameter, and check that the datum to be stored is of that type. However, they are optional
   parameters, and duck typing works fine in Concaten. At the same time, objects of one type are never
   implicitly converted to another type by the language -- though, of course, words, including those in the
   standard library, are free to. (c.f. `puts*`, and indeed all standard library words ending in `*`)
4. **hyperdynamic** - Every keyword is an overrideable (though *not* replaceable) word, and everything is an
   object that can have methods monkey-patched in or out. Every function's code can be read, analyzed, and
   changed at runtime. The one exception to this is an object's properties -- to allow fundamental objects
   and methods to be implemented in the interpreter's language, either for speed boosts or simplicity,
   properties are neither visible nor editable outside of that object's methods.
5. **garbage-collected** - You don't need to worry about memory; Concaten will handle it for you with a
   reference-counting garbage collector. Because of its nature, you can also usually rely on the GC deleting
   and destructing an object when you expect it to -- in comparison to tracing GCs, which run effectively
   whenever they want to, a refcounter imposes a constant, slight overhead in exchange for deleting an object
   as soon as it's no longer used, which means that destructors have meaning. Cyclical references are a
   problem, but not an overwhelming one.
0. **programming langauge** - Hopefully, if you're reading this, self-explanatory.

For an example of how the language will probably look, see `test.ctn`. However, keep in mind that this is
subject to change; though the language is specced out, actual usage might reveal things that I didn't
consider, and need to add in or change.

---

> **A note about versions**: My format is `major.milestone.task`, where each is the number for the
> most recently finished. For example, version `1.1.0` just completed the first task of milestone 2 of
> verison 1 -- probably something about expanding the standard library. Major version `0` is pre-completion;
> some of the parts may be done, but not all of it. Major version `1` is the first completed version of
> the language; from there, I'll iterate based on feedback and input from the users.

> You may sometimes see the version as `x.x.x-[name]`. This indicates that it's not the version from the
> master branch, but some other branch. You'll only see this in the results of `concaten --version`; it
> doesn't appear in the git repo READMEs.

###Current milestone

#### `ctn_object.h` - 0.1

The building block of the language. In Concaten, everything is either a token (an instruction to the parser)
or an object (data stored by the parser). Even words, Concaten's equivalent for functions, are objects.

- [x] `prop_map.h` - 0.1.0
  * PropMap is the property map in `Object`s. It'll also serve nicely as the template from which to draw
    when I'm writing `MethodMap` and, possibly, `ScopeStack` (much later).
  * [x] Define interface
  * [x] Implement methods - NOTE: Initially implemented with `int` values, but a macro should make switching
        those out for Object when that's complete easy.
  * [x] Test methods
- [x] `method_map.h` - 0.1.1
  * Should be mostly the same as `prop_map`, with a different value type.
  * [x] Define interface (if different)
  * [x] Implement methods (probably via copy/paste)
  * [x] Test methods (even if mostly similar)
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
    * Remember to use [atomics](http://en.cppreference.com/w/c/atomic), to mitigate possible threading-related
      issues. If we decide to do threading in v1, at least.
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

