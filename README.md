##Concaten v0.1.3

Concaten is a concatenative, stack-based, strongly but optionally strictly typed, hyperdynamic,
garbage-collected, interpreted programming language. In order, that means that:

1. **concatenative** - Each "word" (Concaten's version of a function) is applied to the results of the one
   that appears before it in the source code. There are a few exceptions, though; notably, `{` and `->`. 
2. **stack-based** - Results, rather than being passed directly to methods or stored in variables, go onto
   an intermediary data stack, on which they can stay indefinitely. Note that this may occur in other
   languages like C, as part of the implementation of passing parameters to methods; however, Concaten's is
   persistent -- C's, if not empty after all the parameters have been passed to the method, is messing
   something up.
3. **strongly but optionally strictly typed** - Most methods related to variable assignment take a type as
   an optional parameter, and check that the datum to be stored is of that type. They are **optional**,
   however, and duck typing works fine in Concaten. At the same time, objects of one type are never
   implicitly converted to another type by the language -- though, of course, words, including those in the
   standard library, are free to. (c.f. all standard library words beginning `*`)
4. **hyperdynamic** - Every keyword is an overrideable (though *not* replaceable) word, and everything is an
   object that can have methods monkey-patched in or out. Every function's code can be read, analyzed, and
   changed at runtime. The one exception to this is an object's properties -- to allow fundamental objects
   and methods to be implemented in the interpreter's language, either for speed boosts or simplicity,
   properties are neither visible nor editable outside of that object's methods.
5. **garbage-collected** - You don't need to worry about memory; Concaten will handle it for you with a
   reference-counting garbage collector. Because of its nature, you can also usually rely on the GC deleting
   and destructing an object when you expect it to -- in comparison to tracing GCs, which run effectively
   whenever they want to, a refcounter imposes a constant, slight overhead in exchange for deleting an object
   as soon as it's no longer used, which means that destructors have meaning. As long as cyclical references
   are avoided, at least.
6. **interpreted** - Rather than being converted to machine language or assembly, then executed directly by
   the CPU, Concaten code is executed as-is by an intermediary program. Depending on the OS and specific
   circumstances, this might be abstracted away such that the user doesn't see it happening directly (e.g.
   through file extension assocations); under the hood, it always is.
0. **programming langauge** - Hopefully, if you're reading this, self-explanatory.

For an example of how the language will probably look, see `test.ctn`. However, keep in mind that this is
subject to change; though the language is specced out, actual usage might reveal things that I didn't
consider, and need to add in or change.

---

**A note about versions**: My format is `major.milestone.task`, where each is the number for the
most recently finished. For example, version `1.1.0` just completed the first task of milestone 2 of
verison 1 -- probably something about expanding the standard library. Major version `0` is pre-completion;
some of the parts may be done, but not all of it. Major version `1` is the first completed version of
the language; from there, I'll iterate based on feedback and input from the users.

You may sometimes see the version as `x.x.x-[name]`. This indicates that it's not the version from the
master branch, but some other branch. You'll only see this in the results of `concaten --version`; it
doesn't appear in the git repo READMEs.

###Current milestone

#### `ctn_object.h` - 0.1

The building block of the language. Even lambdas are manipulatable objects.

- [x] `prop_map.h` - 0.1.0  
  PropMap is the property map in `Object`s. It'll also serve nicely as the template from which to draw
    when I'm writing `MethodMap` and, possibly, `ScopeStack` (much later).
  * [x] Define interface
  * [x] Implement methods  
    Initially implemented with `int` values, but a macro should make switching
      those out for Object when that's complete easy.
  * [x] Test methods
- [x] `method_map.h` - 0.1.1  
  Should be mostly the same as `prop_map`, with a different value type.
  * [x] Define interface (if different)
  * [x] Implement methods (probably via copy/paste)
  * [x] Test methods (even if mostly similar)
- [ ] `ctn_object.h`  
  The basic data container for the language. Most commonly seen *everywhere*.
  * For each of these, design/implement/test
  * [x] Type erasure - 0.1.2  
    An object which stores `void *` and size, and has function (macro?) to convert to/from arbitrary types.
  * [x] Refcounter - 0.1.3  
    An object should be freed once its references reach 0. The trick will be architecting things such that
      it can work like that, without being deleted when it's transferred from (for example) the data stack
      to C code that's just using its value.
  * [ ] Circular references - 0.1.4  
    The big downside to refcounters is that they don't catch circular references. I need to figure out
      a way to deal with that. Maybe a separate thread that occasionally looks through every object for
      cycles? Then I just need to do something fancy with mutexes to prevent modification while that object
      is getting checked, but I'd probably have had to do that to support multithreading anyway.
  * **NB**: Testing this thoroughly will be difficult, given the complexity of the code that's going to be
    using it. It may be better to test it indirectly, through known-working pieces that use it.

###Upcoming milestones

* [ ] `data_stack.h` - 0.2  
  The global data stack, used to pass data into and out of functions. Normal FIFO structure of `Object`s.
* [ ] `token_stack.h` - 0.3
  The stack from which tokens to evaluate are parsed. In the implementation, this isn't actually a stack,
    but it's used as one in Concaten code, so it doesn't really matter.
* [ ] `code_block.h` - 0.4  
  A simple object which represents Concaten's equivalent of anonymous functions. Can be run, in addition
    to being modified at runtime, as it's simply an array of token with some extra words.
* [ ] `ctn_runnable.h` - 0.3  
  A combination object so I can either define things in Concaten, through code blocks, or in C, through
    functions with a certain signature, and call them without worrying about which is which. This layer
    of abstraction will make it much easier to implement user-defined words.
* [ ] `scope_stack.h` - 0.6  
  Contains the list of words. This is a stack so we can sensibly implement things like local variables.
    Shouldn't be too much trouble; it'll mostly be combining `PropMap` and `DataStack`.
* [ ] Main method - 0.7
* [ ] Minimal standard library - 0.8  
  Words like `if`, `{`, and `puts` so we can play with the language at all.
* [ ] Thorough code review  
  I want to find every possible opportunity for improvement. This is in part to hunt down bugs and memory
    leaks, but also because this project is on my resume, and I want it to be as good as possible.
* [ ] Misc. required updates as needed
* [ ] Documentation - 1.0  
  Including demo code, a detailed up-to-date list of every default global word (like `if`) and what they
    do, as well as a brief overview of how Concaten works and how to install/run it.
* [ ] Debug mode - 1.1  
  Allows breakpoints (w/ `breakpoint`?) and stops execution when exception is thrown outside of a `try`.
* [ ] User-created libraries - 1.2  
  `namespace` word that uses some Concaten magic to let people define their own modules. This is in part
    to encourage other people to write ~~my code for me~~ extensions to Concaten, and in part to set up
    the framework for the next milestone(s).
* [ ] Decide which extension libraries to implement, scatter the rest over 2.0 and 3.0 as appropriate
  * Networking
  * File I/O
  * String manipulation
  * Math
  * Regex (that'll be a lot of effort)
  * GUI (ditto)
* [ ] Multithreading - 2.0  
  This is going to require something close to a complete rewrite, to make sure everything is as thread-safe
    as it can get.
  * [Task](https://msdn.microsoft.com/en-us/library/dd537609.aspx)s?
  * Asynchronous versions of the Networking and File I/O APIs.
  * Events? (technically possible already, but easier with multithreading)
* [ ] C code linking at runtime (like Python) - 3.0  
  That way, people can write Concaten libraries in C, for what'll probably be a slight speed boost, but
    also lower-level access to the OS than Concaten allows.

###Previous milestones

- [x] `tokenizer.h` - 0.0  
  Converts a stream of characters into a stream of tokens. That way, the interpreter doesn't need to
    even think about things like comments or whitespace -- it just takes the tokens and runs with them.  
  Can read from both a string in memory and a file on the disk directly. The former will make `eval`
    easier to code, and the latter makes the interpreter a little easier to write.

